#include <amxmodx>
#include <ParamsController>
#include "ParamsController/Forwards"
#include "ParamsController/Utils"
#include "ParamsController/Core/ParamType"
#include "ParamsController/Core/Param"
#include "ParamsController/Core/DefaultParamTypes"

public stock const PluginName[] = "Params Controller";
public stock const PluginVersion[] = _PARAMS_CONTROLLER_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "t.me/arkanaplugins";

static bool:g_bPluginInited = false;

public plugin_precache() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(PARAMS_CONTROLLER_LIBRARY);
    CreateConstCvar("params_controller_version", PluginVersion);
}

PluginInit() {
    CallOnce();
    g_bPluginInited = true;

    RegisterForwards();
    Param_Init();
    RegisterDefaultParamTypes();

    // Тут регать типы
    Forwards_RegAndCall("RegisterTypes", ET_IGNORE);

    register_srvcmd("paras_controller_types", "@SrvCmd_Types");

    // После этого можно юзать типы
    Forwards_RegAndCall("Inited", ET_IGNORE);
}

@SrvCmd_Types() {
    ParamType_PrintListToConsole();
}

RegisterForwards() {
    Forwards_Init("ParamsController");
}

bool:IsPluginInited() {
    return g_bPluginInited;
}

#include "ParamsController/Core/Natives"
