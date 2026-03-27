#include <amxmodx>
#include <ParamsController>

static T_PHGroup:g_hPlayerGroup = Invalid_PHGroup;

DefaultObjects_PH_Player_Register() {
    g_hPlayerGroup = ParamsController_PH_RegisterGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

    ParamsController_PH_RegisterGroupKey(g_hPlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY);
}

DefaultObjects_PH_Player_OnClientAuth(const playerIndex, const sAuthID[]) {
    ParamsController_PH_SetForInt(g_hPlayerGroup, DEFAULT_PH_PLAYER_AUTHID_KEY, playerIndex, sAuthID);
}
