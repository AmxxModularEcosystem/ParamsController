#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_String_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_SHORT_STR_NAME, "@DefaultObjects_ParamType_ShortString_OnRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_STR_NAME, "@DefaultObjects_ParamType_String_OnRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_LONG_STR_NAME, "@DefaultObjects_ParamType_LongString_OnRead");
}

bool:@DefaultObjects_ParamType_ShortString_OnRead(const JSON:valueJson) {
    static str[PARAM_SHORT_STRING_MAX_LEN];
    json_get_string(valueJson, str, charsmax(str));

    return ParamsController_SetString(str);
}

bool:@DefaultObjects_ParamType_String_OnRead(const JSON:valueJson) {
    static str[PARAM_VALUE_MAX_LEN];
    json_get_string(valueJson, str, charsmax(str));

    return ParamsController_SetString(str);
}

bool:@DefaultObjects_ParamType_LongString_OnRead(const JSON:valueJson) {
    static str[PARAM_LONG_STRING_MAX_LEN];
    json_get_string(valueJson, str, charsmax(str));

    return ParamsController_SetString(str);
}
