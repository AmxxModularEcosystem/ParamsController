#include <amxmodx>
#include <reapi>
#include <ParamsController>

DefaultObjects_PH_Global_Register() {
    new value[PH_VALUE_MAX_LEN];
    
    rh_get_mapname(value, charsmax(value), MNT_SET);
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_MAP_KEY, value);
    
    rh_get_mapname(value, charsmax(value), MNT_TRUE);
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_REAL_MAP_KEY, value);
    
    get_user_name(0, value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_SERVER_NAME_KEY, value);
    
    get_user_ip(0, value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_SERVER_IP_KEY, value);
    
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_MAX_PLAYERS_COUNT_KEY, fmt("%d", get_maxplayers()));
    
    PCPH_Gl_RegisterKey(DEFAULT_PH_GLOBAL_DATE_KEY);
    @DefaultObjects_PH_Global_UpdateDate();
    set_task(60.0, "@DefaultObjects_PH_Global_UpdateDate", .flags = "b");
    
    PCPH_Gl_RegisterKey(DEFAULT_PH_GLOBAL_PLAYERS_COUNT_KEY);
    DefaultObjects_PH_Global_UpdatePlayersCount();

    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_TIME_KEY, "@DefaultObjects_PH_Global_Callback_Time");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_DATETIME_KEY, "@DefaultObjects_PH_Global_Callback_Datetime");
}

DefaultObjects_PH_Global_OnClientPutInServer(const playerIndex) {
    #pragma unused playerIndex
    DefaultObjects_PH_Global_UpdatePlayersCount();
}

DefaultObjects_PH_Global_OnClientDisconnected(const playerIndex) {
    #pragma unused playerIndex
    DefaultObjects_PH_Global_UpdatePlayersCount();
}

DefaultObjects_PH_Global_UpdatePlayersCount() {
    PCPH_SetInt(PCPH_GetGlobalGroup(), DEFAULT_PH_GLOBAL_PLAYERS_COUNT_KEY, get_playersnum());
}

@DefaultObjects_PH_Global_UpdateDate() {
    new year, month, day;
    date(year, month, day);

    PCPH_Set(PCPH_GetGlobalGroup(), DEFAULT_PH_GLOBAL_DATE_KEY, fmt("%02d.%02d.%04d", day, month, year));
}

@DefaultObjects_PH_Global_Callback_Time() {
    new hour, minute, second;
    time(hour, minute, second);
    
    PCPH_Cb_Set(fmt("%d:%02d:%02d", hour, minute, second));
}

@DefaultObjects_PH_Global_Callback_Datetime() {
    new hour, minute, second;
    time(hour, minute, second);
    
    new year, month, day;
    date(year, month, day);

    PCPH_Cb_Set(fmt("%d:%02d:%02d %02d.%02d.%04d", hour, minute, second, day, month, year));
}
