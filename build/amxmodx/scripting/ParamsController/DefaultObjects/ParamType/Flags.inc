#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Flags_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_FLAGS_NAME, "@DefaultObjects_ParamType_Flags_OnRead");
}

bool:@DefaultObjects_ParamType_Flags_OnRead(const JSON:valueJson) {
    static str[32];
    json_get_string(valueJson, str, charsmax(str));
    
    return ParamsController_SetCell(read_flags(str));
}
