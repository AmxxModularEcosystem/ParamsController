#include <amxmodx>
#include <json>
#include <ParamsController>

RegisterDefaultParamTypes() {
    // Идея: Либо отдельными типами, либо встроить в ядро поддержку чтения типов массивами
    // e.g. тип "String[]" будет читать массив обычных строк

    // UPD: Общая логика не покатит, т.к. сеттеры рассчитаны именно на Trie,
    // а оборачивать каждый элемент массива в Trie не хочется
    
    // Примитивные типы
    ParamsController_RegSimpleType("Boolean", "@OnBooleanRead");
    ParamsController_RegSimpleType("Integer", "@OnIntegerRead");
    ParamsController_RegSimpleType("Float", "@OnFloatRead");
    ParamsController_RegSimpleType("String", "@OnStringRead");
    ParamsController_RegSimpleType("ShortString", "@OnShortStringRead");
    ParamsController_RegSimpleType("LongString", "@OnLongStringRead");

    // Сложные типы
    ParamsController_RegSimpleType("RGB", "@OnRGBRead");
    ParamsController_RegSimpleType("Model", "@OnModelRead");
    ParamsController_RegSimpleType("Sound", "@OnSoundRead");
    ParamsController_RegSimpleType("Resource", "@OnResourceRead");
    ParamsController_RegSimpleType("File", "@OnFileRead");
    ParamsController_RegSimpleType("Dir", "@OnDirRead");
}

@OnModelRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
    new path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    if (!file_exists(path)) {
        log_amx("[WARNING] Model '%s' not found.", path);
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
    formatex(fullPath, "sound/%s", path);

    if (!file_exists(fullPath)) {
        log_amx("[WARNING] Sound '%s' not found.", fullPath);
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
        log_amx("[WARNING] Precaching file '%s' not found.", path);
        return false;
    }

    new index = precache_generic(path);
    if (equali(tag, "index")) {
        return ParamsController_SetCell(index);
    }

    return ParamsController_SetString(path);
}

@OnFileRead(const JSON:valueJson, const Trie:p
    json_get_string(valueJson, path, charsmax(path)), const key[]) {
    static path[PLATFORM_MAX_PATH];;

    if (!file_exists(path)) {
        log_amx("[WARNING] File '%s' not found.", path);
        return false;
    }

    return ParamsController_SetString(path);
}

@OnDirRead(const JSON:valueJson, const Trie:p, const key[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    if (!dir_exists(path)) {
        log_amx("[WARNING] Dir '%s' not found.", path);
        return false;
    }

    return ParamsController_SetString(path);
}

@OnBooleanRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_bool(jValue));
}

@OnIntegerRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_number(jValue));
}

@OnFloatRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_real(jValue));
}

@OnStringRead(const JSON:jValue) {
    new sValue[PARAM_VALUE_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

@OnShortStringRead(const JSON:jValue) {
    new sValue[PARAM_SHORT_STRING_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

@OnLongStringRead(const JSON:jValue) {
    new sValue[PARAM_LONG_STRING_MAX_LEN];
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
            log_amx("[WARNING] RGB value must be an array, object or string. String format is 'RRR,GGG,BBB' or 'RRR GGG BBB'.");
            return false;
        }
    } else {
        log_amx("[WARNING] RGB value must be an array, object or string.");
        return false;
    }

    TrieSetArray(p, key, color, 3);
    return true;
}
