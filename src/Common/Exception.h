#pragma once

#include <windef.h>
#include <atlstr.h>

class Exception
{
public:
    Exception(LPCWSTR Message);

private:
    CString m_Message;
};
