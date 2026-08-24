/*
 * 行列式计算器 —— 支持至 20 阶
 * 功能：
 *   1) 输入「r k」→ 计算 [r×k] * [k×r]（结果 r×r）的行列式，如 2 4 / 4 2
 *   2) 输入 n（1~20）→ 求 n×n 方阵的行列式（高斯消元 + 部分主元）
 * 参照 Calculator 矩阵模块的 det 思路，扩展至任意 n。
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define MAXN 20
#define EPS  1e-12

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

static void print_mat(double M[MAXN][MAXN], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) printf("%10.6g ", M[i][j]);
        printf("\n");
    }
}

/* 高斯消元 + 部分主元求行列式（原地修改 a；n 阶） */
static double det_gauss(double a[MAXN][MAXN], int n) {
    double d = 1.0;
    for (int i = 0; i < n; i++) {
        /* 找第 i 列绝对值最大的主元行 */
        int p = i;
        for (int r = i + 1; r < n; r++)
            if (fabs(a[r][i]) > fabs(a[p][i])) p = r;
        if (fabs(a[p][i]) < EPS) return 0.0;    /* 奇异矩阵 */
        if (p != i) {                            /* 交换行，行列式变号 */
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

int main(void) {
    printf("行列式计算器（支持至 %d 阶）\n", MAXN);
    printf("  输入「r k」→ 计算 [r×k] * [k×r]（结果 r×r）的行列式，例如 2 4 / 4 2\n");
    printf("  输入 n（1~%d）→ 求 n×n 方阵的行列式\n", MAXN);
    printf("  输入 0 → 退出\n");

    char line[512];
    while (1) {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;

        int a = 0, b = 0;
        int nargs = sscanf(line, "%d %d", &a, &b);
        if (nargs <= 0) {
            /* 跳过空行（scanf 读矩阵后留下的换行），仅对非空且无效的输入提示 */
            int has_chars = 0;
            for (const char *q = line; *q; q++)
                if (!isspace((unsigned char)*q)) { has_chars = 1; break; }
            if (has_chars) printf("输入无效，请重试。\n");
            continue;
        }

        if (nargs == 2) {
            int r = a, k = b;
            if (r < 1 || r > MAXN || k < 1 || k > MAXN) {
                printf("阶数须在 1~%d 内。\n", MAXN);
                continue;
            }
            double A[MAXN][MAXN] = {0}, B[MAXN][MAXN] = {0}, C[MAXN][MAXN] = {0};
            printf("-- 计算 [%d×%d] * [%d×%d] --\n", r, k, k, r);
            read_mat(A, r, k);
            read_mat(B, k, r);
            mat_mul(A, r, k, B, r, C);   /* C = A[r×k] * B[k×r] = [r×r] */
            printf("乘积矩阵 C = [%d×%d]：\n", r, r);
            print_mat(C, r, r);
            printf("det(C) = %.15g\n", det_gauss(C, r));
            continue;
        }

        int n = a;
        if (n == 0) { printf("再见！\n"); break; }
        if (n < 1 || n > MAXN) { printf("阶数须在 1~%d 内。\n", MAXN); continue; }
        double M[MAXN][MAXN] = {0};
        read_mat(M, n, n);
        printf("矩阵：\n");
        print_mat(M, n, n);
        printf("det = %.15g\n", det_gauss(M, n));
    }
    return 0;
}
