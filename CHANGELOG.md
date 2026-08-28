# Changelog

## [0.2.1](https://github.com/Teletubbix/Detcalc/compare/v0.2.0...v0.2.1) (2026-08-28)


### Bug Fixes

* release 上传加 --repo(避免 无 git 仓库导致 gh 无法探测) ([d30a8ab](https://github.com/Teletubbix/Detcalc/commit/d30a8ab7b18e51324cc9eaa3226171252c636979))

## [0.2.0](https://github.com/Teletubbix/Detcalc/compare/v0.1.0...v0.2.0) (2026-08-24)


### Features

* 分数精确值（有理数运算）—— 元素支持分数/整数/小数，行列式与矩阵乘法全程精确 ([29b8d18](https://github.com/Teletubbix/Detcalc/commit/29b8d18d436e28915b1eae0f828dc2e3cdd4f96e))
* 复杂输入格式 —— 支持科学计数(1e-3)、块式/分隔符输入([1 2;3 4])、-f 文件读矩阵 ([85d74eb](https://github.com/Teletubbix/Detcalc/commit/85d74eb02286a5daea57c6438d585a7c6bc86e51))
* 符号/正负号分析 —— det 标记正/负/零(不可逆) ([796ea41](https://github.com/Teletubbix/Detcalc/commit/796ea41c1cacd80241becbfbee847d5982a352de))


### Bug Fixes

* release.yml 的 shell 值加引号(msys2 {0} 被 YAML 误解析) ([c6b2b1b](https://github.com/Teletubbix/Detcalc/commit/c6b2b1bbc9676d21b923874af6b6f822c6d56aeb))
* 修正 Frac 数组初始化告警(改为 {0} 清零) ([b827115](https://github.com/Teletubbix/Detcalc/commit/b827115dd72b280c61248408101f09d5f774973c))
* 先打印原矩阵再求行列式(det_gauss 原地修改，之前显示的是化简后矩阵) ([157ce17](https://github.com/Teletubbix/Detcalc/commit/157ce17efc676f3fde5c4395886ea6fce13b2ec9))
