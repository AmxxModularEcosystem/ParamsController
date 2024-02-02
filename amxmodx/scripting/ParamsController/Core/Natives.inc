#include <amxmodx>
#include "ParamsController/Natives"
#include "ParamsController/Core/ParamType"
#include "ParamsController/Core/Param"

#define API__CheckPluginInitedP(%1) CompositeMacros( \
    if (!IsPluginInited()) { \
        log_error(1, "Attempt to interact with params before init them."); \
        return %1; \
    } \
)
#define API__CheckPluginInited() API__CheckPluginInitedP( )

#define API__CheckCurrentParamsP(%1) CompositeMacros( \
    if (g_tCurrentParams == Invalid_Trie) { \
        log_error(1, "Attempt to set param value outside the read callback."); \
        return %1; \
    } \
)
#define API__CheckCurrentParams() API__CheckCurrentParamsP( )

public plugin_natives() {
    Natives_Init("ParamsController");

    Natives_Reg("Init");

    Natives_Reg("SetCell");
    Natives_Reg("SetString");

    Natives_Reg("ParamType_Register");
    Natives_Reg("ParamType_SetReadCallback");

    Natives_Reg("Param_Construct");
    Natives_Reg("Param_ReadList");
}

@_Init() {
    PluginInit();
}

@_SetCell() {
    enum {Arg_iCell = 1}

    API__CheckPluginInitedP(false);
    API__CheckCurrentParamsP(false);

    return TrieSetCell(g_tCurrentParams, g_sCurrentParamName, get_param(Arg_iCell));
}

@_SetString() {
    enum {Arg_sString = 1}

    API__CheckPluginInitedP(0);
    API__CheckCurrentParamsP(0);

    static sString[PARAM_LONG_STRING_MAX_LEN];
    get_string(Arg_sString, sString, charsmax(sString));

    return TrieSetString(g_tCurrentParams, g_sCurrentParamName, sString);
}

T_ParamType:@_ParamType_Register() {
    enum {Arg_sParamTypeName = 1}

    API__CheckPluginInitedP(Invalid_ParamType);

    new sParamTypeName[PARAM_TYPE_NAME_MAX_LEN];
    get_string(Arg_sParamTypeName, sParamTypeName, charsmax(sParamTypeName));

    return ParamType_Construct(sParamTypeName);
}

@_ParamType_SetReadCallback(const PluginId) {
    enum {Arg_iParamType = 1, Arg_sCallback}

    API__CheckPluginInited();

    new T_ParamType:iParamType = T_ParamType:get_param(Arg_iParamType);
    new sCallback[PARAM_TYPE_READ_CALLBACK_MAX_LEN];
    get_string(Arg_sCallback, sCallback, charsmax(sCallback));
    
    ParamType_SetReadCallback(iParamType, sCallback, PluginId);
}

T_Param:@_Param_Construct() {
    enum {Arg_sKey = 1, Arg_sParamTypeName, Arg_bRequired}

    API__CheckPluginInitedP(Invalid_Param);

    new sKey[PARAM_KEY_MAX_LEN];
    get_string(Arg_sKey, sKey, charsmax(sKey));

    new sParamTypeName[PARAM_TYPE_NAME_MAX_LEN + PARAM_TYPE_TAG_MAX_LEN + 1];
    get_string(Arg_sParamTypeName, sParamTypeName, charsmax(sParamTypeName));
    
    new bool:bRequired = !!get_param(Arg_bRequired);


    new sParamType[PARAM_TYPE_NAME_MAX_LEN], sParamTag[PARAM_TYPE_TAG_MAX_LEN];
    ParamType_GetTypeAndTag(sParamTypeName,
        sParamType, charsmax(sParamType),
        sParamTag, charsmax(sParamTag)
    );
    new T_ParamType:iParamType = ParamType_Find(sParamType, .bOrFail = true);

    return Param_Construct(sKey, iParamType, bRequired, sParamTag);
}

// native Trie:ParamsController_Param_ReadList(const Array:aParams, const JSON:jObj, &Trie:tAppend = Invalid_Trie, &E_ParamsReadErrorType:iErrType, sErrParamName[], const iErrParamNameLen = 0);
Trie:@_Param_ReadList() {
    enum {Arg_aParams = 1, Arg_jObj, Arg_tAppend, Arg_iErrType, Arg_sErrParamName, Arg_iErrParamNameLen}

    API__CheckPluginInitedP(Invalid_Trie);

    new Array:aParams = Array:get_param(Arg_aParams);
    new JSON:jObj = JSON:get_param(Arg_jObj);
    new Trie:tAppend = Trie:get_param_byref(Arg_tAppend);
    new E_ParamsReadErrorType:iErrType = E_ParamsReadErrorType:get_param_byref(Arg_iErrType);
    new sErrParamName[PARAM_KEY_MAX_LEN];
    get_string(Arg_sErrParamName, sErrParamName, charsmax(sErrParamName));
    new iErrParamNameLen = get_param(Arg_iErrParamNameLen);

    if (!json_is_object(jObj)) {
        log_error(0, "[ERROR] Param jObj must be an object. (%d)", jObj);
        return Invalid_Trie;
    }

    new Trie:tRet = Param_ReadList(aParams, jObj, tAppend, iErrType, sErrParamName, iErrParamNameLen);

    set_param_byref(Arg_iErrType, _:iErrType);
    set_string(Arg_sErrParamName, sErrParamName, iErrParamNameLen);

    return tRet;
}
