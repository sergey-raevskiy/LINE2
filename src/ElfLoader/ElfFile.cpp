#include <windef.h>
#include <rtlfuncs.h>

// Just to make sure that our project is able to be compiled.

void TestFunction()
{
    UNICODE_STRING Str;

    RtlInitUnicodeString(&Str, L"HELLO WORLD");
}
