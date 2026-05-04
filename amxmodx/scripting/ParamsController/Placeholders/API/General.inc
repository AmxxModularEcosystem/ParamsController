#include <amxmodx>
#include "ParamsController/API/Utils"
#include "ParamsController/Placeholders/Objects/PHGroup"
#include "ParamsController/Placeholders/Objects/PHTemplate"

API_PH_General_Register() {
    register_native("PCPH_Format", "@API_PH_Format");
    register_native("PCPH_CompileTemplate", "@API_PH_CompileTemplate");
    register_native("PCPH_FormatTemplate", "@API_PH_FormatTemplate");
    register_native("PCPH_DestroyTemplate", "@API_PH_DestroyTemplate");
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

@API_PH_CompileTemplate() {
    enum {Arg_Format = 1}

    API_CheckPluginInited();

    static str[PH_FORMAT_MAX_LEN];
    get_string(Arg_Format, str, charsmax(str));

    return _:PHTemplate_Compile(str);
}

@API_PH_FormatTemplate() {
    enum {Arg_Tmpl = 1, Arg_Out, Arg_OutLen}

    API_CheckPluginInited();

    new T_PHTemplate:tmpl = T_PHTemplate:get_param(Arg_Tmpl);
    if (tmpl == Invalid_PHTemplate) {
        abort(AMX_ERR_PARAMS, "Invalid PHTemplate handle.");
        return;
    }

    static out[PH_FORMAT_MAX_LEN];
    PHTemplate_Format(tmpl, out, charsmax(out));
    set_string(Arg_Out, out, get_param(Arg_OutLen));
}

@API_PH_DestroyTemplate() {
    enum {Arg_Tmpl = 1}

    API_CheckPluginInited();

    new T_PHTemplate:tmpl = T_PHTemplate:get_param(Arg_Tmpl);
    PHTemplate_Destroy(tmpl);
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
