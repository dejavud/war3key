#include "stdafx.h"
#include "TrayMenu.h"
#include "resource.h"


TrayMenu::TrayMenu() :
    m_hWnd(NULL)
{
}


TrayMenu::~TrayMenu()
{
}

BOOL TrayMenu::Init(HWND hWnd)
{
    ATLASSERT(hWnd != NULL);

    if (!m_menu.IsNull())
        m_menu.DestroyMenu();

    if (!m_menu.LoadMenu(IDR_TRAYMENU))
        return FALSE;

    m_hWnd = hWnd;

    return TRUE;
}

void TrayMenu::Show()
{
    if (!m_menu.IsMenu())
        return; 
    
    ::SetForegroundWindow(m_hWnd);

    POINT pos = { 0 };
    ::GetCursorPos(&pos);

    CMenuHandle trayMenu(m_menu.GetSubMenu(0));
    trayMenu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, m_hWnd);

    PostMessage(m_hWnd, WM_NULL, 0, 0);
}
