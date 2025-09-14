#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Integer_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_INT_NAME, "@DefaultObjects_ParamType_Integer_OnRead");
}

bool:@DefaultObjects_ParamType_Integer_OnRead(const JSON:valueJson) {
    switch (json_get_type(valueJson)) {
        case JSONNumber: {
            return ParamsController_SetCell(json_get_number(valueJson));
        }
        case JSONString: {
            new str[16];
            json_get_string(valueJson, str, charsmax(str));

            return ParamsController_SetCell(str_to_num(str));
        }
        case JSONBoolean: {
            return ParamsController_SetCell(json_get_bool(valueJson) ? 1 : 0);
        }
    }
    
    PCJson_LogForFile(valueJson, "WARNING", "Invalid integer value.");
    return false;
}
