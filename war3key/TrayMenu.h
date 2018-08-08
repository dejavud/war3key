#pragma once

class TrayMenu
{
public:
    TrayMenu();
    ~TrayMenu();

public:
    BOOL Init(HWND hWnd);
    void Show();
    
private:
    HWND m_hWnd;
    CMenu m_menu;
};

