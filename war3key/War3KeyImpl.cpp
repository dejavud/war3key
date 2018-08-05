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

    KeyReplaceTable::iterator it = m_keyReplaceTable.find(srcKey);
    if (it != m_keyReplaceTable.end()) {
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

void War3KeyImpl::SetKeyReplaceTable(const KeyReplaceTable& table)
{
    m_keyReplaceTable = table;
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
