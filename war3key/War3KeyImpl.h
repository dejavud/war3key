#pragma once

#include <map>
typedef std::map<DWORD, DWORD> KeyReplaceTable;
typedef std::map<DWORD, CString> KeyNameTable;

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
    KeyReplaceTable& GetKeyReplaceTable();

private:
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    BOOL ReplaceKey(DWORD srcKey);

    BOOL IsChatBoxOpen(); // determine whether the chat box is open in game

    LPVOID RetrieveGameDllBaseAddr(DWORD war3Pid);

private:
    HHOOK m_hook;
    HWND m_hWar3Wnd;
    DWORD m_war3Pid;
    LPVOID m_war3GameDllBaseAddr;
    KeyReplaceTable m_replaceTable;
    KeyNameTable m_nameTable;
};

