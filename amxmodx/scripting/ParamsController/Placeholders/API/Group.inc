#include <amxmodx>
#include "ParamsController/API/Utils"
#include "ParamsController/Placeholders/Objects/PHGroup"

API_PH_Group_Register() {
    register_native("PCPH_RegisterGroup", "@API_PH_RegisterGroup");
    register_native("PCPH_FindGroup", "@API_PH_FindGroup");
    register_native("PCPH_GetGlobalGroup", "@API_PH_GetGlobalGroup");
    register_native("PCPH_Group_PushContext", "@API_PH_PushContext");
    register_native("PCPH_Group_PopContext", "@API_PH_PopContext");
}

T_PHGroup:@API_PH_RegisterGroup() {
    enum {Arg_Prefix = 1}

    API_CheckPluginInited();

    static prefix[PH_GROUP_PREFIX_MAX_LEN];
    get_string(Arg_Prefix, prefix, charsmax(prefix));

    if (prefix[0] == 0) {
        abort(AMX_ERR_PARAMS, "PH group prefix must not be empty.");
        return Invalid_PHGroup;
    }

    if (PHGroup_Find(prefix) != Invalid_PHGroup) {
        abort(AMX_ERR_PARAMS, "PH group with prefix '%s' is already registered.", prefix);
        return Invalid_PHGroup;
    }

    return PHGroup_Construct(prefix);
}

T_PHGroup:@API_PH_FindGroup() {
    enum {Arg_Prefix = 1}

    API_CheckPluginInited();

    static prefix[PH_GROUP_PREFIX_MAX_LEN];
    get_string(Arg_Prefix, prefix, charsmax(prefix));

    return PHGroup_Find(prefix);
}

T_PHGroup:@API_PH_GetGlobalGroup() {
    API_CheckPluginInited();

    return PHGroup_GetGlobal();
}

@API_PH_PushContext() {
    enum {Arg_Group = 1, Arg_ContextKey}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    static contextKey[PH_CONTEXT_KEY_MAX_LEN];
    get_string(Arg_ContextKey, contextKey, charsmax(contextKey));

    PHGroup_PushContext(group, contextKey);
}

@API_PH_PopContext() {
    enum {Arg_Group = 1}

    API_CheckPluginInited();

    new T_PHGroup:group = T_PHGroup:get_param(Arg_Group);

    PHGroup_PopContext(group);
}
