#ifndef _GS_SUPPORT_
#define _GS_SUPPORT_

#ifdef __cplusplus
extern "C" {
#endif

#include <windef.h>

void GSLoad(USHORT Value);
PVOID GSTranslateAddress(PVOID Address);

#ifdef __cplusplus
};
#endif

#endif /* _GS_SUPPORT_ */