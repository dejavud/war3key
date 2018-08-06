#pragma once

#include <map>
typedef std::map<DWORD, DWORD> KeyReplaceTable;
typedef std::map<DWORD, CString> KeyNameTable;

class KeyTable
{
public:
    KeyTable();
    ~KeyTable();

    KeyReplaceTable& ReplaceTable();
    CString GetKeyName(DWORD vkCode) const;

    void Init();

private:
    KeyReplaceTable m_replaceTable;
    KeyNameTable m_nameTable;
};

