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
    KeyNameTable& NameTable();

    void Init();
    
    CString GetKeyName(DWORD vkCode) const;

private:
    KeyReplaceTable m_replaceTable;
    KeyNameTable m_nameTable;
};

