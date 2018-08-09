// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include "War3KeyImpl.h"


#define WAR3_WND_CLASSNAME _T("Warcraft III")
#define WAR3_WND_WINDOWNAME _T("Warcraft III")


MainDlg::MainDlg() :
    m_editNum7(this, MSG_MAP_ID_1),
    m_editNum8(this, MSG_MAP_ID_1),
    m_editNum4(this, MSG_MAP_ID_1),
    m_editNum5(this, MSG_MAP_ID_1),
    m_editNum1(this, MSG_MAP_ID_1),
    m_editNum2(this, MSG_MAP_ID_1),
    m_minimizeToTray(FALSE)
{

}

MainDlg::~MainDlg()
{

}

BOOL MainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL MainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT MainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

    Init();

	return FALSE; // return TRUE unless you set the focus to a control
}

LRESULT MainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

BOOL MainDlg::Init()
{
    m_editNum7.SubclassWindow(GetDlgItem(IDC_EDIT_NUM7));
    m_editNum8.SubclassWindow(GetDlgItem(IDC_EDIT_NUM8));
    m_editNum4.SubclassWindow(GetDlgItem(IDC_EDIT_NUM4));
    m_editNum5.SubclassWindow(GetDlgItem(IDC_EDIT_NUM5));
    m_editNum1.SubclassWindow(GetDlgItem(IDC_EDIT_NUM1));
    m_editNum2.SubclassWindow(GetDlgItem(IDC_EDIT_NUM2));

    m_configFile.Init();
    if (!m_configFile.Load())
        m_configFile.Save();  // if the config file dose not exist, new one

    InitKeyEdits();

    GetDlgItem(IDC_STATIC).SetFocus(); // ensure that no edit control gets the focus when dialog startup

    m_statusBar.Attach(GetDlgItem(IDC_STATUSBAR));
    m_statusBar.SetSimple(TRUE);

    CString s;
    s.LoadString(IDS_STATUSINFO_READY);
    m_statusBar.SetText(SB_SIMPLEID, s);

    m_trayIcon.Install(m_hWnd, WM_TRAY_ICON);
    m_trayMenu.Init(m_hWnd);
    m_minimizeToTray = m_configFile.GetMinimizeToTray();
    CheckMinimizeToTray(m_minimizeToTray);

    SetTimer(TIMER_ID_MONITOR, 500);

    return TRUE;
}

void MainDlg::InitKeyEdits()
{
    War3KeyImpl& impl = War3KeyImpl::Instance();

    KeyReplaceTable& savedKeys = m_configFile.GetSavedKeys();
    KeyReplaceTable& keyReplaceTable = impl.GetKeyReplaceTable();
    keyReplaceTable = savedKeys;
    m_editNum7.SetWindowText(impl.GetKeyName(keyReplaceTable[VK_NUMPAD7]));
    m_editNum8.SetWindowText(impl.GetKeyName(keyReplaceTable[VK_NUMPAD8]));
    m_editNum4.SetWindowText(impl.GetKeyName(keyReplaceTable[VK_NUMPAD4]));
    m_editNum5.SetWindowText(impl.GetKeyName(keyReplaceTable[VK_NUMPAD5]));
    m_editNum1.SetWindowText(impl.GetKeyName(keyReplaceTable[VK_NUMPAD1]));
    m_editNum2.SetWindowText(impl.GetKeyName(keyReplaceTable[VK_NUMPAD2]));
}

void MainDlg::Cleanup()
{
    KillTimer(TIMER_ID_MONITOR);

    m_trayIcon.Remove();

    War3KeyImpl::Instance().UninstallHook();
    War3KeyImpl::Instance().SetDebugPrivilege(FALSE);

    m_configFile.Save();
}

void MainDlg::CloseDialog(int nVal)
{
    Cleanup();

    DestroyWindow();
    ::PostQuitMessage(nVal);
}

LRESULT MainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_minimizeToTray)
        ShowWindow(SW_HIDE);
    else
        CloseDialog(0);
    return 0;
}

LRESULT MainDlg::OnMenuFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    CloseDialog(0);
    return 0;
}

LRESULT MainDlg::OnMenuFileReset(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_configFile.ResetKeys();
    m_configFile.Save();

    InitKeyEdits();

    return 0;
}

LRESULT MainDlg::OnMenuMinimizeToTray(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_minimizeToTray = !m_minimizeToTray;
    CheckMinimizeToTray(m_minimizeToTray);
    
    m_configFile.SetMinimizeToTray(m_minimizeToTray);
    m_configFile.Save();

    return 0;
}

LRESULT MainDlg::OnMenuHelpAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    CString title, tooltips;
    title.LoadString(IDR_MAINFRAME);
    tooltips.LoadString(IDS_TOOLTIPS);
    MessageBox(tooltips, title, MB_OK | MB_ICONINFORMATION);
    return 0;
}

LRESULT MainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UINT_PTR nIDEvent = (UINT_PTR)wParam;
    if (nIDEvent == TIMER_ID_MONITOR)
        War3Monitor();
    return 0;
}

void MainDlg::War3Monitor()
{
    War3KeyImpl& impl = War3KeyImpl::Instance();

    HWND hWar3Wnd = ::FindWindow(WAR3_WND_CLASSNAME, WAR3_WND_WINDOWNAME);
    if (hWar3Wnd != NULL) { // found the game window!
        if (impl.IsHooked())
            return;
        impl.SetDebugPrivilege(TRUE);
        if (impl.InstallHook(hWar3Wnd)) {
            CString s;
            s.LoadString(IDS_STATUSINFO_ACTIVATED);
            m_statusBar.SetText(SB_SIMPLEID, s);
        }
    }
    else {
        if (impl.IsHooked()) {
            impl.UninstallHook(); 
            impl.SetDebugPrivilege(FALSE);
            CString s;
            s.LoadString(IDS_STATUSINFO_DEACTIVATED);
            m_statusBar.SetText(SB_SIMPLEID, s);
        }
    }
}

LRESULT MainDlg::OnKeyDownInEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HWND hEditWnd = ::GetFocus();
    ATLASSERT(hEditWnd != NULL);

    DWORD vkCode = (DWORD)wParam;

    CEdit editControl(hEditWnd);
    CString keyName = War3KeyImpl::Instance().GetKeyName(vkCode);
    editControl.SetWindowText(keyName);

    if (keyName.IsEmpty()) // this keystroke is not supported to replace
        return 0;

    KeyReplaceTable& keyReplaceTable = War3KeyImpl::Instance().GetKeyReplaceTable();
    if (hEditWnd == m_editNum7)
        keyReplaceTable[VK_NUMPAD7] = vkCode;
    else if (hEditWnd == m_editNum8)
        keyReplaceTable[VK_NUMPAD8] = vkCode;
    else if (hEditWnd == m_editNum4)
        keyReplaceTable[VK_NUMPAD4] = vkCode;
    else if (hEditWnd == m_editNum5)
        keyReplaceTable[VK_NUMPAD5] = vkCode;
    else if (hEditWnd == m_editNum1)
        keyReplaceTable[VK_NUMPAD1] = vkCode;
    else if (hEditWnd == m_editNum2)
        keyReplaceTable[VK_NUMPAD2] = vkCode;

    m_configFile.GetSavedKeys() = keyReplaceTable;
    m_configFile.Save();

    return 0;
}

LRESULT MainDlg::OnCharInEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // bypass
    return 0;
}

LRESULT MainDlg::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (LOWORD(lParam) == WM_RBUTTONUP) {
        m_trayMenu.Show();
    }
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) {
        ShowWindow(SW_SHOW);
    }

    return 0;
}

LRESULT MainDlg::OnTrayExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    CloseDialog(0);
    return 0;
}

void MainDlg::CheckMinimizeToTray(BOOL& action)
{
    CMenuHandle menuMain(::GetMenu(m_hWnd));
    CMenuHandle menuFile(menuMain.GetSubMenu(0));

    menuFile.CheckMenuItem(ID_FILE_MINIMIZETOTRAY, MF_BYCOMMAND | action ? MF_CHECKED : MF_UNCHECKED);
}
