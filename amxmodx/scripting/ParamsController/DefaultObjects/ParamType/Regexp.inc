#include <amxmodx>
#include <json>
#include <regex>
#include <ParamsController>

DefaultObjects_ParamType_Regexp_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_REGEXP_NAME, "@DefaultObjects_ParamType_Regexp_OnRead");
}

bool:@DefaultObjects_ParamType_Regexp_OnRead(const JSON:valueJson) {
    static pattern[PARAM_VALUE_MAX_LEN];
    json_get_string(valueJson, pattern, charsmax(pattern));

    static error[256];
    new ret;
    new Regex:re = regex_compile(pattern, ret, error, charsmax(error));

    if (re == REGEX_PATTERN_FAIL) {
        PCJson_LogForFile(valueJson, "WARNING", "Invalid regexp pattern '%s': %s.", pattern, error);
        return false;
    }

    return ParamsController_SetCell(re);
}
