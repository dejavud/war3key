// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include "War3KeyImpl.h"

#define TIMER_ID_MONITOR 1024
#define WAR3_WND_CLASSNAME _T("Warcraft III")
#define WAR3_WND_WINDOWNAME _T("Warcraft III")


MainDlg::MainDlg() :
    m_editNum7(this, MSG_MAP_ID_EDIT),
    m_editNum8(this, MSG_MAP_ID_EDIT),
    m_editNum4(this, MSG_MAP_ID_EDIT),
    m_editNum5(this, MSG_MAP_ID_EDIT),
    m_editNum1(this, MSG_MAP_ID_EDIT),
    m_editNum2(this, MSG_MAP_ID_EDIT)
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

    m_editNum7.SetWindowText(War3KeyImpl::Instance().GetKeyName(VK_NUMPAD7));
    m_editNum8.SetWindowText(War3KeyImpl::Instance().GetKeyName(VK_NUMPAD8));
    m_editNum4.SetWindowText(War3KeyImpl::Instance().GetKeyName(VK_NUMPAD4));
    m_editNum5.SetWindowText(War3KeyImpl::Instance().GetKeyName(VK_NUMPAD5));
    m_editNum1.SetWindowText(War3KeyImpl::Instance().GetKeyName(VK_NUMPAD1));
    m_editNum2.SetWindowText(War3KeyImpl::Instance().GetKeyName(VK_NUMPAD2));

    GetDlgItem(IDC_STATIC).SetFocus(); // ensure that no edit control gets the focus when dialog startup

    m_statusBar.Attach(GetDlgItem(IDC_STATUSBAR));
    m_statusBar.SetSimple(TRUE);

    CString s;
    s.LoadString(IDS_STATUSINFO_READY);
    m_statusBar.SetText(SB_SIMPLEID, s);

    SetTimer(TIMER_ID_MONITOR, 500);

    return TRUE;
}

void MainDlg::Cleanup()
{
    KillTimer(TIMER_ID_MONITOR);

    War3KeyImpl::Instance().UninstallHook();
    War3KeyImpl::Instance().SetDebugPrivilege(FALSE);
}

void MainDlg::CloseDialog(int nVal)
{
    Cleanup();

    DestroyWindow();
    ::PostQuitMessage(nVal);
}

void MainDlg::OnClose()
{
    CloseDialog(0);
}

void MainDlg::OnMenuFileExit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CloseDialog(0);
}

void MainDlg::OnMenuHelpAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CString title, tooltips;
    title.LoadString(IDR_MAINFRAME);
    tooltips.LoadString(IDS_TOOLTIPS);
    MessageBox(tooltips, title, MB_OK | MB_ICONINFORMATION);
}

void MainDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_ID_MONITOR)
        War3Monitor();
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

void MainDlg::OnKeyDownInEdit(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    HWND hEditWnd = ::GetFocus();
    ATLASSERT(hEditWnd != NULL);

    DWORD vkCode = nChar;

    CEdit editControl(hEditWnd);
    CString keyName = War3KeyImpl::Instance().GetKeyName(vkCode);
    editControl.SetWindowText(keyName);

    if (keyName.IsEmpty()) // this keystroke is not supported to replace
        return;

    KeyReplaceTable& keyReplaceTable = War3KeyImpl::Instance().ReplaceTable();
    if (hEditWnd == m_editNum7)
        keyReplaceTable[vkCode] = VK_NUMPAD7;
    else if (hEditWnd == m_editNum8)
        keyReplaceTable[vkCode] = VK_NUMPAD8;
    else if (hEditWnd == m_editNum4)
        keyReplaceTable[vkCode] = VK_NUMPAD4;
    else if (hEditWnd == m_editNum5)
        keyReplaceTable[vkCode] = VK_NUMPAD5;
    else if (hEditWnd == m_editNum1)
        keyReplaceTable[vkCode] = VK_NUMPAD1;
    else if (hEditWnd == m_editNum2)
        keyReplaceTable[vkCode] = VK_NUMPAD2;
}

void MainDlg::OnCharInEdit(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // bypass
}
