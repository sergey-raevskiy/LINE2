#ifndef _LNEXEC_H_
#define _LNEXEC_H_

#ifdef __cplusplus
extern "C" {
#endif

void InitSyscallInterceptor();
void HandleSyscall();

#ifdef __cplusplus
};
#endif

#endif /* _LNEXEC_H_ */
