#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_File_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_FILE_NAME, "@DefaultObjects_ParamType_File_OnRead");
}

bool:@DefaultObjects_ParamType_File_OnRead(const JSON:valueJson, const Trie:p, const key[]) {
    static path[PLATFORM_MAX_PATH];
    json_get_string(valueJson, path, charsmax(path))

    if (!file_exists(path)) {
        PCJson_LogForFile(valueJson, "WARNING", "File '%s' not found.", path);
        return false;
    }

    return ParamsController_SetString(path);
}
