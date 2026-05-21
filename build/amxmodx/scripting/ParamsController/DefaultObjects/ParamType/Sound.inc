#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Sound_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_SOUND_NAME, "@DefaultObjects_ParamType_Sound_OnRead");
}

bool:@DefaultObjects_ParamType_Sound_OnRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
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
