# NGPDS — Fusion Edition

[English](README.md) · **简体中文** · [日本語](README.ja.md)

NGPDS 是 **Fredrik Ahlström（FluBBaOfWard）** 开发的 Neo Geo Pocket／Neo Geo Pocket Color 模拟器，运行于 Nintendo DS 系列设备。Fusion Edition 是千岚之夏维护的非官方分支。

当前验收构建在「关于」中显示 **V0.1.0 · 2026-11-11**。

[原项目](https://github.com/FluBBaOfWard/NGPDS) · [原版使用说明](Docs/UPSTREAM_README.md) · [早期联合修复分支](https://github.com/Blueteemo/NGPDS/tree/personal/combined-stable)

## 主要改进

- **图形兼容性**。《秋之回忆 Pure》改善了已测试的人物转场、背景逐行滚动和资料页文字。《无限轮回：Cure.》修正了已测试剧情中的正文缺失。渲染按扫描线状态工作，并通过缓存减少重复的精灵处理。 [图形说明](Docs/raster.md)
- **Flash 存档与设置**。支持自动保存、写入校验和备份恢复，相同内容不重复写盘。已测试的旧档包括合法的 8 字节空 Flash 存档。 [存档说明](Docs/persistence.md)
- **三语菜单**。英文、简体中文和日文共用 12 像素 Fusion Pixel 界面。触控操作、底栏提示和实体按键会随菜单模式切换。
- **外框与色彩**。遮罩可选石墨、经典或关闭。饱和度可选原色、微鲜艳（+15%）和鲜艳（+20%），原有亮度校正仍可独立调整。 [外框素材说明](Docs/GRAPHITE_BORDER.md)
- **启动声音**。加入启动渐入，用户在 NDSi＋R4 实机上确认启动爆音已消失。渐入结束后仍沿用原有游戏音频合成与时序。 [启动音频说明](Docs/STARTUP_AUDIO.md)

## 安装和操作

1. 按下方步骤构建 `NGPDS.nds`，放入烧录卡。部分烧录卡需要打 DLDI 补丁。
2. 升级时保留 `ngpds/` 或 `data/ngpds/`，它们用于存放存档和设置。替换程序前先备份。
3. ROM 和 BIOS 需自备。游戏内存档所需的 BIOS 见原版使用说明，在「选项 → 主机」选择对应 BIOS。 [原版使用说明](Docs/UPSTREAM_README.md)
4. 触摸下屏或同时按 **L+R** 打开菜单。在「选项 → 通用 → 界面语言」切换语言。

### 菜单和屏幕

默认用实体键或触控操作菜单。「打开菜单时暂停」决定游戏是否在菜单背后继续运行。关闭暂停后才显示「菜单中控制游戏」，这一项默认关闭。打开后，实体键操作已载入的游戏，菜单用触控操作。多页菜单可点右下角「下一页」。没有载入 ROM 时，菜单和文件列表可以用实体键操作。

「菜单外熄灭副屏」「打开菜单时暂停」「下屏显示游戏」是原版已有功能。熄屏选项会在关闭菜单后关掉闲置屏幕。如果游戏显示在下屏，熄灭的是上屏。

### 存档

新配置默认开启「自动保存游戏存档」和「自动保存设置」。升级时保留旧配置中的选择。关闭自动保存时，可在「文件」菜单手动保存游戏存档或设置。即时存档与游戏内 Flash 存档是两套功能。

## 从源码构建

源码在 `personal/trilingual-menu` 分支，依赖使用仓库固定的子模块版本。本次验证使用 Wonderful arm-none-eabi GCC 16.2.0 和 BlocksDS 1.24.0。 [构建命令](README.md#build)

## 验证范围

本轮菜单在 NDSi＋R4 上通过了用户试用。自动化验证覆盖 Pure 与 Cure 的日文原版及当时测试的汉化版本，以及选定场景、按键路由、旧存档和写入故障。其他 ROM 版本、设备及未测场景仍需各自验证。真实 SD 卡突然断电后的恢复尚未验证。鼓点和整体音色还原度另行研究。

[菜单实现与验证记录](Docs/TRILINGUAL_MENU.md) · [原创滚动诊断 ROM](https://github.com/Blueteemo/ngpc-scroll-tests)

## 署名

- 原作者为 **Fredrik Ahlström（FluBBaOfWard）**。
- 分支维护为 **千岚之夏**。
- 界面使用 **Fusion Pixel**，其字体源码、署名与许可证随仓库保留。 [字体说明](third_party/fusion-pixel-12px/)

原作者和各依赖的版权及许可声明保留在源码中。ROM、BIOS 和个人存档由使用者自行提供。 [原版致谢](Docs/UPSTREAM_README.md) · [Credits](README.md#credits)
