# Detcalc 项目规划

C 语言行列式计算器（≤20 阶），沿用 GitHub Actions 自动化（CI + release-please + 自动 Release）。

## 已完成
- [r×k] * [k×r] 矩阵乘法后取行列式（如 2 4 / 4 2）
- 任意维度序列乘法链（如 2 4 3 2 → [2×4]*[4×3]*[3×2]）
- 一般 n×n 方阵行列式（高斯消元+部分主元，≤20 阶）
- 命令行传参（`./Detcalc 2 4` / `./Detcalc 2 4 3 2` / `./Detcalc 3`）、`-o 文件` 导出
- 整数精确显示

## 规划步骤
- **S1 分数精确值**：有理数(num/den)类型 + 精确行列式（Bareiss 算法，整数矩阵免约分）+ 分数输入/输出
- **S2 符号/正负号**：det 的正/负/零标记；乘法链每步结果符号
- **S3 复杂输入**：分数(1/3)、小数、科学计数(1e-3)解析；按行/块输入；从文件读矩阵
- **S4 发布**：release-please 自动升版本（feat→minor / fix→patch），合并 Release PR → tag → 自动构建 Linux+Windows CLI 并上传

## 构建 / 测试
```bash
make        # 构建 Detcalc
make test   # 自动测试
```

## 版本
由 release-please 依据 Conventional Commits 自动管理（CHANGELOG 自动生成）。
