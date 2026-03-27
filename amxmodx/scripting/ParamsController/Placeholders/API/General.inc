#include <amxmodx>
#include "ParamsController/API/Utils"
#include "ParamsController/Placeholders/Objects/PHGroup"

API_PH_General_Register() {
    register_native("ParamsController_PH_Format", "@API_PH_Format");
    register_native("ParamsController_PH_SetValue", "@API_PH_SetValue");
    register_native("ParamsController_PH_SetIntValue", "@API_PH_SetIntValue");
    register_native("ParamsController_PH_SetFloatValue", "@API_PH_SetFloatValue");
}

@API_PH_Format() {
    enum {Arg_Out = 1, Arg_OutLen, Arg_Format}

    API_CheckPluginInited();

    static sFormat[PH_FORMAT_MAX_LEN];
    get_string(Arg_Format, sFormat, charsmax(sFormat));

    static sOut[PH_FORMAT_MAX_LEN];
    PHGroup_FormatString(sFormat, sOut, charsmax(sOut));

    set_string(Arg_Out, sOut, get_param(Arg_OutLen));
}

@API_PH_SetValue() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static sValue[PH_VALUE_MAX_LEN];
    get_string(Arg_Value, sValue, charsmax(sValue));

    PHGroup_SetCurrentValue(sValue);
}

@API_PH_SetIntValue() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static sValue[PH_VALUE_MAX_LEN];
    num_to_str(get_param(Arg_Value), sValue, charsmax(sValue));

    PHGroup_SetCurrentValue(sValue);
}

@API_PH_SetFloatValue() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static sValue[PH_VALUE_MAX_LEN];
    format(sValue, charsmax(sValue), "%.2f", get_param_f(Arg_Value));

    PHGroup_SetCurrentValue(sValue);
}
