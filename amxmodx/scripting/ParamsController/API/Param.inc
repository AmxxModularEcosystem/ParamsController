#include <amxmodx>
#include "ParamsController/API/Utils"

API_Param_Register() {
    register_native("ParamsController_Param_Construct", "@API_Param_Construct");
    register_native("ParamsController_Param_ReadList", "@API_Param_ReadList");
    register_native("ParamsController_Param_Free", "@API_Param_Free");
}

T_Param:@API_Param_Construct() {
    enum {Arg_sKey = 1, Arg_sParamTypeName, Arg_bRequired}

    API_CheckPluginInited();

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
    new T_ParamType:iParamType = ParamType_Find(sParamType, .orFail = true);

    return Param_Construct(sKey, iParamType, bRequired, sParamTag);
}

// native Trie:ParamsController_Param_ReadList(const Array:aParams, const JSON:jObj, &Trie:tAppend = Invalid_Trie, &E_ParamsReadErrorType:iErrType, sErrParamName[], const iErrParamNameLen = 0);
Trie:@API_Param_ReadList() {
    enum {Arg_aParams = 1, Arg_jObj, Arg_tAppend, Arg_iErrType, Arg_sErrParamName, Arg_iErrParamNameLen}

    API_CheckPluginInited();

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

T_Param:@API_Param_Free() {
    enum {Arg_Param = 1}

    new T_Param:param = T_Param:get_param_byref(Arg_Param);
    
    Param_Free(param);

    set_param_byref(Arg_Param, _:Invalid_Param);
    return Invalid_Param;
}