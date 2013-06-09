#include "Exception.h"

Exception::Exception(LPCWSTR Message)
    : m_Message(Message)
{
}

CString Exception::GetDescription() const
{
    return m_Message;
}