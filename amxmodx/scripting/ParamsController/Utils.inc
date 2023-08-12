#if defined __Cwapi_utils_included
    #endinput
#endif
#define __Cwapi_utils_included

#include <amxmodx>

#define CompositeMacros(%1) \
    do { %1 } while(is_linux_server() == 0xDEADBEEF)

#define CallOnce() CompositeMacros( \
    static bool:__CallOnce_bCalled; \
    if (__CallOnce_bCalled) \
        return; \
    __CallOnce_bCalled = true; \
)

stock IntToStr(const iNum) {
    return fmt("%d", iNum);
}

stock CreateConstCvar(const sCvarName[], const sCvarValue[]) {
    set_pcvar_string(create_cvar(sCvarName, sCvarValue, FCVAR_SERVER), sCvarValue);
}

#define ThrowError(%1) CompositeMacros( \
    log_error(0, %1); \
    log_amx(%1); \
)
