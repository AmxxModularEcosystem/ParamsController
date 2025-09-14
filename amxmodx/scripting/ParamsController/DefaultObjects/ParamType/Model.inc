#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_Model_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_MODEL_NAME, "@DefaultObjects_ParamType_Model_OnRead");
}

bool:@DefaultObjects_ParamType_Model_OnRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
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
