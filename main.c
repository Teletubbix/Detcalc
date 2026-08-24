/*
 * Detcalc —— 行列式计算器（支持至 20 阶）
 *   1) 输入「r k」        → 计算 [r×k] * [k×r]（结果 r×r）的行列式，如 2 4 / 4 2
 *   2) 输入维度序列「d0 d1 ... dm」→ 连乘 [d0×d1]*[d1×d2]*...*[dm-1×dm]（需 d0==dm）后取行列式
 *   3) 输入 n（1~20）     → 求 n×n 方阵的行列式（高斯消元 + 部分主元）
 *   命令行：./Detcalc 2 4  [-o 文件] ；./Detcalc 2 4 3 2 [-o 文件]；./Detcalc 3
 *   整数结果精确显示（不带浮点尾巴）；可导出到文件。
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAXN 20
#define EPS  1e-12
#define MAXD 64     /* 乘法链最多维度个数 */

/* 读取一个 rows×cols 矩阵（行优先，可跨多行输入） */
static void read_mat(double A[MAXN][MAXN], int rows, int cols) {
    printf("请输入 %d×%d 矩阵（共 %d 个数，行优先）：\n", rows, cols, rows * cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (scanf("%lf", &A[i][j]) != 1) A[i][j] = 0.0;
}

/* C = A(m×n) * B(n×p) */
static void mat_mul(double A[MAXN][MAXN], int m, int n,
                    double B[MAXN][MAXN], int p,
                    double C[MAXN][MAXN]) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) C[i][j] += A[i][k] * B[k][j];
        }
}

/* 整数结果精确显示：接近整数就打整数，否则保留有效数字 */
static void print_value(FILE *out, double v) {
    if (fabs(v - round(v)) < 1e-9) fprintf(out, "%.0f", round(v));
    else fprintf(out, "%.10g", v);
}

static void print_mat(FILE *out, double M[MAXN][MAXN], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) { print_value(out, M[i][j]); fputc('\t', out); }
        fputc('\n', out);
    }
}

/* 高斯消元 + 部分主元求行列式（原地修改 a；n 阶） */
static double det_gauss(double a[MAXN][MAXN], int n) {
    double d = 1.0;
    for (int i = 0; i < n; i++) {
        int p = i;
        for (int r = i + 1; r < n; r++)
            if (fabs(a[r][i]) > fabs(a[p][i])) p = r;
        if (fabs(a[p][i]) < EPS) return 0.0;   /* 奇异 */
        if (p != i) {
            for (int c = 0; c < n; c++) {
                double t = a[i][c]; a[i][c] = a[p][c]; a[p][c] = t;
            }
            d = -d;
        }
        d *= a[i][i];
        double piv = a[i][i];
        for (int r = i + 1; r < n; r++) {
            double f = a[r][i] / piv;
            for (int c = i; c < n; c++) a[r][c] -= f * a[i][c];
        }
    }
    return d;
}

/* 根据维度序列求解并输出（out=stdout 或导出文件） */
static void solve_spec(FILE *out, int *d, int ndims) {
    for (int i = 0; i < ndims; i++)
        if (d[i] < 1 || d[i] > MAXN) { fprintf(out, "阶数须在 1~%d 内。\n", MAXN); return; }

    /* —— n×n 方阵行列式 —— */
    if (ndims == 1) {
        int n = d[0];
        double M[MAXN][MAXN] = {0};
        read_mat(M, n, n);
        double det = det_gauss(M, n);
        fprintf(out, "矩阵：\n"); print_mat(out, M, n, n);
        fprintf(out, "det = "); print_value(out, det); fprintf(out, "\n");
        return;
    }

    /* —— [r×k] * [k×r] —— */
    if (ndims == 2) {
        int r = d[0], k = d[1];
        double A[MAXN][MAXN] = {0}, B[MAXN][MAXN] = {0}, C[MAXN][MAXN] = {0};
        fprintf(out, "-- 计算 [%d×%d] * [%d×%d] --\n", r, k, k, r);
        read_mat(A, r, k);
        read_mat(B, k, r);
        mat_mul(A, r, k, B, r, C);   /* C = [r×k] * [k×r] = [r×r] */
        double det = det_gauss(C, r);
        fprintf(out, "乘积矩阵 C = [%d×%d]：\n", r, r); print_mat(out, C, r, r);
        fprintf(out, "det = "); print_value(out, det); fprintf(out, "\n");
        return;
    }

    /* —— 乘法链：若干个矩阵连乘 —— */
    int m = ndims - 1;   /* 矩阵个数 = 维度个数 - 1 */
    if (d[0] != d[ndims - 1]) {
        fprintf(out, "乘法链需首尾维度相同(d0==d_last)才得到方阵，当前 d0=%d d_last=%d。\n",
                d[0], d[ndims - 1]);
        return;
    }
    fprintf(out, "-- 乘法链：");
    for (int i = 0; i < m; i++)
        fprintf(out, "[%d×%d]%s", d[i], d[i + 1], (i < m - 1 ? " * " : ""));
    fprintf(out, " --\n");

    double C[MAXN][MAXN] = {0};
    int rows = d[0], cols = d[1];
    read_mat(C, rows, cols);          /* 第一个矩阵 [d0×d1] */
    for (int i = 1; i < m; i++) {
        double M[MAXN][MAXN] = {0}, T[MAXN][MAXN] = {0};
        read_mat(M, d[i], d[i + 1]);
        mat_mul(C, rows, d[i], M, d[i + 1], T);
        for (int a = 0; a < rows; a++)
            for (int b = 0; b < d[i + 1]; b++) C[a][b] = T[a][b];
        cols = d[i + 1];
    }
    double det = det_gauss(C, rows);
    fprintf(out, "乘积矩阵 C = [%d×%d]：\n", rows, cols); print_mat(out, C, rows, cols);
    fprintf(out, "det = "); print_value(out, det); fprintf(out, "\n");
}

/* 解析一行里的所有整数（供交互式 REPL 用） */
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

    /* 解析命令行：`-o 文件` + 维度序列 */
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

    /* 交互式 */
    printf("Detcalc 行列式计算器（支持至 %d 阶）\n", MAXN);
    printf("  输入「r k」          → [r×k] * [k×r]，如 2 4 / 4 2\n");
    printf("  输入维度序列「d0 d1 …」→ 连乘（需 d0==d_last），如 2 4 3 2\n");
    printf("  输入 n（1~%d）       → n×n 方阵行列式\n", MAXN);
    printf("  命令行：./Detcalc 2 4 或 ./Detcalc 2 4 3 2  [-o 文件]\n");
    printf("  输入 0 退出\n");

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
