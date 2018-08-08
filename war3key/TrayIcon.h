#pragma once

#include "CommonDefs.h"

enum TrayBallonStyle
{
    CBS_NONE = 0,
    CBS_INFO,
    CBS_WARNING,
    CBS_ERROR,
    CBS_USER,
};

class TrayIcon
{
public:
    TrayIcon();
    ~TrayIcon();

public:
    BOOL Install(HWND hWnd, UINT msg);
    void Remove();

    BOOL ShowBallon(const CString& info, const CString& title, TrayBallonStyle style);

private:
    NOTIFYICONDATA m_nid;
};

