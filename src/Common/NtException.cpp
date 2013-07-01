#include "NtException.h"

static CString FormatStatus(NTSTATUS Status)
{
    CString Message;

    // Basic status formatting.
    Message.Format(L"%08x", Status);

    return Message;
}

NtException::NtException(NTSTATUS Status)
    : Exception(FormatStatus(Status))
    , m_Status(Status)
{
}
