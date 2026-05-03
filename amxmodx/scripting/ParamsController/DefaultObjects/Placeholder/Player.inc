#include <amxmodx>
#include <ParamsController>

static T_PHGroup:PlayerGroup = Invalid_PHGroup;

DefaultObjects_PH_Player_Register() {
    PlayerGroup = PCPH_RegisterGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

    PCPH_Group_RegisterKey(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY);
}

DefaultObjects_PH_Player_OnClientAuth(const playerIndex, const authId[]) {
    PCPH_SetForInt(PlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY, playerIndex, authId);
}
