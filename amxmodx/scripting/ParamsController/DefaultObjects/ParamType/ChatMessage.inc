#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_ChatMessage_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_CHAT_MESSAGE_NAME, "@DefaultObjects_ParamType_ChatMessage_OnRead");
}

bool:@DefaultObjects_ParamType_ChatMessage_OnRead(const JSON:valueJson) {
    static str[PARAM_CHAT_MESSAGE_MAX_LEN];
    json_get_string(valueJson, str, charsmax(str));
    
    replace_all(str, charsmax(str), "^^1", "^1");
    replace_all(str, charsmax(str), "^^3", "^3");
    replace_all(str, charsmax(str), "^^4", "^4");

    return ParamsController_SetString(str);
}
