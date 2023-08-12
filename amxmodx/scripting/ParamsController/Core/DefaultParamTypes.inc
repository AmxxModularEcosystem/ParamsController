#include <amxmodx>
#include <json>
#include <ParamsController>

RegisterDefaultParamTypes() {
    // Идея: Либо отдельными типами, либо встроить в ядро поддержку чтения типов массивами
    // e.g. тип "String[]" будет читать массив обычных строк
    
    ParamsController_RegSimpleType("Boolean", "@OnBooleanRead");
    ParamsController_RegSimpleType("Integer", "@OnIntegerRead");
    ParamsController_RegSimpleType("Float", "@OnFloatRead");
    ParamsController_RegSimpleType("String", "@OnStringRead");
    ParamsController_RegSimpleType("ShortString", "@OnShortStringRead");
    ParamsController_RegSimpleType("LongString", "@OnLongStringRead");
}

@OnBooleanRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_bool(jValue));
}

@OnIntegerRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_number(jValue));
}

@OnFloatRead(const JSON:jValue) {
    return ParamsController_SetCell(json_get_real(jValue));
}

@OnStringRead(const JSON:jValue) {
    new sValue[PARAM_VALUE_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

@OnShortStringRead(const JSON:jValue) {
    new sValue[PARAM_SHORT_STRING_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}

@OnLongStringRead(const JSON:jValue) {
    new sValue[PARAM_LONG_STRING_MAX_LEN];
    json_get_string(jValue, sValue, charsmax(sValue));

    return ParamsController_SetString(sValue);
}
