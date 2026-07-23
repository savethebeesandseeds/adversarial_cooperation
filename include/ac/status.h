#ifndef AC_STATUS_H
#define AC_STATUS_H

/* Shared result codes for the educational protocol components. */
typedef enum {
    AC_OK = 0,
    AC_ERR_ARGUMENT,
    AC_ERR_LENGTH,
    AC_ERR_FORMAT,
    AC_ERR_CONTEXT,
    AC_ERR_STATE,
    AC_ERR_DUPLICATE,
    AC_ERR_CONFLICT,
    AC_ERR_INVALID_OPENING,
    AC_ERR_TERMINAL,
    AC_ERR_CRYPTO,
    AC_ERR_CAPACITY
} ac_status;

#endif
