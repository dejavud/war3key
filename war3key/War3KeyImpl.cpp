#include "stdafx.h"
#include "War3KeyImpl.h"
#include <Psapi.h>

#define WAR3_GAME_DLL_NAME _T("Game.dll")
#define WAR3_CHATBOX_OFFSET_1_27_0_52240 0xBDAA14  // game version 1.27.0.52240

War3KeyImpl& War3KeyImpl::Instance()
{
    static War3KeyImpl instance;
    return instance;
}

War3KeyImpl::War3KeyImpl() :
    m_hook(NULL),
    m_hWar3Wnd(NULL),
    m_war3Pid(0),
    m_war3GameDllBaseAddr(NULL)
{
    InitKeyNameTable();
}

War3KeyImpl::~War3KeyImpl()
{
}

BOOL War3KeyImpl::InstallHook(HWND hWar3Wnd)
{
    ATLASSERT(m_hook == NULL); 
    ATLASSERT(hWar3Wnd != NULL);

    HINSTANCE hInstance = ::GetModuleHandle(NULL);
    m_hook = ::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    if (m_hook != NULL) {
        m_hWar3Wnd = hWar3Wnd;
        ::GetWindowThreadProcessId(m_hWar3Wnd, &m_war3Pid);
        m_war3GameDllBaseAddr = RetrieveGameDllBaseAddr(m_war3Pid);

        return TRUE;
    }
    else {
        return FALSE;
    }
}

void War3KeyImpl::UninstallHook()
{
    if (m_hook == NULL)
        return;

    ::UnhookWindowsHookEx(m_hook);
    m_hook = NULL;
    m_hWar3Wnd = NULL;
    m_war3Pid = 0;
}

BOOL War3KeyImpl::IsHooked() const
{
    return m_hook != NULL;
}

LRESULT CALLBACK War3KeyImpl::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    War3KeyImpl& impl = War3KeyImpl::Instance();

    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        PKBDLLHOOKSTRUCT pKDHStruct = (PKBDLLHOOKSTRUCT)lParam;
        ATLASSERT(pKDHStruct != NULL);
        if (impl.ReplaceKey(pKDHStruct->vkCode))
            return 1; // a nonzero value to prevent the system from passing the message to the rest
    }

    return ::CallNextHookEx(impl.m_hook, nCode, wParam, lParam);
}

BOOL War3KeyImpl::ReplaceKey(DWORD srcKey)
{
    ATLASSERT(m_hWar3Wnd != NULL);

    if (::GetForegroundWindow() != m_hWar3Wnd)
        return FALSE;

    if (IsChatBoxOpen()) // may NOT work properly
        return FALSE;

    KeyReplaceTable::iterator it = m_replaceTable.find(srcKey);
    if (it != m_replaceTable.end()) {
        BYTE destKey = (BYTE)it->second;
        BYTE scanCode = (BYTE)::MapVirtualKey(destKey, MAPVK_VK_TO_VSC);
        ::keybd_event(destKey, scanCode, 0, 0);
        ::keybd_event(destKey, scanCode, KEYEVENTF_KEYUP, 0);
        return TRUE;
    }

    return FALSE;
}

BOOL War3KeyImpl::IsChatBoxOpen()
{
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ, FALSE, m_war3Pid);
    if (hProcess == NULL)
        return FALSE;

    if (m_war3GameDllBaseAddr == NULL) // not found base address of "Game.dll" from "war3.exe"
        return FALSE;

    LPCVOID pBase = (LPCVOID)((DWORD)m_war3GameDllBaseAddr + WAR3_CHATBOX_OFFSET_1_27_0_52240); // magic number, found by "cheat engine", maybe vary with the game version(currently 1.27.0.52240). fix me~
    int chatEnabled = 0;
    BOOL r = ::ReadProcessMemory(hProcess, pBase, &chatEnabled, sizeof(chatEnabled), NULL);
    ::CloseHandle(hProcess);

    return (chatEnabled != 0);
}

LPVOID War3KeyImpl::RetrieveGameDllBaseAddr(DWORD war3Pid)
{
    LPVOID result = NULL;

    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, m_war3Pid);
    if (hProcess != NULL) {
        HMODULE hMods[1024];
        DWORD cbNeeded = 0;
        if (::EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
            for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                HMODULE hMod = hMods[i];

                TCHAR szModName[MAX_PATH];
                if (::GetModuleBaseName(hProcess, hMod, szModName, sizeof(szModName) / sizeof(TCHAR))) {
                    CString modName(szModName);
                    if (modName.CompareNoCase(WAR3_GAME_DLL_NAME) == 0) {
                        MODULEINFO modInfo = { 0 };
                        if (::GetModuleInformation(hProcess, hMod, &modInfo, sizeof(modInfo))) {
                            result = modInfo.lpBaseOfDll;
                        }
                        break;
                    }
                }
            }
        }
    }

    return result;
}

BOOL War3KeyImpl::SetDebugPrivilege(BOOL enable)
{
    HANDLE token = NULL;
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token))
        return FALSE;

    LUID luid;
    if (!::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
        return FALSE;

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
    if (!::AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL))
        return FALSE;
    ::CloseHandle(token);
    return TRUE;
}

void War3KeyImpl::InitKeyNameTable()
{
    m_replaceTable.clear();

    m_nameTable[VK_NUMPAD0] = _T("Num 0");
    m_nameTable[VK_NUMPAD1] = _T("Num 1");
    m_nameTable[VK_NUMPAD2] = _T("Num 2");
    m_nameTable[VK_NUMPAD3] = _T("Num 3");
    m_nameTable[VK_NUMPAD4] = _T("Num 4");
    m_nameTable[VK_NUMPAD5] = _T("Num 5");
    m_nameTable[VK_NUMPAD6] = _T("Num 6");
    m_nameTable[VK_NUMPAD7] = _T("Num 7");
    m_nameTable[VK_NUMPAD8] = _T("Num 8");
    m_nameTable[VK_NUMPAD9] = _T("Num 9");

    m_nameTable[VK_MULTIPLY] = _T("*");
    m_nameTable[VK_ADD] = _T("+");
    m_nameTable[VK_SUBTRACT] = _T("-");
    m_nameTable[VK_DECIMAL] = _T(".");
    m_nameTable[VK_DIVIDE] = _T("/");

    m_nameTable[VK_F1] = _T("F1");
    m_nameTable[VK_F2] = _T("F2");
    m_nameTable[VK_F3] = _T("F3");
    m_nameTable[VK_F4] = _T("F4");
    m_nameTable[VK_F5] = _T("F5");
    m_nameTable[VK_F6] = _T("F6");
    m_nameTable[VK_F7] = _T("F7");
    m_nameTable[VK_F8] = _T("F8");
    m_nameTable[VK_F9] = _T("F9");
    m_nameTable[VK_F10] = _T("F10");
    m_nameTable[VK_F11] = _T("F11");
    m_nameTable[VK_F12] = _T("F12");

    m_nameTable[VK_NUMLOCK] = _T("NumLk");
    m_nameTable[VK_SCROLL] = _T("SrcLk");

    m_nameTable[VK_SPACE] = _T("Space");
    m_nameTable[VK_PRIOR] = _T("PgUp");
    m_nameTable[VK_NEXT] = _T("PgDn");
    m_nameTable[VK_LEFT] = _T("Left");
    m_nameTable[VK_UP] = _T("Up");
    m_nameTable[VK_RIGHT] = _T("Right");
    m_nameTable[VK_DOWN] = _T("Down");
    m_nameTable[VK_INSERT] = _T("Insert");
    m_nameTable[VK_DELETE] = _T("Delete");

    m_nameTable[VK_LWIN] = _T("LWin");
    m_nameTable[VK_RWIN] = _T("RWin");

    m_nameTable[VK_SHIFT] = _T("Shift");
    m_nameTable[VK_CONTROL] = _T("Ctrl");
    m_nameTable[VK_BACK] = _T("Back");
    m_nameTable[VK_TAB] = _T("Tab");
    m_nameTable[VK_CAPITAL] = _T("CapsLk");

    // VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
    for (DWORD i = 0x30; i <= 0x39; i++) {
        CString s;
        s.Format(_T("%c"), (TCHAR)i);
        m_nameTable[i] = s;
    }

    // VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
    for (DWORD i = 0x41; i <= 0x5A; i++) {
        CString s;
        s.Format(_T("%c"), (TCHAR)i);
        m_nameTable[i] = s;
    }

    m_nameTable[VK_OEM_1] = _T(";");
    m_nameTable[VK_OEM_PLUS] = _T("=");
    m_nameTable[VK_OEM_COMMA] = _T(",");
    m_nameTable[VK_OEM_MINUS] = _T("-");
    m_nameTable[VK_OEM_PERIOD] = _T(".");
    m_nameTable[VK_OEM_2] = _T("/");
    m_nameTable[VK_OEM_3] = _T("~");
    m_nameTable[VK_OEM_4] = _T("[");
    m_nameTable[VK_OEM_5] = _T("\\");
    m_nameTable[VK_OEM_6] = _T("]");
    m_nameTable[VK_OEM_7] = _T("'");
}

CString War3KeyImpl::GetKeyName(DWORD vkCode) const
{
    KeyNameTable::const_iterator it = m_nameTable.find(vkCode);
    if (it != m_nameTable.end())
        return it->second;
    else
        return _T("");
}

KeyReplaceTable& War3KeyImpl::ReplaceTable()
{
    return m_replaceTable;
}
