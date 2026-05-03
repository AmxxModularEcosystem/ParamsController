#include <amxmodx>
#include <ParamsController>

static T_PHGroup:playerGroup = Invalid_PHGroup;

DefaultObjects_PH_Player_Register() {
    playerGroup = PCPH_RegisterGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

    PCPH_Group_RegisterKey(playerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY);
}

DefaultObjects_PH_Player_OnClientAuth(const playerIndex, const authId[]) {
    PCPH_SetForInt(playerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY, playerIndex, authId);
}
