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

    static formatStr[PH_FORMAT_MAX_LEN];
    get_string(Arg_Format, formatStr, charsmax(formatStr));

    static out[PH_FORMAT_MAX_LEN];
    PHGroup_FormatString(formatStr, out, charsmax(out));

    set_string(Arg_Out, out, get_param(Arg_OutLen));
}

@API_PH_SetValue() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static value[PH_VALUE_MAX_LEN];
    get_string(Arg_Value, value, charsmax(value));

    PHGroup_SetCurrentValue(value);
}

@API_PH_SetIntValue() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static value[PH_VALUE_MAX_LEN];
    num_to_str(get_param(Arg_Value), value, charsmax(value));

    PHGroup_SetCurrentValue(value);
}

@API_PH_SetFloatValue() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static value[PH_VALUE_MAX_LEN];
    format(value, charsmax(value), "%.2f", get_param_f(Arg_Value));

    PHGroup_SetCurrentValue(value);
}
