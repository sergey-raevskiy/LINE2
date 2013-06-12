#ifndef _SYS_PARAMS_H_
#define _SYS_PARAMS_H_

/* uname */

#define __OLD_UTS_LEN 8

typedef struct {
    char sysname[9];
    char nodename[9];
    char release[9];
    char version[9];
    char machine[9];
} linux_oldold_utsname_t;

#define __NEW_UTS_LEN 64

typedef struct {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
} linux_old_utsname_t;

typedef struct {
    char sysname[__NEW_UTS_LEN + 1];
    char nodename[__NEW_UTS_LEN + 1];
    char release[__NEW_UTS_LEN + 1];
    char version[__NEW_UTS_LEN + 1];
    char machine[__NEW_UTS_LEN + 1];
    char domainname[__NEW_UTS_LEN + 1];
} linux_new_utsname_t;

/* vectored IO */

typedef struct {
    void *iov_base;
    size_t iov_len;
} linux_iovec_t;

#endif /* _SYS_PARAMS_H_ */
