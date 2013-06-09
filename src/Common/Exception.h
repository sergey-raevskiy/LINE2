#pragma once

#include <windef.h>
#include <atlstr.h>

class Exception
{
public:
    Exception(LPCWSTR Message);
    CString GetDescription() const;

private:
    CString m_Message;
};
