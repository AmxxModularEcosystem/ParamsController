#include <amxmodx>
#include <ParamsController>

static T_PHGroup:PlayerGroup = Invalid_PHGroup;

DefaultObjects_PH_Player_Register() {
    PlayerGroup = PCPH_RegisterGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY);
    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_NAME_KEY);
    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_IP_KEY);
    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_USERID_KEY);

    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_HEALTH_KEY, "@DefaultObjects_PH_Player_Callback_Health");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_ARMOR_KEY, "@DefaultObjects_PH_Player_Callback_Armor");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_FRAGS_KEY, "@DefaultObjects_PH_Player_Callback_Frags");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_PING_KEY, "@DefaultObjects_PH_Player_Callback_Ping");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_CONNECT_TIME_KEY, "@DefaultObjects_PH_Player_Callback_ConnectTime");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_DEATHS_KEY, "@DefaultObjects_PH_Player_Callback_Deaths");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_TEAM_KEY, "@DefaultObjects_PH_Player_Callback_Team");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_TEAM_NAME_KEY, "@DefaultObjects_PH_Player_Callback_TeamName");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_ALIVE_KEY, "@DefaultObjects_PH_Player_Callback_Alive");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_BOT_KEY, "@DefaultObjects_PH_Player_Callback_Bot");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_LOSS_KEY, "@DefaultObjects_PH_Player_Callback_Loss");
    PCPH_Group_RegKey(PlayerGroup, DEFAULT_PH_PLAYER_INDEX_KEY, "@DefaultObjects_PH_Player_Callback_Index");
}

DefaultObjects_PH_Player_OnClientPutInServer(const playerIndex) {
    static value[PH_VALUE_MAX_LEN];

    get_user_name(playerIndex, value, charsmax(value));
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_NAME_KEY, playerIndex, value);

    PCPH_SetIntForInt(PlayerGroup, DEFAULT_PH_PLAYER_USERID_KEY, playerIndex, get_user_userid(playerIndex));
}

DefaultObjects_PH_Player_OnClientDisconnected(const playerIndex) {
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY, playerIndex, "");
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_NAME_KEY, playerIndex, "");
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_IP_KEY, playerIndex, "");
    PCPH_SetIntForInt(PlayerGroup, DEFAULT_PH_PLAYER_USERID_KEY, playerIndex, -1);
}

DefaultObjects_PH_Player_OnClientAuth(const playerIndex, const authId[]) {
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY, playerIndex, authId);

    static value[PH_VALUE_MAX_LEN];

    get_user_ip(playerIndex, value, charsmax(value));
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_IP_KEY, playerIndex, value);
}

@DefaultObjects_PH_Player_Callback_Health(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) ? get_user_health(playerIndex) : 0);
}

@DefaultObjects_PH_Player_Callback_Armor(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) ? get_user_armor(playerIndex) : 0);
}

@DefaultObjects_PH_Player_Callback_Frags(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) ? get_user_frags(playerIndex) : 0);
}

@DefaultObjects_PH_Player_Callback_Ping(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    new ping = 0;
    if (is_user_connected(playerIndex)) {
        new loss;
        get_user_ping(playerIndex, ping, loss);
    }

    PCPH_Cb_SetInt(ping);
}

@DefaultObjects_PH_Player_Callback_ConnectTime(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    if (!is_user_connected(playerIndex)) {
        PCPH_Cb_Set("0:00:00");
        return;
    }

    new seconds = get_user_time(playerIndex);
    new value[24];

    formatex(value, charsmax(value), "%d:%02d:%02d", seconds / 3600, (seconds % 3600) / 60, seconds % 60);

    PCPH_Cb_Set(value);
}

@DefaultObjects_PH_Player_Callback_Deaths(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) ? get_user_deaths(playerIndex) : 0);
}

@DefaultObjects_PH_Player_Callback_Team(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) ? get_user_team(playerIndex) : 0);
}

@DefaultObjects_PH_Player_Callback_TeamName(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    new value[PH_VALUE_MAX_LEN];
    if (is_user_connected(playerIndex)) {
        get_user_team(playerIndex, value, charsmax(value));
    } else {
        value[0] = EOS;
    }

    PCPH_Cb_Set(value);
}

@DefaultObjects_PH_Player_Callback_Alive(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) && is_user_alive(playerIndex) ? 1 : 0);
}

@DefaultObjects_PH_Player_Callback_Bot(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    PCPH_Cb_SetInt(is_user_connected(playerIndex) && is_user_bot(playerIndex) ? 1 : 0);
}

@DefaultObjects_PH_Player_Callback_Loss(const key[], const contextKey[]) {
    new playerIndex = str_to_num(contextKey);

    new loss = 0;
    if (is_user_connected(playerIndex)) {
        new ping;
        get_user_ping(playerIndex, ping, loss);
    }

    PCPH_Cb_SetInt(loss);
}

@DefaultObjects_PH_Player_Callback_Index(const key[], const contextKey[]) {
    PCPH_Cb_SetInt(str_to_num(contextKey));
}
