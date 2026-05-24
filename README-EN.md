# 🛡️ HotkeyDog (Hotkey Watchdog)

Guard your system hotkeys like a loyal watchdog, completely preventing accidental misclicks during intensive gaming sessions or full-screen productivity work!

`HotkeyDog` is an ultra-lightweight, green portable utility built entirely on native C++ (Win32 API). The compiled executable is only about 300KB with zero dependencies, meaning it runs instantly without Python, .NET, or any external runtimes.

### ✨ Features

- **Cyberpunk Dark Theme GUI**: Beautiful owner-drawn pixel-level dark interface that goes easy on your eyes.
- **Dynamic Group Management**: Hotkeys are categorized by software and system behaviors, allowing individual group toggles.
- **Configuration Persistence**: Automatically reads and saves choices into `hotkey_dog.ini` within the same folder.
- **One-Click Hot Toggle**: Press `Ctrl + F12` under any full-screen or gaming scenario to instantly enable/disable protection.
- **Modern DPI Awareness**: Native support for high-DPI scaling; texts remain crisp and clear on 2K/4K displays.
- **Minimize to System Tray**: Clicking minimize completely hides the window into the notification area (system tray) to save taskbar space.

### 🚀 Quick Start

#### Run Directly (Recommended)

1. Download the compiled `HotkeyDog.exe` from the Releases tab.
2. **Right-click -> Run as administrator** (Required to hook and block high-privilege system shortcuts).
3. Toggle the hotkey groups you wish to block, then click the **"Start Guard"** button.

#### Compile from Source

If you have a C++ development environment configured, compile it directly with zero runtime overhead:

- **Using MinGW (g++)**:
  g++ -O2 -mwindows -static -o HotkeyDog.exe hotkey_dog.cpp -lcomctl32 -lshlwapi

- **Using MSVC (Visual Studio Command Prompt)**:
  cl /EHsc /O2 /utf-8 hotkey_dog.cpp /link comctl32.lib shell32.lib shlwapi.lib user32.lib gdi32.lib

### ⌨️ Global Control Hotkey

- **Ctrl + F12**: Universal Hot-Switch (Start Guard / Stop Guard instantly)

### 🚫 Blocked Hotkeys Mapping

#### 1. IM Screenshots
- Alt + A (WeChat Screenshot)
- Ctrl + Alt + A (WeChat Screenshot - Alternative)
- Ctrl + Alt + W (WeChat Main Window)
- Alt + S (Feishu / Lark Screenshot)
- Ctrl + Shift + A (DingTalk Screenshot)
- Ctrl + Alt + O (QQ Screenshot)

#### 2. Input Method (IME) Layouts
- Ctrl + Space (Chinese / English Toggle)
- Ctrl + Shift (IME Framework Layout Switching)
- Alt + Shift (Keyboard Language Switching)
- Win + Space (Windows System Input Layout Toggle)

#### 3. Windows System Shortcuts
- Win + Shift + S (System Snipping Tool)
- Win + L (Lock Screen)
- Win + D (Minimize All Windows / Show Desktop)
- Win + E (Open File Explorer)

#### 4. Other Misclick-prone Keys
- Alt + Tab (Global Window Switching)
- Alt + F4 (Close Active Window)
- Left Win (Standalone Left Windows Key Interception)

> ⚠️ **Note on Ctrl+Alt+Delete**: This sequence belongs to the Windows Secure Attention Sequence (SAS). The operating system strictly prohibits user-mode hooks from intercepting it for security reasons, so it is not covered by this utility.

### 📂 File Structure

HotkeyDog/
├── hotkey_dog.cpp          # Main C++ source code
├── build_cpp.bat           # One-click batch compilation script for Windows
├── HotkeyDog.exe           # Compiled standalone portable executable
├── readme_cn.md            # Chinese Documentation
└── readme_en.md            # English Documentation

### 📄 License

This project is licensed under the MIT License. Feel free to use, modify, and distribute, provided that the original copyright notice is retained.