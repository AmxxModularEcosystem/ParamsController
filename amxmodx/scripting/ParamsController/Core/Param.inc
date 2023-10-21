#if defined __params_controller_src_param_included
    #endinput
#endif
#define __params_controller_src_param_included

#include <amxmodx>
#include <json>
#include <ParamsController>
#include "ParamsController/Core/ParamType"
#include "ParamsController/Utils"
#include "ParamsController/ArrayTrieUtils"

enum _:S_Param {
    Param_Key[PARAM_KEY_MAX_LEN],
    T_ParamType:Param_Type,
    bool:Param_Required,
}

static Array:g_aParams;

Param_Init() {
    CallOnce();
    ParamType_Init();
    g_aParams = ArrayCreate(S_Param, 1);
}

T_Param:Param_Construct(const sKey[], const T_ParamType:iType, const bool:bRequired = true) {
    new Param[S_Param];

    copy(Param[Param_Key], charsmax(Param[Param_Key]), sKey);
    Param[Param_Type] = iType;
    Param[Param_Required] = bRequired;

    return T_Param:ArrayPushArray(g_aParams, Param);
}

static Param__Get(const T_Param:iParam, Param[S_Param]) {
    ArrayGetArray(g_aParams, _:iParam, Param);
}

bool:Param_Read(const T_Param:iParam, const JSON:jValue, const Trie:tParams) {
    new Param[S_Param];
    Param__Get(iParam, Param);
    
    return ParamType_Read(Param[Param_Type], jValue, tParams, Param[Param_Key]);
}

Trie:Param_ReadList(
    const Array:aParams,
    const JSON:jObj,
    &Trie:tAppend = Invalid_Trie,
    &E_ParamsReadErrorType:iErrType = ParamsReadError_None,
    sErrParamName[] = "",
    const iErrParamNameLen = 0
) {
    if (tAppend == Invalid_Trie) {
        tAppend = TrieCreate();
    }

    for (new i = 0, ii = ArraySizeSafe(aParams); i < ii; ++i) {
        new T_Param:iParam = ArrayGetCell(aParams, i);

        new Param[S_Param];
        Param__Get(iParam, Param);

        if (iErrParamNameLen > 0) {
            copy(sErrParamName, iErrParamNameLen, Param[Param_Key]);
        }

        if (!json_object_has_value(jObj, Param[Param_Key])) {
            if (Param[Param_Required]) {
                iErrType = ParamsReadError_RequiredParamNotPresented;
                return tAppend;
            }

            continue;
        }

        new JSON:jValue = json_object_get_value(jObj, Param[Param_Key]);
        new bool:bRes = Param_Read(iParam, jValue, tAppend);
        json_free(jValue);

        if (!bRes) {
            iErrType = ParamsReadError_ParamValueIsInvalid;
            return tAppend;
        }

        if (!TrieKeyExists(tAppend, Param[Param_Key])) {
            ThrowError("Param type '%s' dont write value with key '%s' to trie", ParamType_iGetName(Param[Param_Type]), Param[Param_Key]);
        }
    }
    
    return tAppend;
}
