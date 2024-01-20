#include <amxmodx>
#include "ParamsController/Natives"
#include "ParamsController/Core/ParamType"
#include "ParamsController/Core/Param"

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

    return TrieSetCell(g_tCurrentParams, g_sCurrentParamName, get_param(Arg_iCell));
}

@_SetString() {
    enum {Arg_sString = 1}

    static sString[PARAM_LONG_STRING_MAX_LEN];
    get_string(Arg_sString, sString, charsmax(sString));

    return TrieSetString(g_tCurrentParams, g_sCurrentParamName, sString);
}

T_ParamType:@_ParamType_Register() {
    enum {Arg_sParamTypeName = 1}

    new sParamTypeName[PARAM_TYPE_NAME_MAX_LEN];
    get_string(Arg_sParamTypeName, sParamTypeName, charsmax(sParamTypeName));

    return ParamType_Construct(sParamTypeName);
}

@_ParamType_SetReadCallback(const PluginId) {
    enum {Arg_iParamType = 1, Arg_sCallback}

    new T_ParamType:iParamType = T_ParamType:get_param(Arg_iParamType);
    new sCallback[PARAM_TYPE_READ_CALLBACK_MAX_LEN];
    get_string(Arg_sCallback, sCallback, charsmax(sCallback));
    
    ParamType_SetReadCallback(iParamType, sCallback, PluginId);
}

T_Param:@_Param_Construct() {
    enum {Arg_sKey = 1, Arg_sParamTypeName, Arg_bRequired}

    new sKey[PARAM_KEY_MAX_LEN];
    get_string(Arg_sKey, sKey, charsmax(sKey));

    new sParamTypeName[PARAM_TYPE_NAME_MAX_LEN];
    get_string(Arg_sParamTypeName, sParamTypeName, charsmax(sParamTypeName));

    new i = strfind(sParamTypeName, PARAMS_CONTROLLER_TYPE_TAG_DELIMITER);
    if (i > 0) {
        sParamTypeName[i] = 0; // EOS
    }
    new T_ParamType:iParamType = ParamType_Find(sParamTypeName, .bOrFail=true);
    
    new bool:bRequired = !!get_param(Arg_bRequired);

    new sParamTag[PARAM_TYPE_TAG_MAX_LEN];
    if (i > 0) {
        copy(sParamTag, charsmax(sParamTag), sParamTypeName[i + 1]);
    } else {
        sParamTag[0] = 0;
    }

    return Param_Construct(sKey, iParamType, bRequired, sParamTag);
}

// native Trie:ParamsController_Param_ReadList(const Array:aParams, const JSON:jObj, &Trie:tAppend = Invalid_Trie, &E_ParamsReadErrorType:iErrType, sErrParamName[], const iErrParamNameLen = 0);
Trie:@_Param_ReadList() {
    enum {Arg_aParams = 1, Arg_jObj, Arg_tAppend, Arg_iErrType, Arg_sErrParamName, Arg_iErrParamNameLen}

    new Array:aParams = Array:get_param(Arg_aParams);
    new JSON:jObj = JSON:get_param(Arg_jObj);
    new Trie:tAppend = Trie:get_param_byref(Arg_tAppend);
    new E_ParamsReadErrorType:iErrType = E_ParamsReadErrorType:get_param_byref(Arg_iErrType);
    new sErrParamName[PARAM_KEY_MAX_LEN];
    get_string(Arg_sErrParamName, sErrParamName, charsmax(sErrParamName));
    new iErrParamNameLen = get_param(Arg_iErrParamNameLen);

    new Trie:tRet = Param_ReadList(aParams, jObj, tAppend, iErrType, sErrParamName, iErrParamNameLen);

    set_param_byref(Arg_iErrType, _:iErrType);
    set_string(Arg_sErrParamName, sErrParamName, iErrParamNameLen);

    return tRet;
}
