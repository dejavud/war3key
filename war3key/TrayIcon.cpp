#include "stdafx.h"
#include "TrayIcon.h"
#include "resource.h"

#define TRAY_ICON_ID 1000

TrayIcon::TrayIcon()
{
    memset(&m_nid, 0, sizeof(NOTIFYICONDATA));
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
}


TrayIcon::~TrayIcon()
{
}

BOOL TrayIcon::Install(HWND hWnd, UINT msg)
{
    if (m_nid.hWnd != NULL) // already exists
        return TRUE;

    ATLASSERT(hWnd != NULL);

    m_nid.hWnd = hWnd;
    m_nid.uID = TRAY_ICON_ID;
    m_nid.hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));;
    m_nid.uCallbackMessage = msg;
    ::LoadString(::GetModuleHandle(NULL), IDR_MAINFRAME, m_nid.szTip, sizeof(m_nid.szTip));
    m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    if (!::Shell_NotifyIcon(NIM_ADD, &m_nid)) {
        m_nid.hWnd = NULL;
        return FALSE;
    }
    return TRUE;
}

void TrayIcon::Remove()
{
    m_nid.uFlags = 0;
    ::Shell_NotifyIcon(NIM_DELETE, &m_nid);

    m_nid.hWnd = NULL; // indicates that no longer work
}

BOOL TrayIcon::ShowBallon(const CString& info, const CString& title, TrayBallonStyle style)
{
    if (m_nid.hWnd == NULL)
        return FALSE;

    m_nid.uFlags |= NIF_INFO;
    _tcscpy(m_nid.szInfo, info);
    _tcscpy(m_nid.szInfoTitle, title);
    switch (style) {
    case CBS_NONE:
        m_nid.dwInfoFlags = NIIF_NONE;
        break;
    case CBS_INFO:
        m_nid.dwInfoFlags = NIIF_INFO;
        break;
    case CBS_WARNING:
        m_nid.dwInfoFlags = NIIF_WARNING;
        break;
    case CBS_ERROR:
        m_nid.dwInfoFlags = NIIF_ERROR;
        break;
    case CBS_USER:
        m_nid.dwInfoFlags = NIIF_USER;
        break;
    default:
        m_nid.dwInfoFlags = NIIF_INFO;
        break;
    }
    m_nid.dwInfoFlags |= NIIF_NOSOUND;
    m_nid.uTimeout = 1000;

    return ::Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}
