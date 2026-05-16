#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_PlayerModel_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_PLAYER_MODEL_NAME, "@DefaultObjects_ParamType_PlayerModel_OnRead");
}

bool:@DefaultObjects_ParamType_PlayerModel_OnRead(const JSON:valueJson, const Trie:p, const key[], const tag[]) {
    static modelName[PARAM_SHORT_STRING_MAX_LEN];
    PCSingle_ShortString(valueJson, modelName, charsmax(modelName));

    static path[PLATFORM_MAX_PATH];
    formatex(path, charsmax(path), "models/player/%s/%s.mdl", modelName, modelName);

    if (!file_exists(path)) {
        return PCJson_LogForFile(valueJson, "WARNING", "Player model '%s' (%s) not found.", modelName, path);
    }

    precache_model(path);

    return ParamsController_SetString(path);
}
