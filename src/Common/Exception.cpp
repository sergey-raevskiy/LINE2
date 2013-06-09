#include "Exception.h"

Exception::Exception(LPCWSTR Message)
    : m_Message(Message)
{
}

CString Exception::GetDescription()
{
    return m_Message;
}
