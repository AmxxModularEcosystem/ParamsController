#include <amxmodx>
#include <ParamsController>

DefaultObjects_PH_Global_Register() {
    new T_PHGroup:hGlobal = ParamsController_PH_GetGlobalGroup();

    ParamsController_PH_RegisterGroupKey(hGlobal, DEFAULT_PH_GLOBAL_MAP_KEY);

    static sMap[64];
    get_mapname(sMap, charsmax(sMap));
    ParamsController_PH_Set(hGlobal, DEFAULT_PH_GLOBAL_MAP_KEY, "", sMap);
}
