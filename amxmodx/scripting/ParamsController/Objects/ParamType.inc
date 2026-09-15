#if defined __params_controller_src_param_type_included
    #endinput
#endif
#define __params_controller_src_param_type_included

#include <amxmodx>
#include <ParamsController>

enum _:S_ParamType {
    ParamType_Name[PARAM_TYPE_NAME_MAX_LEN],
    ParamType_ReadCallback,
}

static Array:ParamTypes = Invalid_Array;
static Trie:ParamTypesMap = Invalid_Trie;

ParamType_Init() {
    if (ParamTypes == Invalid_Array) {
        ParamTypes = ArrayCreate(S_ParamType, 1);
    }

    if (ParamTypesMap == Invalid_Trie) {
        ParamTypesMap = TrieCreate();
    }
}

T_ParamType:ParamType_Construct(const name[]) {
    new typeObject[S_ParamType];

    copy(typeObject[ParamType_Name], charsmax(typeObject[ParamType_Name]), name);
    typeObject[ParamType_ReadCallback] = INVALID_HANDLE;

    new T_ParamType:type = T_ParamType:ArrayPushArray(ParamTypes, typeObject);
    TrieSetCell(ParamTypesMap, name, type);

    return type;
}

T_ParamType:ParamType_Find(const name[], const bool:orFail = false) {
    new T_ParamType:type = Invalid_ParamType;
    TrieGetCell(ParamTypesMap, name, type);

    if (orFail && type == Invalid_ParamType) {
        abort(AMX_ERR_PARAMS, "Param type '%s' not found.", name);
    }

    return type;
}

static ParamType_Get(const T_ParamType:type, typeObject[S_ParamType]) {
    ArrayGetArray(ParamTypes, _:type, typeObject);
}

static ParamType_Update(const typeObject[S_ParamType]) {
    new T_ParamType:type = ParamType_Find(typeObject[ParamType_Name], .orFail = true);
    ArraySetArray(ParamTypes, _:type, typeObject);
}

ParamType_iGetName(const T_ParamType:type) {
    new typeObject[S_ParamType];
    ParamType_Get(type, typeObject);
    
    return typeObject[ParamType_Name];
}

ParamType_GetName(const T_ParamType:type, out[], const outLen) {
    new typeObject[S_ParamType];
    ParamType_Get(type, typeObject);
    
    return copy(out, outLen, typeObject[ParamType_Name]);
}

ParamType_SetReadCallback(const T_ParamType:type, const functionName[], const pluginIndex) {
    new typeObject[S_ParamType];
    ParamType_Get(type, typeObject);

    // bool:(const JSON:valueJson, const Trie:p, const key[], const tag[])
    typeObject[ParamType_ReadCallback] = CreateOneForward(pluginIndex, functionName, FP_CELL, FP_CELL, FP_STRING, FP_STRING);
    if (typeObject[ParamType_ReadCallback] == INVALID_HANDLE) {
        abort(AMX_ERR_GENERAL, "Can't create callback by '%s' func of #%d plugin.", functionName, pluginIndex);
        return;
    }
    
    ParamType_Update(typeObject);
}

enum _:S_CurrentParam {
    Trie:CurrentParam_Params,
    CurrentParam_Name[PARAM_KEY_MAX_LEN],
}
new Stack:CurrentParamsStack = Invalid_Stack;
new Trie:CurrentParams = Invalid_Trie;
new CurrentParamName[PARAM_KEY_MAX_LEN] = "";

static PushCurrentParam(const Trie:params, const paramName[]) {
    if (CurrentParamsStack == Invalid_Stack) {
        CurrentParamsStack = CreateStack(S_CurrentParam);
    }

    if (CurrentParams != Invalid_Trie) {
        new entry[S_CurrentParam];
        copy(entry[CurrentParam_Name], charsmax(entry[CurrentParam_Name]), CurrentParamName);
        entry[CurrentParam_Params] = CurrentParams;
        PushStackArray(CurrentParamsStack, entry);
    }

    CurrentParams = params;
    copy(CurrentParamName, charsmax(CurrentParamName), paramName);
}

static PopCurrentParam() {
    if (
        CurrentParamsStack != Invalid_Stack
        && !IsStackEmpty(CurrentParamsStack)
    ) {
        new entry[S_CurrentParam];
        PopStackArray(CurrentParamsStack, entry);
        copy(CurrentParamName, charsmax(CurrentParamName), entry[CurrentParam_Name]);
        CurrentParams = entry[CurrentParam_Params];
    } else {
        CurrentParams = Invalid_Trie;
        CurrentParamName[0] = 0;
    }
}

bool:ParamType_Read(
    const T_ParamType:type,
    const JSON:valueJson,
    const Trie:p,
    const key[],
    const tag[] = ""
) {
    new typeObject[S_ParamType];
    ParamType_Get(type, typeObject);

    new JSON:linkedJson;
    PCJson_HandleLinkedValue(valueJson, linkedJson);
    
    PushCurrentParam(p, key);
    new bool:result;
    ExecuteForward(typeObject[ParamType_ReadCallback], result, linkedJson, p, key, tag);
    PopCurrentParam();

    PCJson_FreeLinked(linkedJson, valueJson);

    return result;
}

ParamType_PrintListToConsole() {
    // TODO: Сделать красиво табличкой как в vipm :)

    server_print("Registered param types:");
    for (new i = 0, ii = ArraySize(ParamTypes); i < ii; ++i) {
        new typeObject[S_ParamType];
        ArrayGetArray(ParamTypes, i, typeObject);

        server_print(" - %s", typeObject[ParamType_Name]);
    }
}

ParamType_GetTypeAndTag(const str[], paramType[], const paramTypeLen, paramTag[], const paramTagLen) {
    return strtok2(str, paramType, paramTypeLen, paramTag, paramTagLen, PARAMS_CONTROLLER_TYPE_TAG_DELIMITER_CHAR);
}
