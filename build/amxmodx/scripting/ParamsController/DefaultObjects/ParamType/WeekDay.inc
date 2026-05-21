#include <amxmodx>
#include <json>
#include <ParamsController>

static Trie:WeekDaysMap = Invalid_Trie;

DefaultObjects_ParamType_WeekDay_Register() {
    WeekDaysMap = TrieCreate();

    ParamsController_RegSimpleType(DEFAULT_PARAMS_WEEK_DAY_NAME, "@DefaultObjects_ParamType_WeekDay_OnRead");
}

bool:@DefaultObjects_ParamType_WeekDay_OnRead(const JSON:valueJson) {
    static str[PARAM_SHORT_STRING_MAX_LEN];
    json_get_string(valueJson, str, charsmax(str));

    new res;
    if (TrieGetCell(WeekDaysMap, str, res)) {
        return ParamsController_SetCell(res);
    }
    
    if (equali("Вс", str) || equali("Воскресенье", str) || equali("Sun", str) || equali("Sunday", str)) {
        res = 0;
    } else if (equali("Пн", str) || equali("Понедельник", str) || equali("Mon", str) || equali("Monday", str)) {
        res = 1;
    } else if (equali("Вт", str) || equali("Вторник", str) || equali("Tue", str) || equali("Tuesday", str)) {
        res = 2;
    } else if (equali("Ср", str) || equali("Среда", str) || equali("Wed", str) || equali("Wednesday", str)) {
        res = 3;
    } else if (equali("Чт", str) || equali("Четверг", str) || equali("Thu", str) || equali("Thursday", str)) {
        res = 4;
    } else if (equali("Пт", str) || equali("Пятница", str) || equali("Fri", str) || equali("Friday", str)) {
        res = 5;
    } else if (equali("Сб", str) || equali("Суббота", str) || equali("Sat", str) || equali("Saturday", str)) {
        res = 6;
    } else {
        PCJson_LogForFile(valueJson, "WARNING", "Week day '%s' not found.", str);
        return false;
    }

    TrieSetCell(WeekDaysMap, str, res);
    
    return ParamsController_SetCell(res);
}
