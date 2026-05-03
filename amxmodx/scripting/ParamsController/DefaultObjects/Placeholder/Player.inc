#include <amxmodx>
#include <reapi>
#include <ParamsController>

static T_PHGroup:PlayerGroup = Invalid_PHGroup;

DefaultObjects_PH_Player_Register() {
    PlayerGroup = PCPH_RegisterGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY);
    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_NAME_KEY);
    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_IP_KEY);
    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_USERID_KEY);
}

DefaultObjects_PH_Player_OnClientAuth(const playerIndex, const authId[]) {
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY, playerIndex, authId);

    static value[PH_VALUE_MAX_LEN];

    get_user_name(playerIndex, value, charsmax(value));
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_NAME_KEY, playerIndex, value);

    get_user_ip(playerIndex, value, charsmax(value));
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_IP_KEY, playerIndex, value);

    PCPH_SetIntForInt(PlayerGroup, DEFAULT_PH_PLAYER_USERID_KEY, playerIndex, get_user_userid(playerIndex));
}
