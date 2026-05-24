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
- **⚡ 一键热切换 / Hotkey Toggle**：Press `Ctrl + F12` to enable/disable anywhere. / 任何全屏或游戏状态下按 `Ctrl + F12` 瞬间开关守护。
- **📦 动态分组控制 / Group Management**：Independent switches for different hotkey groups. / 按软件与系统功能完美归类，支持独立开关。
- **💾 配置自动保存 / Config Persistence**：Autosaves settings to `hotkey_dog.ini`. / 自动读写同目录下配置文件，记住你的选择。

---

## 🚀 Quick Start / 快速开始

### 1. Run Directly / 直接使用 (Recommended)
- Download `HotkeyDog.exe` from the **Releases** tab. / 在 **Releases** 页面下载编译好的可执行文件。
- **Right-click -> Run as administrator** (Crucial to block high-privilege hotkeys). / **右键 -> 以管理员身份运行**（拦截高级别系统快捷键和全屏游戏内热键必须提升权限）。

### 2. Compile / 从源码编译
- **MinGW (g++)**:
  ```bash
  g++ -O2 -mwindows -static -o HotkeyDog.exe hotkey_dog.cpp -lcomctl32 -lshlwapi
