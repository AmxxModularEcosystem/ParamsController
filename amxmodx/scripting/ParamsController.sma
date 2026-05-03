#include <amxmodx>
#include <ParamsController>

#include "ParamsController/Forwards"

#include "ParamsController/Objects/Param"
#include "ParamsController/Placeholders/Objects/PHGroup"
#include "ParamsController/DefaultObjects/Registrar"
#include "ParamsController/DefaultObjects/PlaceholderRegistrar"

public stock const PluginName[] = "Params Controller";
public stock const PluginVersion[] = PARAMS_CONTROLLER_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://github.com/AmxxModularEcosystem/ParamsController";

static bool:PluginInited = false;

public plugin_precache() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(PARAMS_CONTROLLER_LIBRARY);
    PCCvar_Const(PARAMS_CONTROLLER_VERSION_CVAR_NAME, PluginVersion);
}

PluginInit() {
    if (PluginInited) {
        return;
    }
    PluginInited = true;

    Forwards_Init();
    Param_Init();
    PHGroup_Init();
    DefaultObjects_ParamType_Register();
    DefaultObjects_Placeholder_Register();

    // Тут регать типы параметров
    Forwards_RegAndCall("ParamsController_OnRegisterTypes", ET_IGNORE);

    // Тут регать группы и ключи плейсхолдеров
    Forwards_RegAndCall("PCPH_OnRegisterGroups", ET_IGNORE);

    register_srvcmd("params_controller_types", "@SrvCmd_Types");

    // После этого можно юзать типы и плейсхолдеры
    Forwards_RegAndCall("ParamsController_OnInited", ET_IGNORE);
}

public client_authorized(id, const sAuthID[]) {
    if (!IsPluginInited()) {
        return;
    }

    DefaultObjects_Placeholder_OnClientAuth(id, sAuthID);  // id → playerIndex внутри
}

@SrvCmd_Types() {
    ParamType_PrintListToConsole();
}

bool:IsPluginInited() {
    return PluginInited;
}

#include "ParamsController/API/General"
#include "ParamsController/API/Param"
#include "ParamsController/API/ParamType"
#include "ParamsController/Placeholders/API/General"
#include "ParamsController/Placeholders/API/Group"
#include "ParamsController/Placeholders/API/Key"

public plugin_natives() {
    API_General_Register();
    API_Param_Register();
    API_ParamType_Register();
    API_PH_General_Register();
    API_PH_Group_Register();
    API_PH_Key_Register();
}
