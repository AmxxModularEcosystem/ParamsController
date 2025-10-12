#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_TimeInterval_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_TIME_INTERVAL_NAME, "@DefaultObjects_ParamType_TimeInterval_OnRead");
}

bool:@DefaultObjects_ParamType_TimeInterval_OnRead(const JSON:valueJson) {
    switch (json_get_type(valueJson)) {
        case JSONNumber: {
            new num = json_get_number(valueJson);
            if (num < 0) {
                PCJson_LogForFile(valueJson, "WARNING", "Invalid numeric time interval value. Expected >= 0.");
                return false;
            }

            return ParamsController_SetCell(num);
        }
        case JSONString: {
            static str[PARAM_SHORT_STRING_MAX_LEN];
            json_get_string(valueJson, str, charsmax(str));
            
            // https://dev-cs.ru/threads/222/page-3#post-33411
            new t, k;
            for (new i; str[i] != EOS; i++) {
                switch (str[i]) {
                    case '0'..'9': {
                        t = (t * 10) + (str[i] - '0');
                    }
                    case 'i': {
                        k += t * 60;
                        t = 0;
                    }
                    case 'h': {
                        k += t * 60 * 60;
                        t = 0;
                    }
                    case 'd': {
                        k += t * 60 * 60 * 24;
                        t = 0;
                    }
                    case 'm': {
                        k += t * 60 * 60 * 24 * 30;
                        t = 0;
                    }
                    case 'y': {
                        k += t * 60 * 60 * 24 * 30 * 12;
                        t = 0;
                    }
                    default: {
                        break;
                    }
                }
            }
            
            return ParamsController_SetCell(k + t);
        }
    }
    
    PCJson_LogForFile(valueJson, "WARNING", "Invalid time value. Expected string (^"1y2m3d4h5i6^").");
    return false;
}
