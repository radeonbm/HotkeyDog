/*
 * HotkeyDog - 系统热键屏蔽工具 (C++ Native版本)
 * 像看门狗一样守护你的热键，防止游戏时误触！
 * 无需Python环境，编译后仅单个exe文件，体积约300KB
 *
 * 编译方法:
 *   MSVC:  cl /EHsc /O2 /utf-8 hotkey_dog.cpp /link comctl32.lib shell32.lib shlwapi.lib user32.lib gdi32.lib
 *   MinGW: g++ -O2 -mwindows -static -o HotkeyDog.exe hotkey_dog.cpp -lcomctl32 -lshlwapi
 */

#define UNICODE
#define _UNICODE

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <algorithm>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
    name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
    processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// ============================================================
// 常量定义
// ============================================================

#define WM_TRAYICON         (WM_USER + 1)
#define ID_TRAY_ICON        1
#define ID_HOTKEY_TOGGLE    100
#define ID_BTN_PROTECT      101
#define ID_BTN_ABOUT        102
#define ID_BTN_ABOUT_CLOSE  103
#define ID_BTN_LANG         104
#define ID_MENU_SHOW        200
#define ID_MENU_TOGGLE      201
#define ID_MENU_EXIT        202
#define ID_CHK_GROUP_START  300

// 颜色主题
namespace C {
    constexpr COLORREF BG        = RGB(26, 26, 46);
    constexpr COLORREF CARD      = RGB(22, 33, 62);
    constexpr COLORREF ACCENT    = RGB(15, 52, 96);
    constexpr COLORREF HIGHLIGHT = RGB(233, 69, 96);
    constexpr COLORREF TEXT      = RGB(255, 255, 255);
    constexpr COLORREF TEXT2     = RGB(160, 160, 160);
    constexpr COLORREF GREEN     = RGB(0, 184, 148);
    constexpr COLORREF GRAY      = RGB(99, 110, 114);
}

// ============================================================
// 国际化
// ============================================================

bool g_langEn = false;  // false=中文, true=English

// 通用字符串
#define _(zh, en) (g_langEn ? (en) : (zh))

// ============================================================
// 数据结构
// ============================================================

struct BlockedCombo {
    std::wstring name;
    std::wstring nameEn;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
    bool win = false;
    bool leftWinOnly = false;   // 独立的左Win键
    DWORD mainKey = 0;          // 触发键 (0=仅修饰键)
    bool enabled = true;

    bool Matches(DWORD vk) const {
        if (leftWinOnly && mainKey == 0)
            return vk == VK_LWIN;
        if (vk != mainKey) return false;
        if (ctrl  && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
        if (alt   && !(GetAsyncKeyState(VK_MENU)    & 0x8000)) return false;
        if (shift && !(GetAsyncKeyState(VK_SHIFT)    & 0x8000)) return false;
        if (win   && !(GetAsyncKeyState(VK_LWIN) & 0x8000 ||
                      GetAsyncKeyState(VK_RWIN) & 0x8000))     return false;
        return true;
    }
};

struct HotKeyGroup {
    std::wstring name;
    std::wstring nameEn;
    std::wstring icon;
    std::wstring description;
    std::wstring descriptionEn;
    std::vector<BlockedCombo> keys;
    bool groupEnabled;
    int chkId;
};

// ============================================================
// 默认热键配置
// ============================================================

std::vector<HotKeyGroup> g_defaultGroups;

void InitDefaultGroups() {
    g_defaultGroups = {
        {
            L"即时通讯软件截图", L"IM Screenshots", L"[M]",
            L"微信、飞书等软件的截图快捷键", L"Screenshot hotkeys for WeChat, Feishu, etc.",
            {
                {L"微信截屏",      L"WeChat Screenshot",       false, true,  false, false, false, 'A',     true},
                {L"微信截屏(备选)", L"WeChat Screenshot (Alt)", true,  true,  false, false, false, 'A',     true},
                {L"微信主窗口",    L"WeChat Main Window",      true,  true,  false, false, false, 'W',     true},
                {L"飞书截屏",      L"Feishu Screenshot",       false, true,  false, false, false, 'S',     true},
                {L"钉钉截屏",      L"DingTalk Screenshot",     true,  false, true,  false, false, 'A',     true},
                {L"QQ截屏",        L"QQ Screenshot",           true,  true,  false, false, false, 'O',     true},
            },
            true, 0
        },
        {
            L"输入法切换", L"Input Method", L"[K]",
            L"中英文输入法切换快捷键", L"Input method switching hotkeys",
            {
                {L"中英切换",         L"CN/EN Toggle",             true,  false, false, false, false, VK_SPACE,       true},
                {L"输入法切换",       L"Input Method Switch",      true,  false, true,  false, false, 0,              true},
                {L"输入法切换(备选)", L"Input Method Switch (Alt)", false, true,  true,  false, false, 0,              true},
                {L"系统输入法",       L"Win+Space Input",          false, false, false, true,  false, VK_SPACE,       true},
                {L"AI汪仔",           L"AI Assistant",             false, true,  false, false, false, VK_SPACE,       true},
                {L"AI汪仔(备选)",     L"AI Asst. (Alt)",           true,  false, true,  false, false, VK_SPACE,       true},
                {L"中英标点切换",     L"CN/EN Punctuation",        true,  false, false, false, false, VK_OEM_PERIOD,  true},
                {L"全半角切换",       L"Full/Half Width",          false, false, true,  false, false, VK_SPACE,       true},
            },
            true, 0
        },
        {
            L"Windows系统快捷键", L"Windows Shortcuts", L"[W]",
            L"Windows系统自带的快捷键", L"Built-in Windows hotkeys",
            {
                {L"系统截屏",       L"Win+Shift+S Screenshot", false, false, true,  true,  false, 'S',     true},
                {L"Win+L锁屏",     L"Win+L Lock Screen",      false, false, false, true,  false, 'L',     true},
                {L"Win+D显示桌面",  L"Win+D Show Desktop",     false, false, false, true,  false, 'D',     true},
                {L"Win+E文件管理器", L"Win+E File Explorer",    false, false, false, true,  false, 'E',     true},
            },
            true, 0
        },
        {
            L"其他可能误触的键", L"Other Accidental Keys", L"[O]",
            L"其他容易误触的快捷键", L"Other easily accidental hotkeys",
            {
                {L"Alt+Tab切换窗口", L"Alt+Tab Switch Window", false, true, false, false, false, VK_TAB,    true},
                {L"Alt+F4关闭窗口",  L"Alt+F4 Close Window",   false, true, false, false, false, VK_F4,     true},
                {L"左Win键误触",     L"Left Win Key",           false, false,false, false, true,  0,         false},
            },
            false, 0
        },
    };

    for (auto& g : g_defaultGroups) {
        for (auto& k : g.keys) {
            if (k.mainKey == 0 && !k.leftWinOnly) {
                k.mainKey = VK_SHIFT;
            }
        }
    }
}

// ============================================================
// 全局状态
// ============================================================

HINSTANCE   g_hInst       = nullptr;
HWND        g_hwndMain    = nullptr;
HHOOK       g_hHook       = nullptr;
HFONT       g_hFontTitle  = nullptr;
HFONT       g_hFontSub    = nullptr;
HFONT       g_hFontNorm   = nullptr;
HFONT       g_hFontSmall  = nullptr;
HFONT       g_hFontSmallB = nullptr;
HFONT       g_hFontBtn    = nullptr;
HWND        g_btnProtect  = nullptr;
HWND        g_btnAbout    = nullptr;
HWND        g_btnLang     = nullptr;
HWND        g_hwndAbout   = nullptr;
NOTIFYICONDATAW g_nid      = {};
bool        g_protecting   = false;
bool        g_trayCreated  = false;
bool        g_notAdmin     = false;
std::wstring g_statusText;
std::vector<HotKeyGroup> g_groups;

// ============================================================
// 工具函数
// ============================================================

std::wstring GetExeDir() {
    WCHAR buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    PathRemoveFileSpecW(buf);
    return buf;
}

std::wstring GetIniPath() {
    return GetExeDir() + L"\\hotkey_dog.ini";
}

HFONT CreateFontForUI(int size, bool bold = false) {
    return CreateFontW(
        -MulDiv(size, GetDeviceCaps(GetDC(nullptr), LOGPIXELSY), 72),
        0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
}

void InitFonts() {
    g_hFontTitle  = CreateFontForUI(24, true);
    g_hFontSub    = CreateFontForUI(10, false);
    g_hFontNorm   = CreateFontForUI(11, true);
    g_hFontSmall  = CreateFontForUI(9, false);
    g_hFontSmallB = CreateFontForUI(9, true);
    g_hFontBtn    = CreateFontForUI(11, true);
}

void DeleteFonts() {
    if (g_hFontTitle)  { DeleteObject(g_hFontTitle);  g_hFontTitle  = nullptr; }
    if (g_hFontSub)    { DeleteObject(g_hFontSub);    g_hFontSub    = nullptr; }
    if (g_hFontNorm)   { DeleteObject(g_hFontNorm);   g_hFontNorm   = nullptr; }
    if (g_hFontSmall)  { DeleteObject(g_hFontSmall);  g_hFontSmall  = nullptr; }
    if (g_hFontSmallB) { DeleteObject(g_hFontSmallB); g_hFontSmallB = nullptr; }
    if (g_hFontBtn)    { DeleteObject(g_hFontBtn);    g_hFontBtn    = nullptr; }
}

void UpdateStatusText() {
    if (g_notAdmin) {
        g_statusText = _(L"[!] 建议以管理员身份运行以确保所有热键可被屏蔽",
                         L"[!] Run as admin to ensure all hotkeys can be blocked");
    } else if (g_protecting) {
        g_statusText = _(L"守护已启动 - 热键已屏蔽", L"Guard active - Hotkeys blocked");
    } else {
        g_statusText = _(L"就绪 - 选择要屏蔽的热键组，然后启动守护",
                         L"Ready - Select hotkey groups, then start guard");
    }
}

// ============================================================
// 配置读写 (INI)
// ============================================================

void LoadConfig() {
    std::wstring ini = GetIniPath();
    g_groups = g_defaultGroups;

    // 读取语言设置
    int lang = GetPrivateProfileIntW(L"Settings", L"Language", 0, ini.c_str());
    g_langEn = (lang != 0);

    for (auto& g : g_groups) {
        int gEnabled = GetPrivateProfileIntW(L"Groups", g.name.c_str(), g.groupEnabled ? 1 : 0, ini.c_str());
        g.groupEnabled = (gEnabled != 0);

        for (auto& k : g.keys) {
            int kEnabled = GetPrivateProfileIntW(L"Keys", k.name.c_str(), k.enabled ? 1 : 0, ini.c_str());
            k.enabled = (kEnabled != 0);
        }
    }
}

void SaveConfig() {
    std::wstring ini = GetIniPath();
    WritePrivateProfileStringW(L"Settings", L"Language", g_langEn ? L"1" : L"0", ini.c_str());
    for (const auto& g : g_groups) {
        WritePrivateProfileStringW(L"Groups", g.name.c_str(),
            g.groupEnabled ? L"1" : L"0", ini.c_str());
        for (const auto& k : g.keys) {
            WritePrivateProfileStringW(L"Keys", k.name.c_str(),
                k.enabled ? L"1" : L"0", ini.c_str());
        }
    }
}

// ============================================================
// 键盘钩子
// ============================================================

bool IsToggleKey(DWORD vk) {
    if (vk == VK_F12 && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        return true;
    return false;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        bool protecting = g_protecting;  // 快照，减少竞态窗口
        if (protecting) {
            KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vk = kb->vkCode;

            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                if (IsToggleKey(vk))
                    return CallNextHookEx(g_hHook, nCode, wParam, lParam);

                for (const auto& g : g_groups) {
                    if (!g.groupEnabled) continue;
                    for (const auto& k : g.keys) {
                        if (!k.enabled) continue;
                        if (k.Matches(vk)) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void InstallHook() {
    if (!g_hHook) {
        g_hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInst, 0);
    }
}

void RemoveHook() {
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }
}

// ============================================================
// 保护模式切换
// ============================================================

void ToggleProtection() {
    g_protecting = !g_protecting;
    if (g_protecting) {
        InstallHook();
    } else {
        RemoveHook();
    }
    UpdateStatusText();
    
    if (g_btnProtect) {
        InvalidateRect(g_btnProtect, nullptr, TRUE);
        UpdateWindow(g_btnProtect);
    }
    InvalidateRect(g_hwndMain, nullptr, FALSE);
}

// ============================================================
// 系统托盘
// ============================================================

void AddTrayIcon() {
    if (g_trayCreated) return;
    memset(&g_nid, 0, sizeof(g_nid));
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd   = g_hwndMain;
    g_nid.uID    = ID_TRAY_ICON;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon  = LoadIcon(g_hInst, MAKEINTRESOURCE(1));
    if (!g_nid.hIcon) g_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(g_nid.szTip, L"HotkeyDog");
    Shell_NotifyIconW(NIM_ADD, &g_nid);
    g_trayCreated = true;
}

void RemoveTrayIcon() {
    if (g_trayCreated) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_trayCreated = false;
    }
}

void ShowTrayMenu() {
    POINT pt;
    GetCursorPos(&pt);
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SHOW,   _(L"显示主窗口", L"Show Window"));
    AppendMenuW(hMenu, MF_STRING, ID_MENU_TOGGLE,  g_protecting ? _(L"停止守护", L"Stop Guard") : _(L"启动守护", L"Start Guard"));
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, ID_MENU_EXIT,    _(L"退出", L"Exit"));
    SetForegroundWindow(g_hwndMain);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hwndMain, nullptr);
    DestroyMenu(hMenu);
}

// ============================================================
// GUI - 暗色主题辅助
// ============================================================

HBRUSH g_brushBg   = nullptr;
HBRUSH g_brushCard = nullptr;

void PaintCard(HDC hdc, RECT rc) {
    FillRect(hdc, &rc, g_brushCard);
    HPEN pen = CreatePen(PS_SOLID, 1, C::ACCENT);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBr);
    DeleteObject(pen);
}

// 自绘按钮
void DrawProtectButton(LPDRAWITEMSTRUCT dis) {
    RECT rc = dis->rcItem;
    HDC hdc = dis->hDC;

    bool hovered = (dis->itemState & ODS_HOTLIGHT);
    bool pressed = (dis->itemState & ODS_SELECTED);

    COLORREF bg = g_protecting ? C::GREEN : C::HIGHLIGHT;
    if (pressed)    bg = C::ACCENT;
    else if (hovered) bg = g_protecting ? RGB(0, 160, 130) : RGB(210, 55, 80);

    HBRUSH br = CreateSolidBrush(bg);
    FillRect(hdc, &rc, br);
    DeleteObject(br);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, C::TEXT);
    HFONT oldFont = (HFONT)SelectObject(hdc, g_hFontBtn);
    const WCHAR* txt = g_protecting ? _(L"停止守护", L"Stop Guard") : _(L"启动守护", L"Start Guard");
    DrawTextW(hdc, txt, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
}

// ============================================================
// GUI - About 窗口
// ============================================================

std::wstring ComboToString(const BlockedCombo& k) {
    std::wstring s;
    if (k.ctrl)  s += L"Ctrl+";
    if (k.alt)   s += L"Alt+";
    if (k.shift) s += L"Shift+";
    if (k.win)   s += L"Win+";
    if (k.leftWinOnly && k.mainKey == 0) {
        s += L"LWin";
        return s;
    }
    if (k.mainKey == VK_SPACE)       s += L"Space";
    else if (k.mainKey == VK_TAB)    s += L"Tab";
    else if (k.mainKey == VK_F4)     s += L"F4";
    else if (k.mainKey == VK_SHIFT)  s += L"Shift";
    else if (k.mainKey == VK_OEM_PERIOD) s += L".";
    else if (k.mainKey >= 'A' && k.mainKey <= 'Z') s += (WCHAR)k.mainKey;
    else if (k.mainKey != 0)         s += std::to_wstring(k.mainKey);
    return s;
}

void ShowAboutWindow();
void PositionAboutWindow();

LRESULT CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE: {
        CreateWindowW(L"BUTTON", _(L"关闭", L"Close"),
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            365, 505, 80, 24, hwnd, (HMENU)ID_BTN_ABOUT_CLOSE, g_hInst, nullptr);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, g_brushBg);

        // 边框
        HPEN pen = CreatePen(PS_SOLID, 1, C::ACCENT);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        HBRUSH oldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, 0, 0, rc.right, rc.bottom);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBr);
        DeleteObject(pen);

        SetBkMode(hdc, TRANSPARENT);
        int y = 20;

        // 标题
        SetTextColor(hdc, C::HIGHLIGHT);
        HFONT oldF = (HFONT)SelectObject(hdc, g_hFontTitle);
        RECT titleRc = { 0, y, 460, y + 35 };
        DrawTextW(hdc, L"HotkeyDog", -1, &titleRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        y += 38;

        // 版本
        SelectObject(hdc, g_hFontNorm);
        SetTextColor(hdc, C::TEXT);
        RECT verRc = { 0, y, 460, y + 22 };
        DrawTextW(hdc, L"v1.0", -1, &verRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        y += 25;

        // 邮箱
        SelectObject(hdc, g_hFontSub);
        SetTextColor(hdc, RGB(200, 200, 200));
        RECT emailRc = { 0, y, 460, y + 20 };
        DrawTextW(hdc, L"radeonbm@gmail.com", -1, &emailRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        y += 30;

        // 分隔线
        HPEN sepPen = CreatePen(PS_SOLID, 1, C::ACCENT);
        HPEN prevPen = (HPEN)SelectObject(hdc, sepPen);
        MoveToEx(hdc, 30, y, nullptr);
        LineTo(hdc, 430, y);
        SelectObject(hdc, prevPen);
        DeleteObject(sepPen);
        y += 12;

        // 四个分组说明
        for (const auto& g : g_defaultGroups) {
            SetTextColor(hdc, C::GREEN);
            SelectObject(hdc, g_hFontSmallB);
            RECT grpRc = { 25, y, 435, y + 20 };
            DrawTextW(hdc, _(g.name.c_str(), g.nameEn.c_str()), -1, &grpRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            y += 22;

            SelectObject(hdc, g_hFontSmall);
            // 输入法分组用两列显示
            if (g.name == L"输入法切换" && g.keys.size() > 4) {
                size_t half = (g.keys.size() + 1) / 2;
                int leftY = y, rightY = y;
                for (size_t i = 0; i < g.keys.size(); i++) {
                    const auto& k = g.keys[i];
                    std::wstring line = ComboToString(k) + L"  \u2014  " + _(k.name.c_str(), k.nameEn.c_str());
                    SetTextColor(hdc, RGB(200, 200, 200));
                    if (i < half) {
                        RECT keyRc = { 40, leftY, 235, leftY + 18 };
                        DrawTextW(hdc, line.c_str(), -1, &keyRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                        leftY += 18;
                    } else {
                        RECT keyRc = { 240, rightY, 435, rightY + 18 };
                        DrawTextW(hdc, line.c_str(), -1, &keyRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                        rightY += 18;
                    }
                }
                y = (leftY > rightY ? leftY : rightY);
            } else {
                for (const auto& k : g.keys) {
                    std::wstring line = ComboToString(k) + L"  \u2014  " + _(k.name.c_str(), k.nameEn.c_str());
                    SetTextColor(hdc, RGB(200, 200, 200));
                    RECT keyRc = { 40, y, 435, y + 18 };
                    DrawTextW(hdc, line.c_str(), -1, &keyRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                    y += 18;
                }
            }
            y += 8;
        }

        SelectObject(hdc, oldF);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == ID_BTN_ABOUT_CLOSE) {
            RECT rc = dis->rcItem;
            HDC hdc = dis->hDC;
            bool pressed = (dis->itemState & ODS_SELECTED);

            COLORREF bg = pressed ? C::ACCENT : C::HIGHLIGHT;
            HBRUSH br = CreateSolidBrush(bg);
            FillRect(hdc, &rc, br);
            DeleteObject(br);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, C::TEXT);
            HFONT oldF = (HFONT)SelectObject(hdc, g_hFontSmall);
            DrawTextW(hdc, _(L"关闭", L"Close"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldF);
            return TRUE;
        }
        return FALSE;
    }

    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BTN_ABOUT_CLOSE) {
            DestroyWindow(hwnd);
            g_hwndAbout = nullptr;
            return 0;
        }
        break;
    }

    case WM_DESTROY: {
        g_hwndAbout = nullptr;
        return 0;
    }

    } // switch
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void PositionAboutWindow() {
    if (!g_hwndAbout || !IsWindow(g_hwndAbout)) return;
    int w = 460, h = 550;
    RECT mainRc;
    GetWindowRect(g_hwndMain, &mainRc);
    int mainW = mainRc.right - mainRc.left;
    int mainH = mainRc.bottom - mainRc.top;
    int sx = mainRc.left + (mainW - w) / 2;
    int sy = mainRc.top + (mainH - h) / 2;
    SetWindowPos(g_hwndAbout, nullptr, sx, sy, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void ShowAboutWindow() {
    if (g_hwndAbout && IsWindow(g_hwndAbout)) {
        // 语言可能变了，关闭旧的重建
        DestroyWindow(g_hwndAbout);
        g_hwndAbout = nullptr;
    }

    int w = 460, h = 550;

    g_hwndAbout = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        L"HotkeyDogAboutClass",
        L"About HotkeyDog",
        WS_POPUP | WS_CLIPCHILDREN,
        0, 0, w, h,
        g_hwndMain, nullptr, g_hInst, nullptr
    );

    SetLayeredWindowAttributes(g_hwndAbout, 0, 235, LWA_ALPHA);

    PositionAboutWindow();
    ShowWindow(g_hwndAbout, SW_SHOW);
    UpdateWindow(g_hwndAbout);
}

// ============================================================
// GUI - 创建界面
// ============================================================

struct CardInfo {
    RECT cardRect;
    RECT switchRect;
    std::wstring groupName;
};

std::vector<CardInfo> g_cards;

void CreateUI(HWND hwnd) {
    g_cards.clear();
    int chkId = ID_CHK_GROUP_START;

    // ===== 语言切换按钮（About左侧） =====
    g_btnLang = CreateWindowW(L"BUTTON", L"EN",
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        355, 22, 50, 26, hwnd, (HMENU)ID_BTN_LANG, g_hInst, nullptr);

    // ===== About 按钮（标题右侧） =====
    g_btnAbout = CreateWindowW(L"BUTTON", L"About",
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        415, 22, 55, 26, hwnd, (HMENU)ID_BTN_ABOUT, g_hInst, nullptr);

    // ===== 全局保护按钮 =====
    g_btnProtect = CreateWindowW(L"BUTTON", _(L"启动守护", L"Start Guard"),
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        60, 82, 360, 42, hwnd, (HMENU)ID_BTN_PROTECT, g_hInst, nullptr);

    // ===== 分组卡片 =====
    int y = 140;
    for (auto& g : g_groups) {
        g.chkId = chkId;

        CardInfo ci;
        ci.cardRect = { 20, y, 460, y + 80 };
        ci.groupName = g.name;
        g_cards.push_back(ci);

        ci.switchRect = { 385, y + 15, 445, y + 45 };
        CreateWindowW(L"BUTTON",
            g.groupEnabled ? L"ON" : L"OFF",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            385, y + 18, 55, 28, hwnd, (HMENU)(INT_PTR)chkId, g_hInst, nullptr);
        chkId++;

        y += 90;
    }
}

// ============================================================
// 窗口过程
// ============================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE: {
        g_brushBg   = CreateSolidBrush(C::BG);
        g_brushCard = CreateSolidBrush(C::CARD);
        InitFonts();
        LoadConfig();
        CreateUI(hwnd);
        UpdateStatusText();
        RegisterHotKey(hwnd, ID_HOTKEY_TOGGLE, MOD_CONTROL, VK_F12);
        AddTrayIcon();
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 背景
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, g_brushBg);

        // 标题
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, C::HIGHLIGHT);
        HFONT oldF = (HFONT)SelectObject(hdc, g_hFontTitle);
        RECT titleRc = { 20, 10, 460, 48 };
        DrawTextW(hdc, L"HotkeyDog", -1, &titleRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, g_hFontSmall);
        SetTextColor(hdc, C::TEXT2);
        RECT subRc = { 20, 55, 460, 75 };
        DrawTextW(hdc, _(L"热键看门狗 - 减少误触的烦恼", L"Hotkey Watchdog - Less Accidental Triggers"), -1, &subRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 绘制卡片
        int y = 140;
        for (auto& g : g_groups) {
            RECT cardRc = { 20, y, 460, y + 80 };
            PaintCard(hdc, cardRc);

            // 图标
            SetTextColor(hdc, C::HIGHLIGHT);
            SelectObject(hdc, g_hFontNorm);
            RECT iconRc = { 35, y + 10, 65, y + 35 };
            DrawTextW(hdc, g.icon.c_str(), -1, &iconRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            // 组名
            SetTextColor(hdc, C::TEXT);
            RECT nameRc = { 65, y + 8, 375, y + 30 };
            DrawTextW(hdc, _(g.name.c_str(), g.nameEn.c_str()), -1, &nameRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            // 描述
            SetTextColor(hdc, C::TEXT2);
            SelectObject(hdc, g_hFontSmall);
            RECT descRc = { 65, y + 30, 375, y + 48 };
            DrawTextW(hdc, _(g.description.c_str(), g.descriptionEn.c_str()), -1, &descRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            // 热键数量
            WCHAR countBuf[32];
            int enabledCnt = 0;
            for (const auto& k : g.keys) if (k.enabled) enabledCnt++;
            swprintf_s(countBuf, g_langEn ? L"%d hotkeys" : L"%d 个快捷键", enabledCnt);
            RECT cntRc = { 65, y + 48, 375, y + 68 };
            DrawTextW(hdc, countBuf, -1, &cntRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            y += 90;
        }

        // 状态栏
        SetTextColor(hdc, g_protecting ? C::HIGHLIGHT : C::TEXT2);
        SelectObject(hdc, g_hFontSmall);
        RECT stRc = { 20, y + 5, 460, y + 25 };
        DrawTextW(hdc, g_statusText.c_str(), -1, &stRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc, C::TEXT2);
        SelectObject(hdc, g_hFontSmall);
        RECT verRc = { 20, y + 28, 460, y + 46 };
        DrawTextW(hdc, _(L"v1.0 C++ Native | 按 Ctrl+F12 快速切换", L"v1.0 C++ Native | Press Ctrl+F12 to toggle"),
            -1, &verRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldF);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)g_brushBg;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == ID_BTN_PROTECT) {
            DrawProtectButton(dis);
            return TRUE;
        }
        // 语言切换按钮
        if (dis->CtlID == ID_BTN_LANG) {
            RECT rc = dis->rcItem;
            HDC hdc = dis->hDC;
            bool hovered = (dis->itemState & ODS_HOTLIGHT);
            bool pressed = (dis->itemState & ODS_SELECTED);

            COLORREF bg = pressed ? C::ACCENT : (hovered ? RGB(40, 50, 80) : C::CARD);
            HBRUSH br = CreateSolidBrush(bg);
            FillRect(hdc, &rc, br);
            DeleteObject(br);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, hovered ? C::GREEN : C::TEXT2);
            HFONT oldF = (HFONT)SelectObject(hdc, g_hFontSmall);
            DrawTextW(hdc, g_langEn ? L"CN" : L"EN", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldF);
            return TRUE;
        }
        // About 按钮
        if (dis->CtlID == ID_BTN_ABOUT) {
            RECT rc = dis->rcItem;
            HDC hdc = dis->hDC;
            bool hovered = (dis->itemState & ODS_HOTLIGHT);
            bool pressed = (dis->itemState & ODS_SELECTED);

            COLORREF bg = pressed ? C::ACCENT : (hovered ? RGB(40, 50, 80) : C::CARD);
            HBRUSH br = CreateSolidBrush(bg);
            FillRect(hdc, &rc, br);
            DeleteObject(br);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, hovered ? C::HIGHLIGHT : C::TEXT2);
            HFONT oldF = (HFONT)SelectObject(hdc, g_hFontSmall);
            DrawTextW(hdc, L"About", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldF);
            return TRUE;
        }
        // 开关按钮自绘
        if (dis->CtlID >= ID_CHK_GROUP_START && dis->CtlID < ID_CHK_GROUP_START + 20) {
            int idx = (int)(dis->CtlID - ID_CHK_GROUP_START);
            if (idx < (int)g_groups.size()) {
                bool isOn = g_groups[idx].groupEnabled;
                RECT rc = dis->rcItem;
                HDC hdc = dis->hDC;

                COLORREF bg = isOn ? C::GREEN : C::GRAY;
                if (dis->itemState & ODS_SELECTED) bg = isOn ? RGB(0, 150, 120) : RGB(80, 90, 94);

                HBRUSH br = CreateSolidBrush(bg);
                FillRect(hdc, &rc, br);
                DeleteObject(br);

                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, C::TEXT);
                HFONT oldF = (HFONT)SelectObject(hdc, g_hFontSmall);
                const WCHAR* txt = isOn ? L"ON" : L"OFF";
                DrawTextW(hdc, txt, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(hdc, oldF);
            }
            return TRUE;
        }
        return FALSE;
    }

    case WM_COMMAND: {
        WORD id = LOWORD(wParam);
        // 语言切换
        if (id == ID_BTN_LANG) {
            g_langEn = !g_langEn;
            SaveConfig();
            UpdateStatusText();
            
            // 强刷主窗口和顶部功能按钮
            InvalidateRect(hwnd, nullptr, FALSE);
            if (g_btnProtect) InvalidateRect(g_btnProtect, nullptr, TRUE);
            if (g_btnAbout)   InvalidateRect(g_btnAbout, nullptr, TRUE);
            if (g_btnLang)    InvalidateRect(g_btnLang, nullptr, TRUE);
            
            SetWindowTextW(hwnd, g_langEn ? L"HotkeyDog - Hotkey Watchdog" : L"HotkeyDog - 热键看门狗");
            if (g_trayCreated) {
                wcscpy_s(g_nid.szTip, g_langEn ? L"HotkeyDog - Hotkey Watchdog" : L"HotkeyDog - 热键看门狗");
                Shell_NotifyIconW(NIM_MODIFY, &g_nid);
            }
            return 0;
        }
        if (id == ID_BTN_ABOUT) {
            ShowAboutWindow();
            return 0;
        }
        if (id == ID_BTN_PROTECT) {
            bool anyEnabled = false;
            for (const auto& g : g_groups)
                if (g.groupEnabled) { anyEnabled = true; break; }
            if (!anyEnabled && !g_protecting) {
                MessageBoxW(hwnd,
                    _(L"请至少选择一个要屏蔽的热键组", L"Please select at least one hotkey group"),
                    _(L"提示", L"Notice"), MB_OK | MB_ICONWARNING);
                return 0;
            }
            ToggleProtection();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        // 分组开关
        if (id >= ID_CHK_GROUP_START && id < ID_CHK_GROUP_START + 20) {
            int idx = id - ID_CHK_GROUP_START;
            if (idx < (int)g_groups.size()) {
                g_groups[idx].groupEnabled = !g_groups[idx].groupEnabled;
                SaveConfig();
                
                HWND hwndBtn = (HWND)lParam;
                InvalidateRect(hwndBtn, nullptr, TRUE);
                UpdateWindow(hwndBtn);
                
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        // 托盘菜单
        if (id == ID_MENU_SHOW) {
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);
            return 0;
        }
        if (id == ID_MENU_TOGGLE) {
            ToggleProtection();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (id == ID_MENU_EXIT) {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    }

    case WM_HOTKEY: {
        if (wParam == ID_HOTKEY_TOGGLE) {
            ToggleProtection();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        break;
    }

    case WM_TRAYICON: {
        if (wParam == ID_TRAY_ICON) {
            if (LOWORD(lParam) == WM_RBUTTONUP) {
                ShowTrayMenu();
            } else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) {
                ShowWindow(hwnd, SW_RESTORE);
                SetForegroundWindow(hwnd);
            }
        }
        return 0;
    }

    case WM_SIZE: {
        break;
    }

    case WM_MOVE: {
        PositionAboutWindow();
        break;
    }

    case WM_DESTROY: {
        if (g_protecting) {
            g_protecting = false;
            RemoveHook();
        }
        UnregisterHotKey(hwnd, ID_HOTKEY_TOGGLE);
        RemoveTrayIcon();
        
        if (g_brushBg)   DeleteObject(g_brushBg);
        if (g_brushCard) DeleteObject(g_brushCard);
        DeleteFonts();
        
        PostQuitMessage(0);
        return 0;
    }

    } // switch

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ============================================================
// 主函数
// ============================================================

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow) {
    g_hInst = hInst;
    InitDefaultGroups();

    // DPI 感知
    try {
        HMODULE hShCore = LoadLibraryW(L"shcore.dll");
        if (hShCore) {
            typedef HRESULT(WINAPI* SetDPIFn)(int);
            auto setDPI = (SetDPIFn)GetProcAddress(hShCore, "SetProcessDpiAwareness");
            if (setDPI) setDPI(1);
            FreeLibrary(hShCore);
        }
    } catch (...) {}

    // 检查管理员权限
    bool isAdmin = false;
    {
        HANDLE hToken = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            TOKEN_ELEVATION elevation;
            DWORD dwSize = sizeof(TOKEN_ELEVATION);
            if (GetTokenInformation(hToken, TokenElevation, &elevation, dwSize, &dwSize)) {
                isAdmin = elevation.TokenIsElevated != 0;
            }
            CloseHandle(hToken);
        }
    }

    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance      = hInst;
    wc.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(1));
    wc.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground  = nullptr;
    wc.lpszClassName  = L"HotkeyDogClass";
    RegisterClassExW(&wc);

    // 注册 About 窗口类
    WNDCLASSEXW wcAbout = {};
    wcAbout.cbSize        = sizeof(wcAbout);
    wcAbout.style         = CS_HREDRAW | CS_VREDRAW;
    wcAbout.lpfnWndProc   = AboutWndProc;
    wcAbout.hInstance      = hInst;
    wcAbout.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(1));
    wcAbout.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcAbout.hbrBackground  = nullptr;
    wcAbout.lpszClassName  = L"HotkeyDogAboutClass";
    RegisterClassExW(&wcAbout);

    // 创建窗口
    int w = 500, h = 600;
    int sx = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int sy = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    g_hwndMain = CreateWindowExW(
        0, L"HotkeyDogClass",
        L"HotkeyDog - 热键看门狗",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        sx, sy, w, h,
        nullptr, nullptr, hInst, nullptr
    );

    if (!isAdmin) {
        g_notAdmin = true;
    }
    UpdateStatusText();

    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);

    // 消息循环
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
