#include <amxmodx>
#include "ParamsController/API/Utils"
#include "ParamsController/Placeholders/Objects/PHGroup"

API_PH_Key_Register() {
    register_native("ParamsController_PH_RegisterGroupKey", "@API_PH_RegisterGroupKey");
    register_native("ParamsController_PH_SetGroupKeyCallback", "@API_PH_SetGroupKeyCallback");
    register_native("ParamsController_PH_Set", "@API_PH_Set");
    register_native("ParamsController_PH_SetInt", "@API_PH_SetInt");
    register_native("ParamsController_PH_SetFloat", "@API_PH_SetFloat");
}

@API_PH_RegisterGroupKey() {
    enum {Arg_Group = 1, Arg_Key}

    API_CheckPluginInited();

    new T_PHGroup:hGroup = T_PHGroup:get_param(Arg_Group);

    static sKey[PH_KEY_MAX_LEN];
    get_string(Arg_Key, sKey, charsmax(sKey));

    PHGroup_RegisterKey(hGroup, sKey);
}

@API_PH_SetGroupKeyCallback(const pluginIndex) {
    enum {Arg_Group = 1, Arg_Key, Arg_Callback}

    API_CheckPluginInited();

    new T_PHGroup:hGroup = T_PHGroup:get_param(Arg_Group);

    static sKey[PH_KEY_MAX_LEN];
    get_string(Arg_Key, sKey, charsmax(sKey));

    static sCallback[PH_CALLBACK_MAX_LEN];
    get_string(Arg_Callback, sCallback, charsmax(sCallback));

    PHGroup_SetKeyCallback(hGroup, sKey, sCallback, pluginIndex);
}

@API_PH_Set() {
    enum {Arg_Group = 1, Arg_Key, Arg_Value}

    API_CheckPluginInited();

    new T_PHGroup:hGroup = T_PHGroup:get_param(Arg_Group);

    static sKey[PH_KEY_MAX_LEN];
    get_string(Arg_Key, sKey, charsmax(sKey));

    static sValue[PH_VALUE_MAX_LEN];
    get_string(Arg_Value, sValue, charsmax(sValue));

    PHGroup_SetValue(hGroup, sKey, sValue);
}

@API_PH_SetInt() {
    enum {Arg_Group = 1, Arg_Key, Arg_Value}

    API_CheckPluginInited();

    new T_PHGroup:hGroup = T_PHGroup:get_param(Arg_Group);

    static sKey[PH_KEY_MAX_LEN];
    get_string(Arg_Key, sKey, charsmax(sKey));

    static sValue[PH_VALUE_MAX_LEN];
    num_to_str(get_param(Arg_Value), sValue, charsmax(sValue));

    PHGroup_SetValue(hGroup, sKey, sValue);
}

@API_PH_SetFloat() {
    enum {Arg_Group = 1, Arg_Key, Arg_Value}

    API_CheckPluginInited();

    new T_PHGroup:hGroup = T_PHGroup:get_param(Arg_Group);

    static sKey[PH_KEY_MAX_LEN];
    get_string(Arg_Key, sKey, charsmax(sKey));

    static sValue[PH_VALUE_MAX_LEN];
    format(sValue, charsmax(sValue), "%.2f", get_param_f(Arg_Value));

    PHGroup_SetValue(hGroup, sKey, sValue);
}
