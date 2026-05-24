# 🛡️ HotkeyDog (热键看门狗)

像看门狗一样守护你的系统热键，防止游戏或全屏工作时误触！

本程序是基于 C++ Native (Win32 API) 编写的超轻量绿色工具，编译后仅单个 exe 文件（约 300KB），无需安装任何 Python 环境或第三方运行时，双击即可秒开运行。

### ✨ 功能特点

- **暗黑科技风 GUI**：简洁美观的自绘（Owner-drawn）暗色主题界面，不刺眼。
- **动态分组控制**：按软件和系统功能将热键分组管理，支持独立开关。
- **配置自动持久化**：动态读取/保存同一目录下的 `hotkey_dog.ini`，记住你的选择。
- **一键快捷热切换**：在任何全屏或游戏状态下，按下 `Ctrl + F12` 即可瞬间秒开/秒关守护状态。
- **高分屏完美支持**：内置现代 DPI 适配（DPI Awareness），在高缩放屏幕下文字依然锐利清晰。
- **智能隐藏至托盘**：点击最小化按钮时，程序会自动隐藏至右下角系统托盘，不占用任务栏空间。

### 🚀 快速开始

#### 直接使用（推荐）

1. 在 Releases 页面下载编译好的 `HotkeyDog.exe`。
2. **右键 -> 以管理员身份运行**（拦截高级别系统快捷键需要管理员权限）。
3. 勾选你想屏蔽的热键组，点击 **"启动守护"** 按钮。

#### 从源码编译

如果你配置了 C++ 编译环境，可以使用以下命令直接输出免依赖的二进制程序：

- **使用 MinGW (g++) 编译**：
  g++ -O2 -mwindows -static -o HotkeyDog.exe hotkey_dog.cpp -lcomctl32 -lshlwapi

- **使用 MSVC (Visual Studio 命令行) 编译**：
  cl /EHsc /O2 /utf-8 hotkey_dog.cpp /link comctl32.lib shell32.lib shlwapi.lib user32.lib gdi32.lib

### ⌨️ 全局控制热键

- **Ctrl + F12**：全局无条件一键切换（启动守护 / 停止守护）

### 🚫 默认屏蔽的热键规划

#### 1. 即时通讯软件截图
- Alt + A (微信截屏)
- Ctrl + Alt + A (微信截屏-备选)
- Ctrl + Alt + W (微信主窗口呼出)
- Alt + S (飞书/Lark截屏)
- Ctrl + Shift + A (钉钉截屏)
- Ctrl + Alt + O (QQ截屏)

#### 2. 输入法切换
- Ctrl + Space (中英文状态切换)
- Ctrl + Shift (输入法框架切换)
- Alt + Shift (输入法语言切换)
- Win + Space (Windows 系统输入法切换)

#### 3. Windows 系统快捷键
- Win + Shift + S (系统高级截屏工具)
- Win + L (系统锁屏)
- Win + D (最小化所有窗口/显示桌面)
- Win + E (打开文件资源管理器)

#### 4. 其他易误触按键
- Alt + Tab (全局窗口切换)
- Alt + F4 (关闭当前窗口)
- 左 Win 键 (键盘左侧 Windows 徽标键独立拦截)

> ⚠️ **关于 Ctrl+Alt+Delete**：该组合键属于 Windows 核心安全注意序列（SAS），系统在底层强制禁止任何用户态程序和钩子对其拦截，因此本软件无法也不支持屏蔽该组合键。

### 📂 项目文件结构

HotkeyDog/
├── hotkey_dog.cpp          # C++ 主程序核心源码
├── build_cpp.bat           # Windows 一键编译脚本
├── HotkeyDog.exe           # 编译后的免环境绿色可执行文件
├── readme_cn.md            # 中文使用说明书
└── readme_en.md            # 英文使用说明书 (English Docs)

### 📄 开源协议

本项目基于 MIT License 协议开源，你可以自由分享、修改和商用，但请保留原作者版权声明。