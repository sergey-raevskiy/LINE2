#include "NtHandle.h"
#include "NtException.h"

#include <obfuncs.h>
#include <assert.h>

NtHandle::NtHandle()
    : m_Handle(NULL)
{
}

NtHandle::NtHandle(HANDLE Handle)
    : m_Handle(Handle)
{
}

NtHandle::NtHandle(const NtHandle & other)
{
    NT_ERR_E(NtDuplicateObject(NtCurrentProcess(),
                               other.m_Handle,
                               NtCurrentProcess(),
                               &m_Handle,
                               0,
                               0,
                               DUPLICATE_SAME_ACCESS | DUPLICATE_SAME_ATTRIBUTES));
}

PHANDLE NtHandle::operator &()
{
    CloseHandle();
    return &m_Handle;
}

NtHandle::operator HANDLE() const
{
    return m_Handle;
}

void NtHandle::CloseHandle()
{
    if (m_Handle != NULL)
    {
        NTSTATUS Status = NtClose(m_Handle);
        assert(NT_SUCCESS(Status));
    }
}
