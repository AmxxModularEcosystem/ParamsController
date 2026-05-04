#include <amxmodx>
#include "ParamsController/API/Utils"
#include "ParamsController/Placeholders/Objects/PHGroup"

API_PH_Key_Register() {
    register_native("PCPH_Group_RegisterKey", "@API_PH_RegisterGroupKey");
    register_native("PCPH_Group_SetKeyCallback", "@API_PH_SetGroupKeyCallback");
    register_native("PCPH_SetForStr", "@API_PH_SetForStr");
    register_native("PCPH_SetIntForStr", "@API_PH_SetIntForStr");
    register_native("PCPH_SetFloatForStr", "@API_PH_SetFloatForStr");
}

@API_PH_RegisterGroupKey() {
    enum {Arg_Group = 1, Arg_Key}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    static key[PH_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));

    PHGroup_RegisterKey(group, key);
}

@API_PH_SetGroupKeyCallback(const pluginIndex) {
    enum {Arg_Group = 1, Arg_Key, Arg_Callback}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    static key[PH_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));

    static callback[PH_CALLBACK_MAX_LEN];
    get_string(Arg_Callback, callback, charsmax(callback));

    PHGroup_SetKeyCallback(group, key, callback, pluginIndex);
}

@API_PH_SetForStr() {
    enum {Arg_Group = 1, Arg_Key, Arg_ContextKey, Arg_Value}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    static key[PH_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));

    static contextKey[PH_CONTEXT_KEY_MAX_LEN];
    get_string(Arg_ContextKey, contextKey, charsmax(contextKey));

    static value[PH_VALUE_MAX_LEN];
    get_string(Arg_Value, value, charsmax(value));

    PHGroup_SetValue(group, key, contextKey, value);
}

@API_PH_SetIntForStr() {
    enum {Arg_Group = 1, Arg_Key, Arg_ContextKey, Arg_Value}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    static key[PH_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));

    static contextKey[PH_CONTEXT_KEY_MAX_LEN];
    get_string(Arg_ContextKey, contextKey, charsmax(contextKey));

    static value[PH_VALUE_MAX_LEN];
    num_to_str(get_param(Arg_Value), value, charsmax(value));

    PHGroup_SetValue(group, key, contextKey, value);
}

@API_PH_SetFloatForStr() {
    enum {Arg_Group = 1, Arg_Key, Arg_ContextKey, Arg_Value}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    static key[PH_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));

    static contextKey[PH_CONTEXT_KEY_MAX_LEN];
    get_string(Arg_ContextKey, contextKey, charsmax(contextKey));

    static value[PH_VALUE_MAX_LEN];
    format(value, charsmax(value), "%.2f", get_param_f(Arg_Value));

    PHGroup_SetValue(group, key, contextKey, value);
}
