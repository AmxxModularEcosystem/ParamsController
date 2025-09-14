#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Dir_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_DIR_NAME, "@DefaultObjects_ParamType_Dir_OnRead");
}

bool:@DefaultObjects_ParamType_Dir_OnRead(const JSON:valueJson, const Trie:p, const key[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path));

    if (!dir_exists(path)) {
        PCJson_LogForFile(valueJson, "WARNING", "Dir '%s' not found.", path);
        return false;
    }

    return ParamsController_SetString(path);
}
