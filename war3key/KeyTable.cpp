#include "stdafx.h"
#include "KeyTable.h"


KeyTable::KeyTable()
{
    Init();
}

KeyTable::~KeyTable()
{
}

KeyReplaceTable& KeyTable::ReplaceTable()
{
    return m_replaceTable;
}

KeyNameTable& KeyTable::NameTable()
{
    return m_nameTable;
}

void KeyTable::Init()
{
    // Virtual Keys range is from 0x01 to 0xff
    for (DWORD i = 0x01; i <= 0xff; i++)
        m_replaceTable[i] = i;

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

CString KeyTable::GetKeyName(DWORD vkCode) const
{
    KeyNameTable::const_iterator it = m_nameTable.find(vkCode);
    if (it != m_nameTable.end())
        return it->second;
    else
        return _T("");
}
