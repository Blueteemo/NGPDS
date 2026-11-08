# NGPDS — Blueteemo 维护分支

[English](README.md)

NGPDS 是由 **Fredrik Ahlström（FluBBaOfWard）** 开发、运行于 Nintendo DS 系列设备的 Neo Geo Pocket / Neo Geo Pocket Color 模拟器。本仓库是非官方 Fork，用于保存和整理《秋之回忆 Pure》《无限轮回：Cure.》测试过程中形成的兼容性、渲染性能及存档可靠性改进。

[原项目](https://github.com/FluBBaOfWard/NGPDS) · [保留的上游使用与菜单说明（英文）](Docs/UPSTREAM_README.md)

## 三语菜单测试分支

本分支在联合修复与石墨边框上，加入统一 12px 的英文／简体中文／日文菜单及菜单外关闭副屏选项。“关于”区分原作者与分支维护，按键、组件信息收进详情页；修正只配置另一机型 BIOS 时启动按 B 卡住的问题。见[操作、构建与验证边界](Docs/TRILINGUAL_MENU.md)。这是独立测试候选，未替换默认稳定分支，也未修改上游 PR。

## 选择分支

| 分支 | 内容 |
| --- | --- |
| [`personal/combined-stable`](https://github.com/Blueteemo/NGPDS/tree/personal/combined-stable) | 已验收的联合维护源码，包含 Pure/Cure 图形、性能、Flash 存档及配置改进；本轮更新文档。 |
| `main` | 上游程序基线加 Fork 文档，**不包含联合修复**。 |
| [`fix/raster-graphics-performance`](https://github.com/Blueteemo/NGPDS/tree/fix/raster-graphics-performance) | 图形与性能独立提案，对应 [PR #15](https://github.com/FluBBaOfWard/NGPDS/pull/15)。 |
| [`fix/flash-autosave-persistence`](https://github.com/Blueteemo/NGPDS/tree/fix/flash-autosave-persistence) | 存档与配置独立提案，对应 [PR #14](https://github.com/FluBBaOfWard/NGPDS/pull/14)。 |

联合分支保留验收提交 `8b9a49b2ebb1f37d9e49a835c5f833ca41ec58f2` 和固定依赖。后续独立滚动边界实验、精简存档候选均未混入。两份上游 PR 尚未合并。本轮只公开既有联合源码并更新文档，没有发布新的模拟器二进制 Release。

## 联合分支改进

- **《秋之回忆 Pure》**：改善已测人物转场、背景逐行滚动及资料页文字；缓存精灵输入、跳过重复合成与上传，降低渲染开销。
- **《无限轮回：Cure.》**：修正已测正式剧情的正文缺失，保留已验证的 CG、背景及图层行为。
- **Flash 与设置**：增加自动保存选项、配置持久化、替换前校验和恢复副本，避免重复写入相同 Flash 内容；兼容已测试的旧 `.fla`、`.sta`、`settings.cfg`，包括合法的 8 字节空 Flash 记录。

图形修改处理通用扫描线和寄存器行为，不按游戏 ID、固定帧号或截图硬编码，也不通过改变声音合成或时钟掩盖性能问题。精灵合成器本身不新增 DMA 通道或 HBlank CPU 中断；背景底色逐行回放使用 DMA1，调色板使用 DMA2/DMA3、接续既有 VBlank 传输，DMA0 保留滚动路径。

联合分支的新配置默认开启 Flash 和设置自动保存；旧配置中明确的开关选择继续保留。**升级程序不会把已有 Off 强制改成 On。** 若 Flash 自动保存关闭，离开前应执行 **File → Save Flash**，将游戏内进度写入卡中。

## 验证范围与限制

联合基线完成自动化模拟检查、存档故障注入、配置测试及音频对照。测试集中在日文版和当时使用的汉化 ROM；不同汉化补丁、ROM 扩容版本、其他游戏及未测场景不能直接沿用结论。

较早的优化 C 版曾由用户在 NDSi＋R4、普通 DS/NTR 模式下确认体验改善。这是历史反馈，**不代表最终联合程序及后续存档修改获得新一轮实机通过**。文件保护不保证任意 SD 卡故障或突然断电后均可恢复；参考核心匹配不等于真实硬件证明，也不宣称全面兼容或时序完全准确。

联合分支的实现说明：[图形](https://github.com/Blueteemo/NGPDS/blob/personal/combined-stable/Docs/raster.md)、[存档](https://github.com/Blueteemo/NGPDS/blob/personal/combined-stable/Docs/persistence.md)。

## 构建与使用

```sh
git clone --recursive --branch personal/combined-stable https://github.com/Blueteemo/NGPDS.git
cd NGPDS
# 先配置已有 Wonderful / BlocksDS 环境。
make -j8 COMPDB=0
```

验收构建使用 Wonderful arm-none-eabi GCC 16.2.0、BlocksDS 1.24.0；`COMPDB=0` 仅关闭编译数据库合并，不改变代码生成。复现该基线时保留依赖固定版本。

将 `NGPDS.nds` 放入卡中，保留 `ngpds/` 数据目录，自备游戏和所需 BIOS。替换程序前备份存档与配置。菜单操作、BIOS 选择和原版 DS/DS Lite 内存限制见[上游原说明](Docs/UPSTREAM_README.md)；其中的游戏问题列表标记的是**上游基线**，不应视为维护分支的测试结论。联合分支另有 **Autosave Flash** 设置。

## 原创诊断与署名

[ngpc-scroll-tests](https://github.com/Blueteemo/ngpc-scroll-tests) 单独提供原创诊断 ROM、源码、构建方法与参考观察。该诊断项目采用 MIT，不代表 NGPDS 本体采用 MIT；后续最小 K2GE 实验未并入本联合基线。

NGPDS 原作者署名、完整 Credits 和版权声明保留在英文主页及上游原文，各依赖声明保留在对应源码中。本 Fork 不代表原作者认可，也不为上游代码重新指定许可证。不提供商业游戏 ROM、BIOS、私人存档或版权游戏截图。

原作者：**Fredrik Ahlström（FluBBaOfWard）**。完整致谢见 [Credits](README.md#credits)。
