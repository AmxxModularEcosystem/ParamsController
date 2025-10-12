#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Time_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_TIME_NAME, "@DefaultObjects_ParamType_Time_OnRead");
}

bool:@DefaultObjects_ParamType_Time_OnRead(const JSON:valueJson) {
    switch (json_get_type(valueJson)) {
        case JSONNumber: {
            PCJson_LogForFile(valueJson, "WARNING", "Not recommended time format. Use string (^"HH[:MM[:SS]]^") instead numeric.");
            return ParamsController_SetCell(json_get_number(valueJson));
        }
        case JSONString: {
            static str[PARAM_SHORT_STRING_MAX_LEN];
            json_get_string(valueJson, str, charsmax(str));
            
            new parts[3][3];
            if (explode_string(str, ":", parts, 3, 3) > 3) {
                PCJson_LogForFile(valueJson, "WARNING", "Invalid time format. Use ^"HH[:MM[:SS]]^".");
                return false;
            }

            new h = clamp(str_to_num(parts[0]), 0, 23) * 60 * 60;
            new m = clamp(str_to_num(parts[1]), 0, 59) * 60;
            new s = clamp(str_to_num(parts[2]), 0, 59);
            
            return ParamsController_SetCell(h + m + s);
        }
    }
    
    PCJson_LogForFile(valueJson, "WARNING", "Invalid time value. Expected string (^"HH[:MM[:SS]]^").");
    return false;
}
