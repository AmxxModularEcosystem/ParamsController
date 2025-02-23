#if defined __Cwapi_utils_included
    #endinput
#endif
#define __Cwapi_utils_included

#include <amxmodx>

stock CreateConstCvar(const sCvarName[], const sCvarValue[]) {
    set_pcvar_string(create_cvar(sCvarName, sCvarValue), sCvarValue);
}
