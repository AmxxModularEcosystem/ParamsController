#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_PHTemplate_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_PH_TEMPLATE_NAME, "@DefaultObjects_ParamType_PHTemplate_OnRead");
}

bool:@DefaultObjects_ParamType_PHTemplate_OnRead(const JSON:valueJson) {
    static src[PARAM_VALUE_MAX_LEN];
    json_get_string(valueJson, src, charsmax(src));

    new T_PHTemplate:tmpl = PCPH_CompileTemplate(src);

    return ParamsController_SetCell(tmpl);
}
