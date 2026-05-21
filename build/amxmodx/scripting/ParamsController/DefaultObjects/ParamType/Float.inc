#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Float_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_FLOAT_NAME, "@DefaultObjects_ParamType_Float_OnRead");
}

bool:@DefaultObjects_ParamType_Float_OnRead(const JSON:valueJson) {
    switch (json_get_type(valueJson)) {
        case JSONNumber: {
            return ParamsController_SetCell(json_get_real(valueJson));
        }
        case JSONString: {
            new str[16];
            json_get_string(valueJson, str, charsmax(str));

            return ParamsController_SetCell(str_to_float(str));
        }
        case JSONBoolean: {
            return ParamsController_SetCell(json_get_bool(valueJson) ? 1 : 0);
        }
    }
    
    PCJson_LogForFile(valueJson, "WARNING", "Invalid float (real) value.");
    return false;
}
