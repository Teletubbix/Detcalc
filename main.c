/*
 * Detcalc —— 行列式计算器（支持至 20 阶，精确有理数运算）
 *   1) 输入「r k」        → [r×k] * [k×r]（结果 r×r）的行列式，如 2 4 / 4 2
 *   2) 输入维度序列「d0 d1 ... dm」→ 连乘（需 d0==dm）后取行列式
 *   3) 输入 n（1~20）     → n×n 方阵的行列式
 *   元素支持分数(1/3)、整数、小数(1.5)；结果精确（整数直接显示、分数显示 a/b）
 *   命令行：./Detcalc 2 4 [-o 文件] ；./Detcalc 2 4 3 2 ；./Detcalc 3  [-o 文件]
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAXN 20
#define MAXD 64

/* —— 精确有理数（long long 分子/分母，每次运算后约分） —— */
typedef struct { long long num, den; } Frac;

static long long gcd_ll(long long a, long long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { long long t = a % b; a = b; b = t; }
    return a;
}

/* 规范化：分母为正，约到最简 */
static Frac frac_norm(long long num, long long den) {
    if (den == 0) { return (Frac){0, 1}; }        /* 除零兜底 */
    if (den < 0) { num = -num; den = -den; }
    long long g = gcd_ll(num, den);
    if (g) { num /= g; den /= g; }
    return (Frac){num, den};
}
static Frac frac_make(long long num, long long den) { return frac_norm(num, den); }
static Frac frac_add(Frac a, Frac b) { return frac_norm(a.num * b.den + b.num * a.den, a.den * b.den); }
static Frac frac_sub(Frac a, Frac b) { return frac_norm(a.num * b.den - b.num * a.den, a.den * b.den); }
static Frac frac_mul(Frac a, Frac b) { return frac_norm(a.num * b.num, a.den * b.den); }
static Frac frac_div(Frac a, Frac b) { return frac_norm(a.num * b.den, a.den * b.num); }
/* |a| 是否大于 |b|（交叉相乘避免浮点） */
static int frac_abs_gt(Frac a, Frac b) {
    long long x = a.num * (a.num < 0 ? -1 : 1) * b.den;
    long long y = b.num * (b.num < 0 ? -1 : 1) * a.den;
    return x > y;
}

/* 解析一个 token 为分数：支持 a/b、整数、小数(1.5)。失败返回 0，成功返回 1 */
static int parse_frac(const char *s, Frac *out) {
    const char *slash = strchr(s, '/');
    if (slash) {
        char a[64], b[64];
        size_t la = (size_t)(slash - s), lb = strlen(slash + 1);
        if (la == 0 || la >= sizeof(a) - 1 || lb == 0 || lb >= sizeof(b) - 1) return 0;
        memcpy(a, s, la); a[la] = '\0';
        memcpy(b, slash + 1, lb); b[lb] = '\0';
        char *ea, *eb;
        long long na = strtoll(a, &ea, 10), nb = strtoll(b, &eb, 10);
        if (ea == a || eb == b || nb == 0) return 0;
        *out = frac_make(na, nb);
        return 1;
    }
    if (strchr(s, '.')) {
        long long num = 0, scale = 1;
        const char *p = s;
        int neg = (*p == '-'); if (neg || *p == '+') p++;
        int seen_dot = 0;
        for (; *p; p++) {
            if (*p == '.') { if (seen_dot) return 0; seen_dot = 1; continue; }
            if (!isdigit((unsigned char)*p)) return 0;
            num = num * 10 + (*p - '0');
            if (seen_dot) scale *= 10;
        }
        *out = frac_make(neg ? -num : num, scale);
        return 1;
    }
    char *e;
    long long v = strtoll(s, &e, 10);
    if (e == s) return 0;
    *out = (Frac){v, 1};
    return 1;
}

static int read_elem(Frac *f) {
    char tok[128];
    if (scanf("%127s", tok) != 1) return 0;
    return parse_frac(tok, f);
}
static void read_mat(Frac A[MAXN][MAXN], int rows, int cols) {
    printf("请输入 %d×%d 矩阵（共 %d 个数，行优先；支持分数如 1/2）：\n", rows, cols, rows * cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (!read_elem(&A[i][j])) A[i][j] = (Frac){0, 1};
}

/* C = A(m×n) * B(n×p) */
static void mat_mul(Frac A[MAXN][MAXN], int m, int n, Frac B[MAXN][MAXN], int p, Frac C[MAXN][MAXN]) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++) {
            C[i][j] = (Frac){0, 1};
            for (int k = 0; k < n; k++) C[i][j] = frac_add(C[i][j], frac_mul(A[i][k], B[k][j]));
        }
}

static void fstr(Frac f, char *out, size_t sz) {
    if (f.den == 1) snprintf(out, sz, "%lld", f.num);
    else snprintf(out, sz, "%lld/%lld", f.num, f.den);
}

static void print_mat(FILE *out, Frac M[MAXN][MAXN], int rows, int cols) {
    char b[64];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) { fstr(M[i][j], b, sizeof(b)); fprintf(out, "%12s", b); }
        fputc('\n', out);
    }
}

/* 高斯消元 + 部分主元（精确有理数）求行列式；原地修改 a */
static Frac det_gauss(Frac a[MAXN][MAXN], int n) {
    Frac d = {1, 1};
    for (int i = 0; i < n; i++) {
        int p = i;
        for (int r = i + 1; r < n; r++)
            if (frac_abs_gt(a[r][i], a[p][i])) p = r;
        if (a[p][i].num == 0) return (Frac){0, 1};
        if (p != i) {
            for (int c = 0; c < n; c++) { Frac t = a[i][c]; a[i][c] = a[p][c]; a[p][c] = t; }
            d = frac_mul(d, (Frac){-1, 1});
        }
        d = frac_mul(d, a[i][i]);
        Frac piv = a[i][i];
        for (int r = i + 1; r < n; r++) {
            Frac f = frac_div(a[r][i], piv);
            for (int c = i; c < n; c++)
                a[r][c] = frac_sub(a[r][c], frac_mul(f, a[i][c]));
        }
    }
    return d;
}

static void solve_spec(FILE *out, int *d, int ndims) {
    for (int i = 0; i < ndims; i++)
        if (d[i] < 1 || d[i] > MAXN) { fprintf(out, "阶数须在 1~%d 内。\n", MAXN); return; }

    if (ndims == 1) {
        int n = d[0];
        Frac M[MAXN][MAXN] = {0};
        read_mat(M, n, n);
        Frac det = det_gauss(M, n);
        char b[64];
        fprintf(out, "矩阵：\n"); print_mat(out, M, n, n);
        fstr(det, b, sizeof(b));
        fprintf(out, "det = %s\n", b);
        return;
    }
    if (ndims == 2) {
        int r = d[0], k = d[1];
        Frac A[MAXN][MAXN] = {0}, B[MAXN][MAXN] = {0}, C[MAXN][MAXN] = {0};
        fprintf(out, "-- 计算 [%d×%d] * [%d×%d] --\n", r, k, k, r);
        read_mat(A, r, k);
        read_mat(B, k, r);
        mat_mul(A, r, k, B, r, C);
        Frac det = det_gauss(C, r);
        char b[64];
        fprintf(out, "乘积矩阵 C = [%d×%d]：\n", r, r); print_mat(out, C, r, r);
        fstr(det, b, sizeof(b));
        fprintf(out, "det = %s\n", b);
        return;
    }
    int m = ndims - 1;
    if (d[0] != d[ndims - 1]) {
        fprintf(out, "乘法链需首尾维度相同(d0==d_last)才得到方阵，当前 d0=%d d_last=%d。\n", d[0], d[ndims - 1]);
        return;
    }
    fprintf(out, "-- 乘法链：");
    for (int i = 0; i < m; i++)
        fprintf(out, "[%d×%d]%s", d[i], d[i + 1], (i < m - 1 ? " * " : ""));
    fprintf(out, " --\n");

    Frac C[MAXN][MAXN] = {0};
    int rows = d[0], cols = d[1];
    read_mat(C, rows, cols);
    for (int i = 1; i < m; i++) {
        Frac M[MAXN][MAXN] = {0}, T[MAXN][MAXN] = {0};
        read_mat(M, d[i], d[i + 1]);
        mat_mul(C, rows, d[i], M, d[i + 1], T);
        for (int a = 0; a < rows; a++)
            for (int b = 0; b < d[i + 1]; b++) C[a][b] = T[a][b];
        cols = d[i + 1];
    }
    Frac det = det_gauss(C, rows);
    char b[64];
    fprintf(out, "乘积矩阵 C = [%d×%d]：\n", rows, cols); print_mat(out, C, rows, cols);
    fstr(det, b, sizeof(b));
    fprintf(out, "det = %s\n", b);
}

static int parse_int_line(const char *line, int *arr, int max) {
    int n = 0;
    const char *p = line;
    while (*p && n < max) {
        while (*p && !isdigit((unsigned char)*p) && *p != '-' && *p != '+') p++;
        if (!isdigit((unsigned char)*p)) break;
        char *end;
        long v = strtol(p, &end, 10);
        arr[n++] = (int)v;
        p = end;
    }
    return n;
}

int main(int argc, char **argv) {
    const char *outfile = NULL;
    int dims[MAXD];
    int ndims = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) { outfile = argv[i + 1]; i++; }
        else if (isdigit((unsigned char)argv[i][0]) ||
                 (argv[i][0] == '-' && isdigit((unsigned char)argv[i][1]))) {
            dims[ndims++] = (int)strtol(argv[i], NULL, 10);
        } else {
            fprintf(stderr, "未知参数: %s\n", argv[i]);
        }
    }
    if (ndims > 0) {
        FILE *out = outfile ? fopen(outfile, "w") : stdout;
        if (!out) { perror("打开输出文件失败"); return 1; }
        solve_spec(out, dims, ndims);
        if (outfile) { fclose(out); printf("已导出到 %s\n", outfile); }
        return 0;
    }

    printf("Detcalc 行列式计算器（支持至 %d 阶，精确分数运算）\n", MAXN);
    printf("  「r k」→ [r×k]*[k×r]；维度序列「d0 d1 …」→ 连乘(需 d0==d_last)；输入 n → n×n 行列式\n");
    printf("  元素可输分数(1/2)、整数、小数(1.5)。0 退出。\n");
    char line[512];
    while (1) {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        int nd = parse_int_line(line, dims, MAXD);
        if (nd <= 0) continue;
        if (nd == 1 && dims[0] == 0) { printf("再见！\n"); break; }
        solve_spec(stdout, dims, nd);
    }
    return 0;
}
