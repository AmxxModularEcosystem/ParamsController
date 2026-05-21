#include <amxmodx>
#include "ParamsController/API/Utils"

API_General_Register() {
    register_native("ParamsController_Init", "@API_Init");
    register_native("ParamsController_SetCell", "@API_SetCell");
    register_native("ParamsController_SetString", "@API_SetString");
}

@API_Init() {
    PluginInit();
}

@API_SetCell() {
    enum {Arg_Cell = 1}

    API_CheckPluginInited();
    API_CheckCurrentParams();

    return TrieSetCell(g_tCurrentParams, g_sCurrentParamName, get_param(Arg_Cell));
}

@API_SetString() {
    enum {Arg_String = 1}

    API_CheckPluginInited();
    API_CheckCurrentParams();

    static sString[PARAM_LONG_STRING_MAX_LEN];
    get_string(Arg_String, sString, charsmax(sString));

    return TrieSetString(g_tCurrentParams, g_sCurrentParamName, sString);
}