// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "KeyTable.h"

#define MSG_MAP_ID_EDIT 1

class MainDlg : public CDialogImpl<MainDlg>, public CUpdateUI<MainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

    MainDlg();
    ~MainDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(MainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(MainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_TIMER(OnTimer)
        COMMAND_ID_HANDLER_EX(ID_FILE_EXIT, OnMenuFileExit)
        COMMAND_ID_HANDLER_EX(ID_HELP_ABOUT, OnMenuHelpAbout)
    ALT_MSG_MAP(MSG_MAP_ID_EDIT)
        MSG_WM_KEYDOWN(OnKeyDownInEdit)
        MSG_WM_CHAR(OnCharInEdit)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    void OnClose();

    void OnMenuFileExit(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnMenuHelpAbout(UINT uNotifyCode, int nID, CWindow wndCtl);

    void OnTimer(UINT_PTR nIDEvent);

    void OnKeyDownInEdit(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnCharInEdit(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
    BOOL Init();
	void CloseDialog(int nVal);
    void Cleanup();

    void War3Monitor();

private:
    CStatusBarCtrl m_statusBar;
    KeyTable m_keyTable;
    CContainedWindow m_editNum7, m_editNum8, m_editNum4, m_editNum5, m_editNum1, m_editNum2;
};
