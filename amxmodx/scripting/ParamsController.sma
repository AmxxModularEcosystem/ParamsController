#include <amxmodx>
#include <ParamsController>

#include "ParamsController/Forwards"
#include "ParamsController/Utils"

#include "ParamsController/Objects/Param"
#include "ParamsController/DefaultObjects/ParamType"

public stock const PluginName[] = "Params Controller";
public stock const PluginVersion[] = PARAMS_CONTROLLER_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://github.com/AmxxModularEcosystem/ParamsController";

static bool:PluginInited = false;

public plugin_precache() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(PARAMS_CONTROLLER_LIBRARY);
    CreateConstCvar(PARAMS_CONTROLLER_VERSION_CVAR_NAME, PluginVersion);
}

PluginInit() {
    if (PluginInited) {
        return;
    }
    PluginInited = true;

    Forwards_Init();
    Param_Init();
    RegisterDefaultParamTypes();

    // Тут регать типы
    Forwards_RegAndCall("ParamsController_OnRegisterTypes", ET_IGNORE);

    register_srvcmd("paras_controller_types", "@SrvCmd_Types");

    // После этого можно юзать типы
    Forwards_RegAndCall("ParamsController_OnInited", ET_IGNORE);
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

public plugin_natives() {
    API_General_Register();
    API_Param_Register();
    API_ParamType_Register();
}
