#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Boolean_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_BOOL_NAME, "@DefaultObjects_ParamType_Boolean_OnRead");
}

bool:@DefaultObjects_ParamType_Boolean_OnRead(const JSON:valueJson) {
    switch (json_get_type(valueJson)) {
        case JSONNumber: {
            return ParamsController_SetCell(json_get_number(valueJson) != 0);
        }
        case JSONString: {
            static str[16];
            json_get_string(valueJson, str, charsmax(str));

            if (
                equali(str, "yes")
                || equali(str, "true")
                || equali(str, "on")
                || equali(str, "1")
                || equali(str, "y")
            ) {
                return ParamsController_SetCell(true);
            }

            if (
                equali(str, "no")
                || equali(str, "false")
                || equali(str, "off")
                || equali(str, "0")
                || equali(str, "n")
            ) {
                return ParamsController_SetCell(false);
            }
            
            PCJson_LogForFile(valueJson, "WARNING", "Invalid string boolean value (%s).", str);
            return false;
        }
        case JSONBoolean: {
            return ParamsController_SetCell(json_get_bool(valueJson));
        }
    }

    PCJson_LogForFile(valueJson, "WARNING", "Invalid boolean value.");
    return false;
}
