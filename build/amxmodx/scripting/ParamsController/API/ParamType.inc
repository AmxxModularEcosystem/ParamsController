#include <amxmodx>
#include "ParamsController/API/Utils"

API_ParamType_Register() {
    register_native("ParamsController_ParamType_Register", "@API_ParamType_Register");
    register_native("ParamsController_ParamType_SetReadCallback", "@API_ParamType_SetReadCallback");
}

T_ParamType:@API_ParamType_Register() {
    enum {Arg_Name = 1}

    API_CheckPluginInited();

    new name[PARAM_TYPE_NAME_MAX_LEN];
    get_string(Arg_Name, name, charsmax(name));

    return ParamType_Construct(name);
}

@API_ParamType_SetReadCallback(const pluginIndex) {
    enum {Arg_Type = 1, Arg_FunctionName}

    API_CheckPluginInited();

    new T_ParamType:type = T_ParamType:get_param(Arg_Type);
    new functionName[PARAM_TYPE_READ_CALLBACK_MAX_LEN];
    get_string(Arg_FunctionName, functionName, charsmax(functionName));
    
    ParamType_SetReadCallback(type, functionName, pluginIndex);
}