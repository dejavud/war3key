#pragma once

#include "War3KeyImpl.h"

class Config
{
public:
    Config();
    ~Config();

    void Init();

    BOOL Load();
    BOOL Save();

private:
    const CString& GetConfigFilePath();

    CString U8toCS(const std::string& s);
    std::string CStoU8(const CString& s);

public:
    BOOL m_minimizeToTray;
    KeyConfig m_keyConfig;

private:
    CString m_filePath;
};

