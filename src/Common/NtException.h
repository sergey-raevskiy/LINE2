#pragma once

#include "Exception.h"

class NtException : public Exception
{
public:
    NtException(NTSTATUS Status);

private:
    NTSTATUS m_Status;
};
