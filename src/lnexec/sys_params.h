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

/* ldt & tls */

typedef struct {
    unsigned int  entry_number;
    unsigned int  base_addr;
    unsigned int  limit;
    unsigned int  seg_32bit:1;
    unsigned int  contents:2;
    unsigned int  read_exec_only:1;
    unsigned int  limit_in_pages:1;
    unsigned int  seg_not_present:1;
    unsigned int  useable:1;
#ifdef __x86_64__
    unsigned int  lm:1;
#endif
} linux_user_desc_t;

/* time */

typedef int linux_time_t;

#endif /* _SYS_PARAMS_H_ */
