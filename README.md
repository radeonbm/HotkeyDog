# 🛡️ HotkeyDog (热键看门狗)

[简体中文] | [English]

### 💡 开发初衷 / Motivation
作为一个动作游戏玩家，每次在激战中极限走位或连招时，总是不小心误触快捷键调出输入法（或者微信截图框），导致画面卡顿、操作失误，瞬间血压飙升、心情烦躁。为了彻底根治这个痛点，我动手写了这个超轻量的系统级“看门狗”工具。
As an action game enthusiast, I was constantly frustrated by accidentally triggering input methods (IME) or IM screenshots during intense gaming sessions, leading to missed controls and ruined combos. To permanently cure this headache, I built this ultra-lightweight, native system watchdog utility.

---

像看门狗一样守护你的系统热键，防止游戏或全屏工作时遭遇无情误触！基于 C++ Win32 API 编写，无任何第三方依赖，编译后仅约 300KB，绿色免安装。
Guard your system hotkeys like a loyal watchdog, completely preventing accidental misclicks during intensive gaming or full-screen work! Built on native C++ (Win32 API), zero dependencies, only ~300KB standalone executable.

---

## ✨ Features / 功能特点

- **🖥️ 赛博暗黑风界面 / Cyberpunk GUI**：Owner-drawn dark theme, easy on eyes. / 简洁美观的自绘暗色主题。
- **⚡ 一键热切换 / Hotkey Toggle**：Press `Ctrl + Alt + F10` to enable/disable anywhere. / 任何全屏或游戏状态下按 `Ctrl + Alt + F10` 开关守护。
- **📦 动态分组控制 / Group Management**：Independent switches for different hotkey groups. / 按软件与系统功能归类，支持独立开关。
- **💾 配置自动保存 / Config Persistence**：Autosaves settings to `hotkey_dog.ini`. / 自动读写同目录下配置文件，记住你的选择。

---

## 🚀 Quick Start / 快速开始

### 1. Run Directly / 直接使用 (Recommended)
- Download `HotkeyDog.exe` from the **Releases** tab. / 在 **Releases** 页面下载编译好的可执行文件。
- Accept the UAC administrator prompt at launch (Crucial to block high-privilege hotkeys). / 启动时接受 UAC 管理员权限提示（拦截高级别系统快捷键和全屏游戏内热键必须提升权限）。

### 2. Compile / 从源码编译
- Configure MSVC or MinGW-w64, then run `build_cpp.bat`. It embeds the administrator manifest. / 配置 MSVC 或 MinGW-w64 后运行 `build_cpp.bat`；它会嵌入管理员权限清单。

## HotkeyDog v1.1.0

### 中文

- 修复全局键盘钩子注册、卸载失败后仍显示“已开启”的问题。
- 防止重复启动多个实例，避免钩子和配置互相冲突。
- 全局开关调整为 `Ctrl + Alt + F10`，避免与系统保留的 F12 冲突，并防止按住按键反复切换。
- 修复组合键拦截在修饰键场景下偶发失效的问题。
- 程序启动时请求管理员权限，以提升对高权限程序和游戏内快捷键的拦截能力。
- 改进配置保存失败提示、托盘图标创建检查和系统资源释放。
- 新增构建清单，确保发布版本带有管理员权限配置。

### English

- Fixed keyboard-hook registration and cleanup failures that could incorrectly show protection as enabled.
- Prevented multiple running instances from conflicting over hooks and configuration.
- Changed the global toggle to `Ctrl + Alt + F10` to avoid the system-reserved F12 key and prevent repeated toggles while holding keys.
- Fixed occasional shortcut-blocking failures involving modifier keys.
- The application now requests administrator privileges at startup for improved blocking in elevated applications and games.
- Improved configuration-save error reporting, tray-icon creation checks, and system resource cleanup.
- Added a build manifest to ensure release builds include the administrator privilege configuration.
