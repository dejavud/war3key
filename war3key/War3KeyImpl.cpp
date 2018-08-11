#include "stdafx.h"
#include "War3KeyImpl.h"
#include <Psapi.h>

#define WAR3_GAME_EXE_NAME _T("War3.exe")
#define WAR3_GAME_DLL_NAME _T("Game.dll")
#define WAR3_CHATBOX_FLAG_OFFSET_1_20_E_BASE_ON_EXE 0x0005CB8C
#define WAR3_CHATBOX_FLAG_OFFSET_1_24_E_BASE_ON_DLL 0x00AE8450
#define WAR3_CHATBOX_FLAG_OFFSET_1_27_A_BASE_ON_DLL 0x00BDAA14


War3KeyImpl& War3KeyImpl::Instance()
{
    static War3KeyImpl instance;
    return instance;
}

War3KeyImpl::War3KeyImpl() :
    m_hook(NULL),
    m_hWar3Wnd(NULL),
    m_war3Pid(0),
    m_war3GameChatBoxFlagAddr(0)
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
        RetrieveGameInfo(m_war3Pid);

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

    if (m_keyConfig.m_disableLWin && srcKey == VK_LWIN)
        return TRUE;

    KeyReplaceTable& replaceKeyTable = m_keyConfig.m_keyReplaceTable;
    for (KeyReplaceTable::iterator it = replaceKeyTable.begin(); it != replaceKeyTable.end(); it++) {
        if (it->second == srcKey) {
            BYTE destKey = (BYTE)it->first;
            BYTE scanCode = (BYTE)::MapVirtualKey(destKey, MAPVK_VK_TO_VSC);
            ::keybd_event(destKey, scanCode, 0, 0);
            ::keybd_event(destKey, scanCode, KEYEVENTF_KEYUP, 0);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL War3KeyImpl::IsChatBoxOpen()
{
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ, FALSE, m_war3Pid);
    if (hProcess == NULL)
        return FALSE;

    if (m_war3GameChatBoxFlagAddr == 0) // not found chat box status flag address from "Game.dll" or "War3.exe"
        return FALSE;

    LPCVOID pBase = (LPCVOID)m_war3GameChatBoxFlagAddr; // magic number, found by "cheat engine", maybe vary with the game version.
    int chatEnabled = 0;
    BOOL r = ::ReadProcessMemory(hProcess, pBase, &chatEnabled, sizeof(chatEnabled), NULL);
    ::CloseHandle(hProcess);

    return (chatEnabled != 0);
}

void War3KeyImpl::RetrieveGameInfo(DWORD war3Pid)
{
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, m_war3Pid);
    if (hProcess == NULL)
        return;

    // get "game.dll" path
    TCHAR tmpPath[MAX_PATH] = { 0 };
    ::GetModuleFileNameEx(hProcess, NULL, tmpPath, MAX_PATH);
    CString exePath(tmpPath);
    CString dllPath = exePath.Left(exePath.ReverseFind(_T('\\')) + 1) + WAR3_GAME_DLL_NAME;

    // get file version number of "game.dll"
    DWORD dwHandle = 0;
    DWORD fviSize = ::GetFileVersionInfoSize(dllPath, &dwHandle);

    CString verStr;
    char* fviData = new char[fviSize];
    if (::GetFileVersionInfo(dllPath, dwHandle, fviSize, fviData)) {
        LPVOID pBuffer = NULL;
        UINT size = 0;
        if (::VerQueryValue(fviData, _T("\\"), &pBuffer, &size)) {
            VS_FIXEDFILEINFO* pFileInfo = (VS_FIXEDFILEINFO*)pBuffer;
            ATLASSERT(pFileInfo != NULL);
            verStr.Format(_T("%d.%d.%d.%d"),
                (pFileInfo->dwFileVersionMS >> 16) & 0xffff,
                (pFileInfo->dwFileVersionMS >> 0) & 0xffff,
                (pFileInfo->dwFileVersionLS >> 16) & 0xffff,
                (pFileInfo->dwFileVersionLS >> 0) & 0xffff
            );
        }
    }
    delete[] fviData;

    // get chat box status flag address
    if (verStr.CompareNoCase(_T("1.20.4.6074")) == 0) {  // 1.20e, offset based of "war3.exe"
        m_war3GameChatBoxFlagAddr = RetrieveChatBoxStatusFlagAddr(hProcess, WAR3_GAME_EXE_NAME, WAR3_CHATBOX_FLAG_OFFSET_1_20_E_BASE_ON_EXE);
    }
    else if (verStr.CompareNoCase(_T("1.24.4.6387")) == 0) { // 1.24e, offset based of "game.dll"
        m_war3GameChatBoxFlagAddr = RetrieveChatBoxStatusFlagAddr(hProcess, WAR3_GAME_DLL_NAME, WAR3_CHATBOX_FLAG_OFFSET_1_24_E_BASE_ON_DLL);
    }
    else if (verStr.CompareNoCase(_T("1.27.0.52240")) == 0) { // 1.27a, offset based of "game.dll"
        m_war3GameChatBoxFlagAddr = RetrieveChatBoxStatusFlagAddr(hProcess, WAR3_GAME_DLL_NAME, WAR3_CHATBOX_FLAG_OFFSET_1_27_A_BASE_ON_DLL);
    }
    else {
        m_war3GameChatBoxFlagAddr = 0;
    }

    ::CloseHandle(hProcess);
}

DWORD War3KeyImpl::RetrieveChatBoxStatusFlagAddr(HANDLE hProcess, CString basedModName, DWORD offset)
{
    ATLASSERT(hProcess != NULL);

    HMODULE hBasedMod = NULL;

    HMODULE hMods[1024];
    DWORD cbNeeded = 0;
    if (::EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            HMODULE hMod = hMods[i];
            TCHAR szModName[MAX_PATH];
            if (::GetModuleBaseName(hProcess, hMod, szModName, sizeof(szModName) / sizeof(TCHAR))) {
                CString modName(szModName);
                if (modName.CompareNoCase(basedModName) == 0) {
                    hBasedMod = hMod;
                    break;
                }
            }
        }
    }

    if (hBasedMod == NULL)
        return 0;

    return (DWORD)hBasedMod + offset;
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

KeyConfig& War3KeyImpl::GetKeyConfig()
{
    return m_keyConfig;
}
