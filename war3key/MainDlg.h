// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommonDefs.h"
#include "Config.h"
#include "TrayIcon.h"
#include "TrayMenu.h"

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

	BEGIN_MSG_MAP(MainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        COMMAND_ID_HANDLER(ID_FILE_EXIT, OnMenuFileExit)
        COMMAND_ID_HANDLER(ID_FILE_MINIMIZETOTRAY, OnMenuMinimizeToTray)
        COMMAND_ID_HANDLER(ID_HELP_ABOUT, OnMenuHelpAbout)
        MESSAGE_HANDLER(WM_TRAY_ICON, OnTrayIcon)
        COMMAND_ID_HANDLER(ID_TRAY_EXIT, OnTrayExit)
    ALT_MSG_MAP(MSG_MAP_ID_1)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDownInEdit)
        MESSAGE_HANDLER(WM_CHAR, OnCharInEdit)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnMenuFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnMenuMinimizeToTray(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnMenuHelpAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnKeyDownInEdit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCharInEdit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTrayExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
    BOOL Init();
	void CloseDialog(int nVal);
    void Cleanup();

    void War3Monitor();

    void CheckMinimizeToTray(BOOL& action);

private:
    CStatusBarCtrl m_statusBar;
    CContainedWindow m_editNum7, m_editNum8, m_editNum4, m_editNum5, m_editNum1, m_editNum2;
    Config m_configFile;
    TrayIcon m_trayIcon;
    TrayMenu m_trayMenu;
    BOOL m_minimizeToTray;
};
