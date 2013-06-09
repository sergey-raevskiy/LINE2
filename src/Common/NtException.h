#pragma once

#include "Exception.h"

class NtException : public Exception
{
public:
    NtException(NTSTATUS Status);

private:
    NTSTATUS m_Status;
};

// A helper macro for NTSTATUS wrapping.

#define NT_ERR_E(Expr) do {                                                   \
    NTSTATUS Status = ( Expr );                                               \
                                                                              \
    if (!NT_SUCCESS(Status))                                                  \
    {                                                                         \
        throw NtException(Status);                                            \
    }                                                                         \
} while (0)
