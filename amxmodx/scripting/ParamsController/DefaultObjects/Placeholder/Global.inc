#include <amxmodx>
#include <ParamsController>

static GlobalStartTime = 0;

DefaultObjects_PH_Global_Register() {
    new value[PH_VALUE_MAX_LEN];
    
    get_mapname(value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_MAP_KEY, value);
    
    get_mapname(value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_REAL_MAP_KEY, value);
    
    get_user_name(0, value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_SERVER_NAME_KEY, value);
    
    get_user_ip(0, value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_SERVER_IP_KEY, value);
    
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_MAX_PLAYERS_COUNT_KEY, fmt("%d", get_maxplayers()));

    get_modname(value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_MOD_KEY, value);

    get_amxx_verstring(value, charsmax(value));
    PCPH_Gl_ConstKey(DEFAULT_PH_GLOBAL_AMXX_VERSION_KEY, value);
    
    PCPH_Gl_RegisterKey(DEFAULT_PH_GLOBAL_DATE_KEY);
    @DefaultObjects_PH_Global_UpdateDate();
    set_task(60.0, "@DefaultObjects_PH_Global_UpdateDate", .flags = "b");
    
    PCPH_Gl_RegisterKey(DEFAULT_PH_GLOBAL_PLAYERS_COUNT_KEY);
    DefaultObjects_PH_Global_UpdatePlayersCount();

    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_TIME_KEY, "@DefaultObjects_PH_Global_Callback_Time");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_DATETIME_KEY, "@DefaultObjects_PH_Global_Callback_Datetime");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_TIME_LEFT_KEY, "@DefaultObjects_PH_Global_Callback_TimeLeft");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_GAMETIME_KEY, "@DefaultObjects_PH_Global_Callback_Gametime");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_ALIVE_COUNT_KEY, "@DefaultObjects_PH_Global_Callback_AliveCount");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_DEAD_COUNT_KEY, "@DefaultObjects_PH_Global_Callback_DeadCount");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_BOT_COUNT_KEY, "@DefaultObjects_PH_Global_Callback_BotCount");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_HUMAN_COUNT_KEY, "@DefaultObjects_PH_Global_Callback_HumanCount");

    GlobalStartTime = get_systime();

    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_UPTIME_KEY, "@DefaultObjects_PH_Global_Callback_Uptime");
    PCPH_Gl_RegKey(DEFAULT_PH_GLOBAL_NEXTMAP_KEY, "@DefaultObjects_PH_Global_Callback_Nextmap");
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

// Форматирует длительность в "Ч:ММ:СС", либо "Nд Ч:ММ:СС" для значений от суток.
DefaultObjects_PH_Global_FormatDuration(const seconds, out[], const outLen) {
    if (seconds >= 86400) {
        formatex(out, outLen, "%dd %02d:%02d:%02d", seconds / 86400, (seconds % 86400) / 3600, (seconds % 3600) / 60, seconds % 60);
    } else {
        formatex(out, outLen, "%02d:%02d:%02d", seconds / 3600, (seconds % 3600) / 60, seconds % 60);
    }
}

// Считает игроков по фильтру get_players (например, "ah" — живые, без HLTV).
DefaultObjects_PH_Global_CountPlayers(const flags[]) {
    new players[MAX_PLAYERS], count;
    get_players(players, count, flags);

    return count;
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

@DefaultObjects_PH_Global_Callback_Uptime() {
    new value[32];
    DefaultObjects_PH_Global_FormatDuration(get_systime() - GlobalStartTime, value, charsmax(value));

    PCPH_Cb_Set(value);
}

@DefaultObjects_PH_Global_Callback_Nextmap() {
    new value[PH_VALUE_MAX_LEN];

    get_cvar_string("amx_nextmap", value, charsmax(value));
    if (value[0] == EOS) {
        copy(value, charsmax(value), "-");
    }

    PCPH_Cb_Set(value);
}

@DefaultObjects_PH_Global_Callback_Gametime() {
    new value[32];
    DefaultObjects_PH_Global_FormatDuration(floatround(get_gametime()), value, charsmax(value));

    PCPH_Cb_Set(value);
}

@DefaultObjects_PH_Global_Callback_TimeLeft() {
    new timeleft = get_timeleft();
    if (timeleft < 0) {
        timeleft = 0;
    }

    new value[32];
    DefaultObjects_PH_Global_FormatDuration(timeleft, value, charsmax(value));

    PCPH_Cb_Set(value);
}

@DefaultObjects_PH_Global_Callback_AliveCount() {
    PCPH_Cb_SetInt(DefaultObjects_PH_Global_CountPlayers("ah"));
}

@DefaultObjects_PH_Global_Callback_DeadCount() {
    PCPH_Cb_SetInt(DefaultObjects_PH_Global_CountPlayers("bh"));
}

@DefaultObjects_PH_Global_Callback_BotCount() {
    PCPH_Cb_SetInt(DefaultObjects_PH_Global_CountPlayers("dh"));
}

@DefaultObjects_PH_Global_Callback_HumanCount() {
    PCPH_Cb_SetInt(DefaultObjects_PH_Global_CountPlayers("ch"));
}
