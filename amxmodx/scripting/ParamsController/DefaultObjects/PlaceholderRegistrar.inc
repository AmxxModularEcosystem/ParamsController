#include <amxmodx>
#include <ParamsController>

#include "ParamsController/DefaultObjects/Placeholder/Global"
#include "ParamsController/DefaultObjects/Placeholder/Player"

DefaultObjects_Placeholder_Register() {
    DefaultObjects_PH_Global_Register();
    DefaultObjects_PH_Player_Register();
}

DefaultObjects_Placeholder_OnClientAuth(const playerIndex, const sAuthID[]) {
    DefaultObjects_PH_Player_OnClientAuth(playerIndex, sAuthID);
}
