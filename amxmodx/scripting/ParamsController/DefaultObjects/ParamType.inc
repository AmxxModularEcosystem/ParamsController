#include <amxmodx>
#include <json>
#include <ParamsController>

RegisterDefaultParamTypes() {
    // Идея: Либо отдельными типами, либо встроить в ядро поддержку чтения типов массивами
    // e.g. тип "String[]" будет читать массив обычных строк

    // UPD: Общая логика не покатит, т.к. сеттеры рассчитаны именно на Trie,
    // а оборачивать каждый элемент массива в Trie не хочется
    
    // Примитивные типы
    ParamsController_RegSimpleType(DEFAULT_PARAMS_BOOL_NAME, "@OnBooleanRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_INT_NAME, "@OnIntegerRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_FLOAT_NAME, "@OnFloatRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_STR_NAME, "@OnStringRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_SHORT_STR_NAME, "@OnShortStringRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_LONG_STR_NAME, "@OnLongStringRead");

    // Сложные типы
    ParamsController_RegSimpleType(DEFAULT_PARAMS_RGB_NAME, "@OnRGBRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_MODEL_NAME, "@OnModelRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_SOUND_NAME, "@OnSoundRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_RESOURCE_NAME, "@OnResourceRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_FILE_NAME, "@OnFileRead");
    ParamsController_RegSimpleType(DEFAULT_PARAMS_DIR_NAME, "@OnDirRead");
}

@OnBooleanRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_bool(jValue));
    switch (json_get_type(jValue)) {
        case JSONNumber: {
            return ParamsController_SetCell(json_get_number(jValue) != 0);
        }
        case JSONString: {
            static str[16];
            json_get_string(jValue, str, charsmax(str));

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
                return ParamsController_SetCell(true);
            }
            
            PCJson_LogForFile(valueJson, "WARNING", "Invalid string boolean value (%s).", str);
            return false;
        }
        case JSONBoolean: {
            return ParamsController_SetCell(json_get_bool(jValue));
        }
        default: {
            PCJson_LogForFile(valueJson, "WARNING", "Invalid boolean value.");
            return false;
        }
    }
}

@OnIntegerRead(const JSON:jValue) {
    switch (json_get_type(jValue)) {
        case JSONNumber: {
            return ParamsController_SetCell(json_get_number(jValue));
        }
        case JSONString: {
            new str[16];
            json_get_string(jValue, str, charsmax(str));

            return ParamsController_SetCell(str_to_num(str));
        }
        case JSONBoolean: {
            return ParamsController_SetCell(json_get_bool(jValue) ? 1 : 0);
        }
        default: {
            PCJson_LogForFile(valueJson, "WARNING", "Invalid integer value.");
            return false;
        }
    }
}

@OnFloatRead(const JSON:jValue) {
    switch (json_get_type(jValue)) {
        case JSONNumber: {
            return ParamsController_SetCell(json_get_real(jValue));
        }
        case JSONString: {
            new str[16];
            json_get_string(jValue, str, charsmax(str));

            return ParamsController_SetCell(str_to_float(str));
        }
        case JSONBoolean: {
            return ParamsController_SetCell(json_get_bool(jValue) ? 1 : 0);
        }
        default: {
            PCJson_LogForFile(valueJson, "WARNING", "Invalid float (real) value.");
            return false;
        }
    }
}

@OnStringRead(const JSON:jValue) {
    static sValue[PARAM_VALUE_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

@OnShortStringRead(const JSON:jValue) {
    static sValue[PARAM_SHORT_STRING_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

@OnLongStringRead(const JSON:jValue) {
    static sValue[PARAM_LONG_STRING_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

bool:@OnRGBRead(const JSON:valueJson, const Trie:p, const key[]) {
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

@OnModelRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    if (!file_exists(path)) {
        PCJson_LogForFile(valueJson, "WARNING", "Model '%s' not found.", path);
        return false;
    }

    new index = precache_model(path);
    if (equali(tag, "index")) {
        return ParamsController_SetCell(index);
    }

    return ParamsController_SetString(path);
}

@OnSoundRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    static fullPath[PLATFORM_MAX_PATH];
    formatex(fullPath, charsmax(fullPath), "sound/%s", path);

    if (!file_exists(fullPath)) {
        PCJson_LogForFile(valueJson, "WARNING", "Sound '%s' not found.", fullPath);
        return false;
    }

    new index = precache_sound(path);
    if (equali(tag, "index")) {
        return ParamsController_SetCell(index);
    }

    return ParamsController_SetString(path);
}

@OnResourceRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    if (!file_exists(path)) {
        PCJson_LogForFile(valueJson, "WARNING", "Precaching file '%s' not found.", path);
        return false;
    }

    new index = precache_generic(path);
    if (equali(tag, "index")) {
        return ParamsController_SetCell(index);
    }

    return ParamsController_SetString(path);
}

@OnFileRead(const JSON:valueJson, const Trie:p, const key[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path))

    if (!file_exists(path)) {
        PCJson_LogForFile(valueJson, "WARNING", "File '%s' not found.", path);
        return false;
    }

    return ParamsController_SetString(path);
}

@OnDirRead(const JSON:valueJson, const Trie:p, const key[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    if (!dir_exists(path)) {
        PCJson_LogForFile(valueJson, "WARNING", "Dir '%s' not found.", path);
        return false;
    }

    return ParamsController_SetString(path);
}
