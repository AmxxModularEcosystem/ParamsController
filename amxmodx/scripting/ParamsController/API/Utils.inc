#if defined __pc_core__api_utils_included
    #endinput
#endif
#define __pc_core__api_utils_included

#include <amxmodx>

stock API_CheckPluginInited() {
    if (!IsPluginInited()) {
        abort(AMX_ERR_PARAMS, "Attempt to interact with params before init them.");
    }
}

stock API_CheckCurrentParams() {
    if (g_tCurrentParams == Invalid_Trie) {
        abort(AMX_ERR_PARAMS, "Attempt to set param value outside the read callback.");
    }
}
