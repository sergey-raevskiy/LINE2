#pragma once

#include <windef.h>

class NtHandle
{
public:
    NtHandle();
    NtHandle(HANDLE Handle);
    NtHandle(const NtHandle & Other);

    PHANDLE operator &();
    operator HANDLE() const;

private:
    HANDLE m_Handle;

    void CloseHandle();
};
