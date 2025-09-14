#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_RGB_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_RGB_NAME, "@DefaultObjects_ParamType_RGB_OnRead");
}

bool:@DefaultObjects_ParamType_RGB_OnRead(const JSON:valueJson, const Trie:p, const key[]) {
    new color[3] = {0, ...};
    if (json_is_array(valueJson)) {
        for (new i = 0, ii = min(json_array_get_count(valueJson), 3); i < ii; ++i) {
            color[i] = json_array_get_number(valueJson, i);
        }
    } else if (json_is_object(valueJson)) {
        if (json_object_has_value(valueJson, "R")) {
            color[0] = json_object_get_number(valueJson, "R");
        } else if (json_object_has_value(valueJson, "Red")) {
            color[0] = json_object_get_number(valueJson, "Red");
        } else {
            color[0] = 0;
        }
        
        if (json_object_has_value(valueJson, "G")) {
            color[1] = json_object_get_number(valueJson, "G");
        } else if (json_object_has_value(valueJson, "Green")) {
            color[1] = json_object_get_number(valueJson, "Green");
        } else {
            color[1] = 0;
        }
        
        if (json_object_has_value(valueJson, "B")) {
            color[2] = json_object_get_number(valueJson, "B");
        } else if (json_object_has_value(valueJson, "Blue")) {
            color[2] = json_object_get_number(valueJson, "Blue");
        } else {
            color[2] = 0;
        }
    } else if (json_is_string(valueJson)) {
        static str[32], colorStr[3][8];
        json_get_string(valueJson, str, charsmax(str));

        if (contain(str, ",")) {
            explode_string(str, ",", colorStr, 3, charsmax(colorStr[]));
            for (new i = 0; i < 3; ++i) {
                color[i] = str_to_num(colorStr[i]);
            }
        } else if (contain(str, " ")) {
            explode_string(str, " ", colorStr, 3, charsmax(colorStr[]));
            for (new i = 0; i < 3; ++i) {
                color[i] = str_to_num(colorStr[i]);
            }
        } else {
            PCJson_LogForFile(valueJson, "WARNING", "RGB value must be an array, object or string. String format is 'RRR,GGG,BBB' or 'RRR GGG BBB'.");
            return false;
        }
    } else {
        PCJson_LogForFile(valueJson, "WARNING", "RGB value must be an array, object or string.");
        return false;
    }

    TrieSetArray(p, key, color, 3);
    return true;
}
