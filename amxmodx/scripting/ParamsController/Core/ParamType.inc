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

new Trie:g_tCurrentParams = Invalid_Trie;
new g_sCurrentParamName[PARAM_KEY_MAX_LEN] = "";

static SetCurrentParam(const Trie:tParams, const sParamName[]) {
    g_tCurrentParams = tParams;
    copy(g_sCurrentParamName, charsmax(g_sCurrentParamName), sParamName);
}

static ResetCurrentParam() {
    g_tCurrentParams = Invalid_Trie;
    g_sCurrentParamName[0] = 0;
}

bool:ParamType_Read(
    const T_ParamType:iParamType,
    const JSON:jValue,
    const Trie:tParams,
    const sParamKey[],
    const sParamTag[] = NULL_STRING
) {
    new ParamType[S_ParamType];
    ParamType__Get(iParamType, ParamType);
    
    SetCurrentParam(tParams, sParamKey);
    new bool:bReturned;
    ExecuteForward(ParamType[ParamType_ReadCallback], bReturned, jValue, tParams, sParamKey, sParamTag);
    ResetCurrentParam();

    return bReturned;
}

ParamType_PrintListToConsole() {
    // TODO: Сделать красиво табличкой как в vipm :)

    server_print("Registered param types:");
    ArrayMapForeachArray (g_amParamTypes: i => ParamType[S_ParamType]) {
        server_print(" - %s", ParamType[ParamType_Name]);
    }
}
