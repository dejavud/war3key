#include "stdafx.h"
#include "Config.h"
#include <fstream>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

#define CONFIG_SETTINGS         "Settings"
#define CONFIG_SETTINGS_TRAY    "MinimizeToTray"
#define CONFIG_REPLACEKEYS      "ReplaceKeys"
#define CONFIG_KEY_NUM7         "Num7"
#define CONFIG_KEY_NUM8         "Num8"
#define CONFIG_KEY_NUM4         "Num4"
#define CONFIG_KEY_NUM5         "Num5"
#define CONFIG_KEY_NUM1         "Num1"
#define CONFIG_KEY_NUM2         "Num2"

Config::Config() :
    m_minimizeToTray(FALSE)
{
}


Config::~Config()
{
}

void Config::Init()
{
    ResetKeys();

    m_minimizeToTray = TRUE; // default checked
}

void Config::ResetKeys()
{
    m_savedKeys[VK_NUMPAD7] = VK_NUMPAD7;
    m_savedKeys[VK_NUMPAD8] = VK_NUMPAD8;
    m_savedKeys[VK_NUMPAD4] = VK_NUMPAD4;
    m_savedKeys[VK_NUMPAD5] = VK_NUMPAD5;
    m_savedKeys[VK_NUMPAD1] = VK_NUMPAD1;
    m_savedKeys[VK_NUMPAD2] = VK_NUMPAD2;
}

BOOL Config::Load()
{
    ifstream f(GetConfigFilePath());
    if (!f.is_open())
        return FALSE;

    try {
        json j;
        f >> j;

        if (j.find(CONFIG_SETTINGS) != j.end()) {
            json settings = j[CONFIG_SETTINGS];
            if (settings.is_object()) {
                m_minimizeToTray = (BOOL)settings.at(CONFIG_SETTINGS_TRAY).get<int>();
            }
        }

        if (j.find(CONFIG_REPLACEKEYS) != j.end()) {
            json keys = j[CONFIG_REPLACEKEYS];
            if (keys.is_object()) {
                m_savedKeys[VK_NUMPAD7] = (DWORD)keys.at(CONFIG_KEY_NUM7);
                m_savedKeys[VK_NUMPAD8] = (DWORD)keys.at(CONFIG_KEY_NUM8);
                m_savedKeys[VK_NUMPAD4] = (DWORD)keys.at(CONFIG_KEY_NUM4);
                m_savedKeys[VK_NUMPAD5] = (DWORD)keys.at(CONFIG_KEY_NUM5);
                m_savedKeys[VK_NUMPAD1] = (DWORD)keys.at(CONFIG_KEY_NUM1);
                m_savedKeys[VK_NUMPAD2] = (DWORD)keys.at(CONFIG_KEY_NUM2);
            }
        }
    }
    catch (const std::exception& e) {
        const char* errstr = e.what();
        // ...
    }
    
    return TRUE;
}

BOOL Config::Save()
{
    try {
        json j;

        json settings;
        settings[CONFIG_SETTINGS_TRAY] = (int)m_minimizeToTray;
        j[CONFIG_SETTINGS] = settings;

        json keys;
        keys[CONFIG_KEY_NUM7] = m_savedKeys[VK_NUMPAD7];
        keys[CONFIG_KEY_NUM8] = m_savedKeys[VK_NUMPAD8];
        keys[CONFIG_KEY_NUM4] = m_savedKeys[VK_NUMPAD4];
        keys[CONFIG_KEY_NUM5] = m_savedKeys[VK_NUMPAD5];
        keys[CONFIG_KEY_NUM1] = m_savedKeys[VK_NUMPAD1];
        keys[CONFIG_KEY_NUM2] = m_savedKeys[VK_NUMPAD2];
        j[CONFIG_REPLACEKEYS] = keys;

        ofstream f(GetConfigFilePath());
        if (!f.is_open())
            return FALSE;

        f << j.dump(4);
    }
    catch (const std::exception& e) {
        const char* errstr = e.what();
        return FALSE;
    }

    return TRUE;
}

KeyReplaceTable& Config::GetSavedKeys()
{
    return m_savedKeys;
}

BOOL Config::GetMinimizeToTray() const
{
    return m_minimizeToTray;
}

void Config::SetMinimizeToTray(BOOL enable)
{
    m_minimizeToTray = enable;
}

const CString& Config::GetConfigFilePath()
{
    if (!m_filePath.IsEmpty())
        return m_filePath;

    TCHAR appPath[MAX_PATH];
    ::GetModuleFileName(NULL, appPath, MAX_PATH);
    CString tmp = appPath;
    int index = tmp.ReverseFind('.');
    ATLASSERT(index != -1);

    m_filePath = tmp.Left(index);
    m_filePath += _T(".json");

    return m_filePath;
}

CString Config::U8toCS(const std::string& s)
{
    CStringW wstr;
    int num_required, num_written;
    num_required = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length(), NULL, 0);
    if (num_required > 0) {
        WCHAR* wbuffer = new WCHAR[num_required];  // include terminating null character
        num_written = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length(), wbuffer, num_required);
        ATLASSERT(num_written == num_required);
        wstr = wbuffer;
        delete[] wbuffer;
    }

    CString tstr;
#if defined UNICODE || defined _UNICODE
    tstr = wstr;
#else
    num_required = ::WideCharToMultiByte(CP_ACP, 0, wstr, wstr.GetLength() + 1, NULL, 0, NULL, NULL);
    if (num_required > 0) {
        char* buffer = new char[num_required];
        num_written = ::WideCharToMultiByte(CP_ACP, 0, wstr, wstr.GetLength() + 1, buffer, num_required, NULL, NULL);
        ATLASSERT(num_written == num_required);
        tstr = buffer;
        delete[] buffer;
    }
#endif

    return tstr;
}

std::string Config::CStoU8(const CString& s)
{
    CStringW wstr;
    int num_required, num_written;
#if defined UNICODE || defined _UNICODE
    wstr = s;
#else
    num_required = ::MultiByteToWideChar(CP_ACP, 0, s, s.GetLength(), NULL, 0);
    if (num_required > 0) {
        wchar_t* wbuffer = new wchar_t[num_required];
        num_written = ::MultiByteToWideChar(CP_ACP, 0, s, s.GetLength(), wbuffer, num_required);
        ATLASSERT(num_written == num_required);
        wstr = wbuffer;
        delete[] wbuffer;
    }
#endif

    std::string str;
    num_required = ::WideCharToMultiByte(CP_UTF8, 0, wstr, wstr.GetLength() + 1, NULL, 0, NULL, NULL);
    if (num_required <= 0)
        return str;

    char* buffer = new char[num_required];  // include terminating null character
    num_written = ::WideCharToMultiByte(CP_UTF8, 0, wstr, wstr.GetLength() + 1, buffer, num_required, NULL, NULL);
    ATLASSERT(num_written == num_required);
    str.assign(buffer);
    delete[] buffer;

    return str;
}