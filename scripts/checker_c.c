/*
 * Independent kissing-configuration checker (V2_checker)
 * Implements the spec in SPEC.md from scratch.
 *
 * Decision rule:  for every pair (x,y), x != y:
 *   valid iff  g <= 0  OR  4*g^2 <= Nx*Ny
 * where g = <x,y>, Nx = <x,x>, Ny = <y,y>
 *
 * All arithmetic in __int128 to avoid overflow for coordinates up to ~3e6
 * in dimension 21 with coefficients up to ~2.
 *
 * Overflow check:
 *   max |coord| ~ 3e6, max dim ~ 21, max coeff ~ 6
 *   max |g| <= 21 * 6 * (3e6)^2 = 21*6*9e12 = ~1.13e15  (fits int64)
 *   max g^2 <= (1.13e15)^2 = ~1.28e30  (fits __int128, max ~1.7e38)
 *   max 4*g^2 <= ~5.1e30               (fits __int128)
 *   max Nx,Ny <= 1.13e15               (fits int64)
 *   max Nx*Ny <= (1.13e15)^2 = ~1.28e30 (fits __int128)
 *
 * Parallelised with OpenMP over the outer pair loop, using at most 6 threads.
 *
 * Build:
 *   gcc -O2 -fopenmp -o checker checker.c
 *
 * Usage:
 *   ./checker <file>
 * Exit 0 iff valid.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

/* ------------------------------------------------------------------ */
/* __int128 overflow-safe multiplication                                */
/* We check: |a|, |b| <= 2^63, product fits in __int128               */
/* ------------------------------------------------------------------ */
static __int128 safe_mul128(__int128 a, __int128 b)
{
    /* __int128 range: -(2^127) to 2^127-1                            */
    /* Our values are at most ~5e30 < 2^103, well within range.       */
    return a * b;
}

/* ------------------------------------------------------------------ */
/* Parsing helpers                                                      */
/* ------------------------------------------------------------------ */

/* Skip whitespace except newlines */
static void skip_spaces(const char **p)
{
    while (**p == ' ' || **p == '\t' || **p == '\r')
        (*p)++;
}

/* Parse a signed 64-bit integer from *p, advance *p */
static int parse_int64(const char **p, int64_t *out)
{
    skip_spaces(p);
    const char *s = *p;
    int neg = 0;
    if (*s == '-') { neg = 1; s++; }
    else if (*s == '+') { s++; }
    if (!isdigit((unsigned char)*s)) return 0;
    uint64_t v = 0;
    while (isdigit((unsigned char)*s)) {
        uint64_t d = *s - '0';
        if (v > (UINT64_MAX - d) / 10) {
            fprintf(stderr, "Error: integer overflow during parsing\n");
            exit(2);
        }
        v = v * 10 + d;
        s++;
    }
    if (neg) {
        if (v > (uint64_t)INT64_MAX + 1) {
            fprintf(stderr, "Error: integer overflow during parsing (negative)\n");
            exit(2);
        }
        *out = -(int64_t)v;
    } else {
        if (v > (uint64_t)INT64_MAX) {
            fprintf(stderr, "Error: integer overflow during parsing\n");
            exit(2);
        }
        *out = (int64_t)v;
    }
    *p = s;
    return 1;
}

/* ------------------------------------------------------------------ */
/* Read entire file into memory                                         */
/* ------------------------------------------------------------------ */
static char *read_file(const char *path, size_t *len_out)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Error: cannot open '%s': %s\n", path, strerror(errno));
        exit(2);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) {
        fprintf(stderr, "Error: ftell failed\n");
        exit(2);
    }
    char *buf = malloc((size_t)sz + 1);
    if (!buf) {
        fprintf(stderr, "Error: out of memory\n");
        exit(2);
    }
    if ((size_t)sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fprintf(stderr, "Error: read failed\n");
        exit(2);
    }
    buf[sz] = '\0';
    fclose(f);
    if (len_out) *len_out = (size_t)sz;
    return buf;
}

/* ------------------------------------------------------------------ */
/* Detect format: certificate (starts with "Dimension:") or plain      */
/* ------------------------------------------------------------------ */
static int is_certificate_format(const char *buf)
{
    /* Skip leading whitespace */
    while (*buf == ' ' || *buf == '\t' || *buf == '\r' || *buf == '\n') buf++;
    return (strncmp(buf, "Dimension:", 10) == 0);
}

/* ------------------------------------------------------------------ */
/* Advance past rest of current line                                    */
/* ------------------------------------------------------------------ */
static void skip_line(const char **p)
{
    while (**p && **p != '\n') (*p)++;
    if (**p == '\n') (*p)++;
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 2;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    size_t flen;
    char *buf = read_file(argv[1], &flen);
    const char *p = buf;

    int dim = 0;
    int64_t header_count = -1;  /* -1 means "no header" */
    int64_t *coeffs = NULL;

    /* ---- Parse header (if certificate format) ---- */
    if (is_certificate_format(p)) {
        /* "Dimension: d" */
        while (*p && *p != ':') p++;
        if (!*p) { fprintf(stderr, "Error: malformed Dimension line\n"); return 2; }
        p++; /* skip ':' */
        skip_spaces(&p);
        int64_t d64;
        if (!parse_int64(&p, &d64) || d64 <= 0 || d64 > 100000) {
            fprintf(stderr, "Error: bad dimension value\n");
            return 2;
        }
        dim = (int)d64;
        skip_line(&p);

        /* "Number of points: M" */
        while (*p && *p != ':') p++;
        if (!*p) { fprintf(stderr, "Error: malformed Number of points line\n"); return 2; }
        p++;
        if (!parse_int64(&p, &header_count) || header_count < 0) {
            fprintf(stderr, "Error: bad point count\n");
            return 2;
        }
        skip_line(&p);

        /* "Inner product coefficients: c1,c2,...,cd" */
        while (*p && *p != ':') p++;
        if (!*p) { fprintf(stderr, "Error: malformed coefficients line\n"); return 2; }
        p++;
        skip_spaces(&p);
        coeffs = malloc((size_t)dim * sizeof(int64_t));
        if (!coeffs) { fprintf(stderr, "Error: out of memory\n"); return 2; }
        for (int i = 0; i < dim; i++) {
            if (!parse_int64(&p, &coeffs[i])) {
                fprintf(stderr, "Error: too few coefficients (expected %d)\n", dim);
                return 2;
            }
            if (coeffs[i] <= 0) {
                fprintf(stderr, "Error: coefficient %d is non-positive (%lld)\n", i, (long long)coeffs[i]);
                return 2;
            }
            if (i < dim - 1) {
                skip_spaces(&p);
                if (*p == ',') p++;
                else { fprintf(stderr, "Error: expected comma after coefficient %d\n", i); return 2; }
            }
        }
        skip_line(&p);

        /* "Points:" */
        while (*p) {
            const char *line_start = p;
            while (*p && *p != '\n') p++;
            /* check if this line is "Points:" */
            char tmp[16] = {0};
            size_t llen = (size_t)(p - line_start);
            if (llen >= 6) {
                memcpy(tmp, line_start, 6 < llen ? 6 : llen);
            }
            if (*p == '\n') p++;
            if (strncmp(tmp, "Points", 6) == 0) break;
        }
    } else {
        /* Plain format: no header, identity metric, detect dim from first line */
        /* Peek at first non-empty line to count dimensions */
        const char *peek = p;
        while (*peek == '\n' || *peek == '\r') peek++;
        const char *line_start = peek;
        while (*peek && *peek != '\n') peek++;
        /* Count whitespace-separated tokens */
        const char *q = line_start;
        int cnt = 0;
        while (q < peek) {
            while (q < peek && (isspace((unsigned char)*q))) q++;
            if (q < peek) {
                cnt++;
                while (q < peek && !isspace((unsigned char)*q)) q++;
            }
        }
        dim = cnt;
        if (dim <= 0) {
            fprintf(stderr, "Error: could not detect dimension from first data line\n");
            return 2;
        }
        /* Identity metric */
        coeffs = malloc((size_t)dim * sizeof(int64_t));
        if (!coeffs) { fprintf(stderr, "Error: out of memory\n"); return 2; }
        for (int i = 0; i < dim; i++) coeffs[i] = 1;
    }

    printf("Dimension: %d\n", dim);
    if (header_count >= 0)
        printf("Header count: %lld\n", (long long)header_count);

    /* ---- Read all vectors ---- */
    /* We store vectors as rows in a flat int64_t array */
    size_t capacity = 4096;
    int64_t *vecs = malloc(capacity * (size_t)dim * sizeof(int64_t));
    if (!vecs) { fprintf(stderr, "Error: out of memory\n"); return 2; }
    int64_t n = 0;

    /* Determine delimiter: certificate uses comma, plain uses whitespace */
    int use_comma = (header_count >= 0);  /* certificate format uses comma */

    while (*p) {
        /* Skip blank lines */
        skip_spaces(&p);
        if (*p == '\n') { p++; continue; }
        if (!*p) break;

        /* Parse d integers (comma-separated for certificate, space-separated for plain) */
        if (n == (int64_t)capacity) {
            capacity *= 2;
            int64_t *tmp2 = realloc(vecs, capacity * (size_t)dim * sizeof(int64_t));
            if (!tmp2) { fprintf(stderr, "Error: out of memory\n"); return 2; }
            vecs = tmp2;
        }

        int64_t *row = vecs + n * dim;
        int ok = 1;
        for (int i = 0; i < dim; i++) {
            skip_spaces(&p);
            if (!parse_int64(&p, &row[i])) {
                ok = 0;
                break;
            }
            if (i < dim - 1) {
                skip_spaces(&p);
                if (use_comma) {
                    if (*p == ',') p++;
                    else {
                        fprintf(stderr, "Error: expected comma at vector %lld component %d\n",
                                (long long)n+1, i);
                        ok = 0;
                        break;
                    }
                } else {
                    /* whitespace already skipped by next parse_int64 */
                }
            }
        }
        if (!ok) break;
        skip_line(&p);
        n++;
    }

    free(buf);

    printf("Vectors read: %lld\n", (long long)n);

    /* ---- Check header count ---- */
    if (header_count >= 0 && header_count != n) {
        printf("INVALID: header says %lld vectors but %lld found\n",
               (long long)header_count, (long long)n);
        free(vecs);
        free(coeffs);
        return 1;
    }

    /* ---- Check zero vectors ---- */
    for (int64_t i = 0; i < n; i++) {
        int64_t *x = vecs + i * dim;
        __int128 nx = 0;
        for (int k = 0; k < dim; k++) {
            __int128 t = (__int128)coeffs[k] * x[k] * x[k];
            nx += t;
        }
        if (nx == 0) {
            printf("INVALID: vector %lld is zero\n", (long long)i);
            free(vecs);
            free(coeffs);
            return 1;
        }
    }

    /* ---- Precompute norms ---- */
    __int128 *norms = malloc((size_t)n * sizeof(__int128));
    if (!norms) { fprintf(stderr, "Error: out of memory\n"); return 2; }
    for (int64_t i = 0; i < n; i++) {
        int64_t *x = vecs + i * dim;
        __int128 nx = 0;
        for (int k = 0; k < dim; k++) {
            nx += (__int128)coeffs[k] * x[k] * x[k];
        }
        norms[i] = nx;
    }

    /* ---- Set thread count ---- */
#ifdef _OPENMP
    int nthreads = 6;
    omp_set_num_threads(nthreads);
#endif

    /* ---- Check all pairs ---- */
    /* We need to find any violating pair and report it.              */
    /* Use a shared flag + pair indices for early exit.               */
    volatile int found_violation = 0;
    volatile int64_t viol_i = -1, viol_j = -1;

    #pragma omp parallel for schedule(dynamic, 64) shared(found_violation, viol_i, viol_j)
    for (int64_t i = 0; i < n; i++) {
        if (found_violation) continue;  /* skip remaining rows */
        const int64_t *xi = vecs + i * dim;
        __int128 ni = norms[i];

        for (int64_t j = i + 1; j < n; j++) {
            if (found_violation) break;

            const int64_t *xj = vecs + j * dim;
            __int128 nj = norms[j];

            /* Compute g = <xi, xj> */
            __int128 g = 0;
            for (int k = 0; k < dim; k++) {
                g += (__int128)coeffs[k] * xi[k] * xj[k];
            }

            /* Valid iff g <= 0 or 4*g^2 <= ni*nj */
            int valid;
            if (g <= 0) {
                valid = 1;
            } else {
                /* g > 0: check 4*g^2 <= ni*nj */
                /* Overflow check: ni, nj <= 21 * 6 * (3e6)^2 ~ 1.13e15 < 2^50 */
                /* ni*nj <= 2^100, fits in __int128 (max ~2^127)          */
                /* 4*g^2: g < ni+nj < 2*1.13e15 < 2^51, g^2 < 2^102      */
                /* 4*g^2 < 2^104, fits in __int128                        */
                __int128 g2 = g * g;
                /* Check 4*g2 won't overflow: 4*g2 < 4*(1.13e15)^2 ~5e30 < 2^103 */
                __int128 four_g2 = safe_mul128((__int128)4, g2);
                __int128 ninj = safe_mul128(ni, nj);
                valid = (four_g2 <= ninj);
            }

            if (!valid) {
                #pragma omp critical
                {
                    if (!found_violation) {
                        found_violation = 1;
                        viol_i = i;
                        viol_j = j;
                    }
                }
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) * 1e-9;

    if (found_violation) {
        /* Recompute the inner products to report */
        int64_t i = viol_i, j = viol_j;
        int64_t *xi = vecs + i * dim;
        int64_t *xj = vecs + j * dim;
        __int128 g = 0;
        for (int k = 0; k < dim; k++)
            g += (__int128)coeffs[k] * xi[k] * xj[k];
        __int128 ni = norms[i], nj = norms[j];

        /* Print violating pair */
        printf("INVALID\n");
        printf("Violating pair: vectors %lld and %lld\n",
               (long long)viol_i, (long long)viol_j);
        /* Print inner product as signed decimal */
        {
            __int128 g_print = g;
            if (g_print < 0) { printf("  <xi,xj> = -"); g_print = -g_print; }
            else printf("  <xi,xj> = ");
            /* Print __int128 */
            char tmp[50]; int tp = 49; tmp[tp] = '\0';
            if (g_print == 0) { tmp[--tp] = '0'; }
            else while (g_print > 0) { tmp[--tp] = '0' + (int)(g_print % 10); g_print /= 10; }
            printf("%s\n", tmp + tp);
        }
        /* print Nx, Ny */
        {
            __int128 v = ni; char tmp[50]; int tp = 49; tmp[tp] = '\0';
            if (v == 0) { tmp[--tp] = '0'; }
            else while (v > 0) { tmp[--tp] = '0' + (int)(v % 10); v /= 10; }
            printf("  <xi,xi> = %s\n", tmp + tp);
        }
        {
            __int128 v = nj; char tmp[50]; int tp = 49; tmp[tp] = '\0';
            if (v == 0) { tmp[--tp] = '0'; }
            else while (v > 0) { tmp[--tp] = '0' + (int)(v % 10); v /= 10; }
            printf("  <xj,xj> = %s\n", tmp + tp);
        }
        printf("Points: %lld\n", (long long)n);
        printf("Time: %.2f s\n", elapsed);
        free(vecs); free(coeffs); free(norms);
        return 1;
    }

    printf("VALID\n");
    printf("Points: %lld\n", (long long)n);
    printf("Time: %.2f s\n", elapsed);
    free(vecs); free(coeffs); free(norms);
    return 0;
}
