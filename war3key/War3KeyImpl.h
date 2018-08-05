#pragma once

#include "KeyTable.h"

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

    void SetKeyReplaceTable(const KeyReplaceTable& table);

    BOOL SetDebugPrivilege(BOOL enable);

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
    KeyReplaceTable m_keyReplaceTable;
};

