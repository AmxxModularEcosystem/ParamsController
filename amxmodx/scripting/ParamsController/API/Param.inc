#include <amxmodx>
#include "ParamsController/API/Utils"

API_Param_Register() {
    register_native("ParamsController_Param_Construct", "@API_Param_Construct");
    register_native("ParamsController_Param_ReadList", "@API_Param_ReadList");
    register_native("ParamsController_Param_Read", "@API_Param_Read");
    register_native("ParamsController_Param_Free", "@API_Param_Free");
}

T_Param:@API_Param_Construct() {
    enum {Arg_Key = 1, Arg_ParamTypeName, Arg_Required}

    API_CheckPluginInited();

    new key[PARAM_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));

    new paramTypeName[PARAM_TYPE_NAME_MAX_LEN + PARAM_TYPE_TAG_MAX_LEN + 1];
    get_string(Arg_ParamTypeName, paramTypeName, charsmax(paramTypeName));

    new bool:required = !!get_param(Arg_Required);

    new typeName[PARAM_TYPE_NAME_MAX_LEN], paramTag[PARAM_TYPE_TAG_MAX_LEN];
    ParamType_GetTypeAndTag(paramTypeName,
        typeName, charsmax(typeName),
        paramTag, charsmax(paramTag)
    );
    new T_ParamType:paramType = ParamType_Find(typeName, .orFail = true);

    return Param_Construct(key, paramType, required, paramTag);
}

// native Trie:ParamsController_Param_ReadList(const Array:params, const JSON:objectJson, &Trie:append = Invalid_Trie, &E_ParamsReadErrorType:errType, errParamName[], const errParamNameLen = 0);
Trie:@API_Param_ReadList() {
    enum {Arg_Params = 1, Arg_ObjectJson, Arg_Append, Arg_ErrType, Arg_ErrParamName, Arg_ErrParamNameLen}

    API_CheckPluginInited();

    new Array:params = Array:get_param(Arg_Params);
    new JSON:objectJson = JSON:get_param(Arg_ObjectJson);
    new Trie:append = Trie:get_param_byref(Arg_Append);
    new E_ParamsReadErrorType:errType = E_ParamsReadErrorType:get_param_byref(Arg_ErrType);
    new errParamName[PARAM_KEY_MAX_LEN];
    get_string(Arg_ErrParamName, errParamName, charsmax(errParamName));
    new errParamNameLen = get_param(Arg_ErrParamNameLen);

    if (!json_is_object(objectJson)) {
        log_error(AMX_ERR_PARAMS, "[ERROR] Param objectJson must be an object. (%d)", objectJson);
        return Invalid_Trie;
    }

    new Trie:ret = Param_ReadList(params, objectJson, append, errType, errParamName, errParamNameLen);

    set_param_byref(Arg_ErrType, _:errType);
    set_string(Arg_ErrParamName, errParamName, errParamNameLen);

    return ret;
}

bool:@API_Param_Read() {
    enum {Arg_Param = 1, Arg_ValueJson, Arg_Dest, Arg_OrFailKey}

    API_CheckPluginInited();

    new T_Param:param = T_Param:get_param(Arg_Param);
    new JSON:valueJson = JSON:get_param(Arg_ValueJson);
    new Trie:dest = Trie:get_param(Arg_Dest);
    new orFailKey[MAX_FMT_LENGTH];
    get_string(Arg_OrFailKey, orFailKey, charsmax(orFailKey));

    if (valueJson == Invalid_JSON) {
        // log_error(AMX_ERR_PARAMS, "[ERROR] Param valueJson must be a valid json handler. (%d)", valueJson);
        return false;
    }

    new bool:ret = Param_Read(param, valueJson, dest);

    if (orFailKey[0] != EOS && !ret) {
        PCJson_ErrorForFile(valueJson, "Field '%s' has invalid value for '%s' param type.", orFailKey, Param_iGetType(param));
        return false;
    }

    return ret;
}

T_Param:@API_Param_Free() {
    enum {Arg_Param = 1}

    new T_Param:param = T_Param:get_param_byref(Arg_Param);
    
    Param_Free(param);

    set_param_byref(Arg_Param, _:Invalid_Param);
    return Invalid_Param;
}