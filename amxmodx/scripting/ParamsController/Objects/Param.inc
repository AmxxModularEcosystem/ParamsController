#if defined __params_controller_src_param_included
    #endinput
#endif
#define __params_controller_src_param_included

#include <amxmodx>
#include <json>
#include <ParamsController>
#include "ParamsController/Objects/ParamType"

enum _:S_Param {
    Param_Key[PARAM_KEY_MAX_LEN],
    T_ParamType:Param_Type,
    bool:Param_Required,
    Param_Tag[PARAM_TYPE_TAG_MAX_LEN],
}

static Array:Params = Invalid_Array;
static Stack:FreeParamHandlers = Invalid_Stack;

Param_Init() {
    ParamType_Init();

    if (Params == Invalid_Array) {
        Params = ArrayCreate(S_Param, 1);
    }

    if (FreeParamHandlers == Invalid_Stack) {
        FreeParamHandlers = CreateStack();
    }
}

T_Param:Param_Construct(
    const key[],
    const T_ParamType:type,
    const bool:isRequired = true,
    const tag[] = ""
) {
    new paramObject[S_Param];

    if (type == Invalid_ParamType) {
        abort(AMX_ERR_PARAMS, "Invalid param type handler (%d).", type);
        return Invalid_Param;
    }

    copy(paramObject[Param_Key], charsmax(paramObject[Param_Key]), key);
    paramObject[Param_Type] = type;
    paramObject[Param_Required] = isRequired;
    copy(paramObject[Param_Tag], charsmax(paramObject[Param_Tag]), tag);

    if (IsStackEmpty(FreeParamHandlers)) {
        return T_Param:ArrayPushArray(Params, paramObject);
    } else {
        new T_Param:param;
        PopStackCell(FreeParamHandlers, param);
        ArraySetArray(Params, _:param, paramObject);
        return param;
    }
}

static Param_Get(const T_Param:param, paramObject[S_Param]) {
    ArrayGetArray(Params, _:param, paramObject);
    if (paramObject[Param_Type] == Invalid_ParamType) {
        abort(AMX_ERR_PARAMS, "Invalid param type handler (%d).", paramObject[Param_Type]);
    }
}

Param_iGetType(const T_Param:param) {
    new paramObject[S_Param];
    Param_Get(param, paramObject);

    new typeName[PARAM_TYPE_NAME_MAX_LEN];
    ParamType_GetName(paramObject[Param_Type], typeName, charsmax(typeName));

    return typeName;
}

T_Param:Param_Free(&T_Param:param) {
    ArraySetCell(Params, _:param, Invalid_ParamType, Param_Type);
    PushStackCell(FreeParamHandlers, param);
    return param = Invalid_Param;
}

bool:Param_Read(const T_Param:param, const JSON:valueJson, const Trie:p) {
    new paramObject[S_Param];
    Param_Get(param, paramObject);
    
    return ParamType_Read(paramObject[Param_Type], valueJson, p, paramObject[Param_Key], paramObject[Param_Tag]);
}

Trie:Param_ReadList(
    const Array:params,
    const JSON:objectJson,
    &Trie:p = Invalid_Trie,
    &E_ParamsReadErrorType:iErrType = ParamsReadError_None,
    sErrParamName[] = "",
    const iErrParamNameLen = 0
) {
    if (p == Invalid_Trie) {
        p = TrieCreate();
    }

    if (params == Invalid_Array) {
        return p;
    }

    for (new i = 0, ii = ArraySize(params); i < ii; ++i) {
        new T_Param:param = ArrayGetCell(params, i);

        new paramObject[S_Param];
        Param_Get(param, paramObject);

        if (iErrParamNameLen > 0) {
            copy(sErrParamName, iErrParamNameLen, paramObject[Param_Key]);
        }

        if (!json_object_has_value(objectJson, paramObject[Param_Key])) {
            if (paramObject[Param_Required]) {
                iErrType = ParamsReadError_RequiredParamNotPresented;
                return p;
            }

            continue;
        }

        new JSON:valueJson = json_object_get_value(objectJson, paramObject[Param_Key]);
        new bool:ret = Param_Read(param, valueJson, p);
        json_free(valueJson);

        if (!ret) {
            iErrType = ParamsReadError_ParamValueIsInvalid;
            return p;
        }

        if (!TrieKeyExists(p, paramObject[Param_Key])) {
            abort(AMX_ERR_GENERAL, "Param type '%s' dont write value with key '%s' to trie.", ParamType_iGetName(paramObject[Param_Type]), paramObject[Param_Key]);
        }
    }
    
    return p;
}
