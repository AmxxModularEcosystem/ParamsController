#include <amxmodx>
#include "ParamsController/API/Utils"
#include "ParamsController/Placeholders/Objects/PHGroup"

API_PH_General_Register() {
    register_native("PCPH_Format", "@API_PH_Format");
    register_native("PCPH_Cb_Set", "@API_PH_Cb_Set");
    register_native("PCPH_Cb_SetInt", "@API_PH_Cb_SetInt");
    register_native("PCPH_Cb_SetFloat", "@API_PH_Cb_SetFloat");
}

@API_PH_Format() {
    enum {Arg_Out = 1, Arg_OutLen, Arg_Format}

    API_CheckPluginInited();

    static str[PH_FORMAT_MAX_LEN];
    get_string(Arg_Format, str, charsmax(str));

    static out[PH_FORMAT_MAX_LEN];
    PHGroup_FormatString(str, out, charsmax(out));

    set_string(Arg_Out, out, get_param(Arg_OutLen));
}

@API_PH_Cb_Set() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static value[PH_VALUE_MAX_LEN];
    get_string(Arg_Value, value, charsmax(value));

    PHGroup_SetCurrentValue(value);
}

@API_PH_Cb_SetInt() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static value[PH_VALUE_MAX_LEN];
    num_to_str(get_param(Arg_Value), value, charsmax(value));

    PHGroup_SetCurrentValue(value);
}

@API_PH_Cb_SetFloat() {
    enum {Arg_Value = 1}

    API_CheckPluginInited();
    API_CheckCurrentPH();

    static value[PH_VALUE_MAX_LEN];
    format(value, charsmax(value), "%.2f", get_param_f(Arg_Value));

    PHGroup_SetCurrentValue(value);
}
