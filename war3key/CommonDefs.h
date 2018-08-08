#pragma once

#include <Windows.h>

// message map id
enum MapMsgID {
    MSG_MAP_ID_BEGIN = 0,
    MSG_MAP_ID_1,
};

// custom window message 
enum CustomWndMsg {
    WM_USER_BEGIN = WM_USER,
    WM_TRAY_ICON,   // tray icon handler message
};

// timer id
enum TimerID {
    TIMER_ID_BEGIN = 1024,
    TIMER_ID_MONITOR,
};
