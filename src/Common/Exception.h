#pragma once

#include <windef.h>
#include <atlstr.h>

class Exception
{
public:
    Exception(LPCWSTR Message);
    CString GetDescription();

private:
    CString m_Message;
};
