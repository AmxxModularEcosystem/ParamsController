#include <amxmodx>
#include <ParamsController>

DefaultObjects_PH_Global_Register() {
    new T_PHGroup:group = PCPH_GetGlobalGroup();

    PCPH_Group_RegisterKey(group, DEFAULT_PH_GLOBAL_MAP_KEY);

    static sMap[64];
    get_mapname(sMap, charsmax(sMap));
    PCPH_Set(group, DEFAULT_PH_GLOBAL_MAP_KEY, sMap);
}
