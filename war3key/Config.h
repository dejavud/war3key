#pragma once

#include "War3KeyImpl.h"

class Config
{
public:
    Config();
    ~Config();

    void Init();
    void ResetKeys();

    BOOL Load();
    BOOL Save();

public:
    KeyReplaceTable& GetSavedKeys();

    BOOL GetMinimizeToTray() const;
    void SetMinimizeToTray(BOOL enable);

private:
    const CString& GetConfigFilePath();

    CString U8toCS(const std::string& s);
    std::string CStoU8(const CString& s);

private:
    KeyReplaceTable m_savedKeys;
    CString m_filePath;
    BOOL m_minimizeToTray;
};

