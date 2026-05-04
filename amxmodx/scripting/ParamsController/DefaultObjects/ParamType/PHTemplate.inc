#include <amxmodx>
#include <json>
#include <ParamsController>

DefaultObjects_ParamType_PHTemplate_Register() {
    ParamsController_RegSimpleType(DEFAULT_PARAMS_PH_TEMPLATE_NAME, "@DefaultObjects_ParamType_PHTemplate_OnRead");
}

bool:@DefaultObjects_ParamType_PHTemplate_OnRead(const JSON:valueJson) {
    return ParamsController_SetCell(
        PCPH_CompileTemplate(
            PCSingle_iString(valueJson)
        )
    );
}
