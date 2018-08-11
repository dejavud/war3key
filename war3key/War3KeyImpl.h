#pragma once

#include <map>
typedef std::map<DWORD, DWORD> KeyReplaceTable;
typedef std::map<DWORD, CString> KeyNameTable;

struct KeyConfig
{
    KeyReplaceTable m_keyReplaceTable;
    BOOL m_disableLWin;

    KeyConfig() : m_disableLWin(FALSE) 
    {}

    void Reset() 
    {
        m_keyReplaceTable[VK_NUMPAD7] = VK_NUMPAD7;
        m_keyReplaceTable[VK_NUMPAD8] = VK_NUMPAD8;
        m_keyReplaceTable[VK_NUMPAD4] = VK_NUMPAD4;
        m_keyReplaceTable[VK_NUMPAD5] = VK_NUMPAD5;
        m_keyReplaceTable[VK_NUMPAD1] = VK_NUMPAD1;
        m_keyReplaceTable[VK_NUMPAD2] = VK_NUMPAD2;

        m_disableLWin = FALSE;
    }
};

class War3KeyImpl
{
public:
    // singleton
    static War3KeyImpl& Instance();

private:
    War3KeyImpl();
    ~War3KeyImpl();

    // non-copyable
    War3KeyImpl(const War3KeyImpl&);
    War3KeyImpl& operator=(const War3KeyImpl&); 

public:
    BOOL InstallHook(HWND hWar3Wnd);
    void UninstallHook();
    BOOL IsHooked() const;

    BOOL SetDebugPrivilege(BOOL enable);

    void InitKeyNameTable();
    CString GetKeyName(DWORD vkCode) const;

    KeyConfig& GetKeyConfig();

private:
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    BOOL ReplaceKey(DWORD srcKey);

    BOOL IsChatBoxOpen(); // determine whether the chat box is open in game

    void RetrieveGameInfo(DWORD war3Pid);
    DWORD RetrieveChatBoxStatusFlagAddr(HANDLE hProcess, CString basedModName, DWORD offset);

private:
    HHOOK m_hook;
    HWND m_hWar3Wnd;
    DWORD m_war3Pid;
    DWORD m_war3GameChatBoxFlagAddr;
    KeyNameTable m_nameTable;
    KeyConfig m_keyConfig;
};

