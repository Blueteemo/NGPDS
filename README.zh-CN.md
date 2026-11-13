# NGPDS — Fusion Edition

[English](README.md) · **简体中文** · [日本語](README.ja.md)

NGPDS 是 **Fredrik Ahlström（FluBBaOfWard）** 开发的 Neo Geo Pocket／Neo Geo Pocket Color 模拟器，运行于 Nintendo DS 系列设备。Fusion Edition 是千岚之夏维护的非官方分支。

v1.0.0 定稿在「关于」中显示 `V1.0.0 2026-11-13`。

[原项目](https://github.com/FluBBaOfWard/NGPDS) · [原版使用说明](Docs/UPSTREAM_README.md) · [早期联合修复分支](https://github.com/Blueteemo/NGPDS/tree/personal/combined-stable)

## 主要改进

- **图形兼容性**。《秋之回忆 Pure》改善了已测试的人物转场、背景逐行滚动和资料页文字。《无限轮回：Cure.》修正了已测试剧情中的正文缺失。渲染按扫描线状态工作，并通过缓存减少重复的精灵处理。 [图形说明](Docs/raster.md)
- **Flash 存档与设置**。支持自动保存、写入校验和备份恢复，相同内容不重复写盘。已测试的旧档包括合法的 8 字节空 Flash 存档。 [存档说明](Docs/persistence.md)
- **三语菜单**。英文、简体中文和日文共用 12 像素 Fusion Pixel 界面。触控操作、底栏提示和实体按键会随菜单模式切换。
- **外框与色彩**。遮罩可选石墨、经典或关闭。饱和度可选原色、微鲜艳（+15%）和鲜艳（+20%），原有亮度校正仍可独立调整。 [外框素材说明](Docs/GRAPHITE_BORDER.md)
- **音频**。「选项 → 通用 → 音频模式」提供标准（默认）和经典（旧版混音）。启动时使用短暂渐入，此前在 NDSi＋R4 实机上确认爆音已消失。 [启动音频说明](Docs/STARTUP_AUDIO.md)

## 安装和操作

1. 按下方步骤构建 `NGPDS.nds`，放入烧录卡。部分烧录卡需要打 DLDI 补丁。
2. 升级时保留 `ngpds/` 或 `data/ngpds/`，它们用于存放存档和设置。替换程序前先备份。
3. ROM 需自备。内置 HLE BIOS 可运行受支持的游戏并处理受支持的 Flash 存档。使用自备 BIOS 时，在「选项 → 主机」中选择。
4. 触摸下屏或同时按 **L+R** 打开菜单。在「选项 → 通用 → 界面语言」切换语言。

### 菜单和屏幕

默认用实体键或触控操作菜单。「打开菜单时暂停」决定游戏是否在菜单背后继续运行。关闭暂停后才显示「菜单中控制游戏」，这一项默认关闭。打开后，实体键操作已载入的游戏，菜单用触控操作。多页菜单可点右下角「下一页」。没有载入 ROM 时，菜单和文件列表可以用实体键操作。

「菜单外熄灭副屏」「打开菜单时暂停」「下屏显示游戏」是原版已有功能。熄屏选项会在关闭菜单后关掉闲置屏幕。如果游戏显示在下屏，熄灭的是上屏。

### 存档

新配置默认开启「自动保存游戏存档」和「自动保存设置」。升级时保留旧配置中的选择。关闭自动保存时，可在「文件」菜单手动保存游戏存档或设置。即时存档与游戏内 Flash 存档是两套功能。

## 从源码构建

v1.0.0 源码归档包含主仓库和七个固定版本的依赖。归档中的 `rebuild.sh` 可离线检出、构建并校验 `NGPDS.nds`。验证使用 Wonderful arm-none-eabi GCC 16.2.0 和 BlocksDS 1.24.0。 [构建命令](README.md#build)

## 验证范围

此前已在 NDSi＋R4 上验证菜单和启动爆音修复。自动化测试覆盖 Pure 和 Cure 日文版及汉化版的选定场景、菜单按键、旧存档和模拟写入故障。在 melonDS 上，Cure 日文版使用内置 HLE BIOS 完成了游戏内存档、冷启动和继续游戏。结论限于所测游戏和存档路径。真实 SD 卡突然断电后的恢复及鼓点还原度仍待验证。v0.2.3 基线已从干净检出重建，校验值与 R4 安装记录一致。v1.0.0 沿用该基线并更新了版本信息。

[菜单实现与验证记录](Docs/TRILINGUAL_MENU.md) · [原创滚动诊断 ROM](https://github.com/Blueteemo/ngpc-scroll-tests)

## 署名

- 原作者为 **Fredrik Ahlström（FluBBaOfWard）**。
- 分支维护为 **千岚之夏**。
- 界面使用 **Fusion Pixel**，其字体源码、署名与许可证随仓库保留。 [字体说明](third_party/fusion-pixel-12px/)

原作者和各依赖的版权及许可声明保留在源码中。ROM、BIOS 和个人存档由使用者自行提供。 [原版致谢](Docs/UPSTREAM_README.md) · [Credits](README.md#credits)
