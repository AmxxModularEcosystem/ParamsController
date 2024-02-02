#if defined __params_controller_src_param_type_included
    #endinput
#endif
#define __params_controller_src_param_type_included

#include <amxmodx>
#include <ParamsController>
#include "ParamsController/ArrayMap"
#include "ParamsController/Utils"

enum _:S_ParamType {
    ParamType_Name[PARAM_TYPE_NAME_MAX_LEN],
    ParamType_ReadCallback,

    // Пока полноценные ивенты не нужны, мб потом переделаю
    // И вообще тут уже больше реализация метода, а не ивенты, ну да ладно)
    // T_Events:ParamType_Events,
}

static ArrayMap(g_amParamTypes);

ParamType_Init() {
    CallOnce();
    InitArrayMap(g_amParamTypes, S_ParamType, 1);
}

T_ParamType:ParamType_Construct(const sName[]) {
    new ParamType[S_ParamType];

    copy(ParamType[ParamType_Name], charsmax(ParamType[ParamType_Name]), sName);
    ParamType[ParamType_ReadCallback] = INVALID_HANDLE;
    // ParamType[ParamType_Events] = Events_Init(E_ParamTypeEvent);

    return T_ParamType:ArrayMapPushArray(g_amParamTypes, ParamType, ParamType[ParamType_Name]);
}

T_ParamType:ParamType_Find(const sName[], const bool:bOrFail = false) {
    new T_ParamType:iParamType = T_ParamType:ArrayMapGetIndex(g_amParamTypes, sName);

    if (bOrFail && iParamType == Invalid_ParamType) {
        ThrowError("Param type '%s' not found.", sName);
    }

    return iParamType;
}

static ParamType__Get(const T_ParamType:iParamType, ParamType[S_ParamType]) {
    ArrayMapGetiArray(g_amParamTypes, iParamType, ParamType);
}

static ParamType__Update(const ParamType[S_ParamType]) {
    ArrayMapSetArray(g_amParamTypes, ParamType[ParamType_Name], ParamType);
}

ParamType_iGetName(const T_ParamType:iParamType) {
    new ParamType[S_ParamType];
    ParamType__Get(iParamType, ParamType);
    
    return ParamType[ParamType_Name];
}

ParamType_SetReadCallback(const T_ParamType:iParamType, const sCallback[], const PluginId) {
    new ParamType[S_ParamType];
    ParamType__Get(iParamType, ParamType);

    // bool:(const JSON:jValue, const Trie:tParams, const sParamKey[])
    ParamType[ParamType_ReadCallback] = CreateOneForward(PluginId, sCallback, FP_CELL, FP_CELL, FP_STRING, FP_STRING);
    if (ParamType[ParamType_ReadCallback] == INVALID_HANDLE) {
        ThrowError("Can't create callback by '%s' func of #%d plugin.", sCallback, PluginId);
        return;
    }
    
    ParamType__Update(ParamType);
}

enum _:S_CurrentParam {
    Trie:CurrentParam_Params,
    CurrentParam_Name[PARAM_KEY_MAX_LEN],
}
new Stack:g_stCurrentParams = Invalid_Stack;
new Trie:g_tCurrentParams = Invalid_Trie;
new g_sCurrentParamName[PARAM_KEY_MAX_LEN] = "";

static PushCurrentParam(const Trie:tParams, const sParamName[]) {
    if (g_stCurrentParams == Invalid_Stack) {
        g_stCurrentParams = CreateStack(S_CurrentParam);
    }

    if (g_tCurrentParams != Invalid_Trie) {
        new CurrentParam[S_CurrentParam];
        copy(CurrentParam[CurrentParam_Name], charsmax(CurrentParam[CurrentParam_Name]), g_sCurrentParamName);
        CurrentParam[CurrentParam_Params] = g_tCurrentParams;
        PushStackArray(g_stCurrentParams, CurrentParam);
    }

    g_tCurrentParams = tParams;
    copy(g_sCurrentParamName, charsmax(g_sCurrentParamName), sParamName);
}

static PopCurrentParam() {
    if (
        g_stCurrentParams != Invalid_Stack
        && !IsStackEmpty(g_stCurrentParams)
    ) {
        new CurrentParam[S_CurrentParam];
        PopStackArray(g_stCurrentParams, CurrentParam);
        copy(g_sCurrentParamName, charsmax(g_sCurrentParamName), CurrentParam[CurrentParam_Name]);
        g_tCurrentParams = CurrentParam[CurrentParam_Params];
    } else {
        g_tCurrentParams = Invalid_Trie;
        g_sCurrentParamName[0] = 0;
    }

}

bool:ParamType_Read(
    const T_ParamType:iParamType,
    const JSON:jValue,
    const Trie:tParams,
    const sParamKey[],
    const sParamTag[] = ""
) {
    new ParamType[S_ParamType];
    ParamType__Get(iParamType, ParamType);
    
    PushCurrentParam(tParams, sParamKey);
    new bool:bReturned;
    ExecuteForward(ParamType[ParamType_ReadCallback], bReturned, jValue, tParams, sParamKey, sParamTag);
    PopCurrentParam();

    return bReturned;
}

ParamType_PrintListToConsole() {
    // TODO: Сделать красиво табличкой как в vipm :)

    server_print("Registered param types:");
    ArrayMapForeachArray (g_amParamTypes: i => ParamType[S_ParamType]) {
        server_print(" - %s", ParamType[ParamType_Name]);
    }
}

ParamType_GetTypeAndTag(const str[], sParamType[], const iParamTypeLen, sParamTag[], const iParamTagLen) {
    return strtok2(str, sParamType, iParamTypeLen, sParamTag, iParamTagLen, PARAMS_CONTROLLER_TYPE_TAG_DELIMITER_CHAR);
}
