#if defined __pc_src_phgroup_included
    #endinput
#endif
#define __pc_src_phgroup_included

#include <amxmodx>
#include <ParamsController>

enum _:S_PHGroup {
    PHGroup_Prefix[PH_GROUP_PREFIX_MAX_LEN],
    Trie:PHGroup_Callbacks,                             // ключ → хендлер форварда (INVALID_HANDLE если колбека нет)
    Trie:PHGroup_Values,                                // "ключ:контекст" → текущее значение (строка)
    Stack:PHGroup_ContextStack,                         // стек сохранённых предыдущих контекстов
    PHGroup_CurrentContext[PH_CONTEXT_KEY_MAX_LEN],     // текущий активный контекст (вершина стека)
}

static Array:g_aPHGroups = Invalid_Array;
static Trie:g_tPHGroupsMap = Invalid_Trie;
static T_PHGroup:g_hPHGlobalGroup = Invalid_PHGroup;

static T_PHGroup:g_hCurrentPHGroup = Invalid_PHGroup;
static g_sCurrentPHKey[PH_KEY_MAX_LEN];
static g_sCurrentPHContext[PH_CONTEXT_KEY_MAX_LEN];

PHGroup_Init() {
    if (g_aPHGroups == Invalid_Array) {
        g_aPHGroups = ArrayCreate(S_PHGroup, 1);
    }

    if (g_tPHGroupsMap == Invalid_Trie) {
        g_tPHGroupsMap = TrieCreate();
    }

    g_hPHGlobalGroup = PHGroup_Construct("");
}

T_PHGroup:PHGroup_Construct(const sPrefix[]) {
    new groupData[S_PHGroup];

    copy(groupData[PHGroup_Prefix], charsmax(groupData[PHGroup_Prefix]), sPrefix);
    groupData[PHGroup_Callbacks] = TrieCreate();
    groupData[PHGroup_Values] = TrieCreate();
    groupData[PHGroup_ContextStack] = Invalid_Stack;
    groupData[PHGroup_CurrentContext][0] = 0;

    new T_PHGroup:hGroup = T_PHGroup:ArrayPushArray(g_aPHGroups, groupData);

    if (sPrefix[0] != 0) {
        TrieSetCell(g_tPHGroupsMap, sPrefix, hGroup);
    }

    return hGroup;
}

T_PHGroup:PHGroup_Find(const sPrefix[]) {
    new T_PHGroup:hGroup = Invalid_PHGroup;
    TrieGetCell(g_tPHGroupsMap, sPrefix, hGroup);
    return hGroup;
}

T_PHGroup:PHGroup_GetGlobal() {
    return g_hPHGlobalGroup;
}

static PHGroup_Get(const T_PHGroup:hGroup, groupData[S_PHGroup]) {
    ArrayGetArray(g_aPHGroups, _:hGroup, groupData);
}

static PHGroup_Update(const T_PHGroup:hGroup, const groupData[S_PHGroup]) {
    ArraySetArray(g_aPHGroups, _:hGroup, groupData);
}

// Составной ключ для Values Trie: "key:contextKey" или просто "key" если контекст пустой
static PHGroup_BuildValueKey(const sKey[], const sContextKey[], sOut[], const iOutLen) {
    if (sContextKey[0] == 0) {
        copy(sOut, iOutLen, sKey);
    } else {
        format(sOut, iOutLen, "%s:%s", sKey, sContextKey);
    }
}

PHGroup_RegisterKey(const T_PHGroup:hGroup, const sKey[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    if (!TrieKeyExists(groupData[PHGroup_Callbacks], sKey)) {
        TrieSetCell(groupData[PHGroup_Callbacks], sKey, INVALID_HANDLE);
    }
}

PHGroup_SetKeyCallback(const T_PHGroup:hGroup, const sKey[], const sCallback[], const pluginIndex) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    new iOldForward;
    if (TrieGetCell(groupData[PHGroup_Callbacks], sKey, iOldForward) && iOldForward != INVALID_HANDLE) {
        DestroyForward(iOldForward);
    }

    // (const sKey[], const sContextKey[])
    new iForward = CreateOneForward(pluginIndex, sCallback, FP_STRING, FP_STRING);
    if (iForward == INVALID_HANDLE) {
        abort(AMX_ERR_GENERAL, "Can't create PH callback by '%s' func of #%d plugin.", sCallback, pluginIndex);
        return;
    }

    TrieSetCell(groupData[PHGroup_Callbacks], sKey, iForward);
}

PHGroup_PushContext(const T_PHGroup:hGroup, const sContextKey[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    if (groupData[PHGroup_ContextStack] == Invalid_Stack) {
        groupData[PHGroup_ContextStack] = CreateStack(PH_CONTEXT_KEY_MAX_LEN);
    }

    PushStackArray(groupData[PHGroup_ContextStack], groupData[PHGroup_CurrentContext], PH_CONTEXT_KEY_MAX_LEN);
    copy(groupData[PHGroup_CurrentContext], charsmax(groupData[PHGroup_CurrentContext]), sContextKey);

    PHGroup_Update(hGroup, groupData);
}

PHGroup_PopContext(const T_PHGroup:hGroup) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    if (
        groupData[PHGroup_ContextStack] != Invalid_Stack
        && !IsStackEmpty(groupData[PHGroup_ContextStack])
    ) {
        PopStackArray(groupData[PHGroup_ContextStack], groupData[PHGroup_CurrentContext], PH_CONTEXT_KEY_MAX_LEN);
    } else {
        groupData[PHGroup_CurrentContext][0] = 0;
    }

    PHGroup_Update(hGroup, groupData);
}

PHGroup_SetValue(const T_PHGroup:hGroup, const sKey[], const sContextKey[], const sValue[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    static sValueKey[PH_KEY_MAX_LEN + PH_CONTEXT_KEY_MAX_LEN + 2];
    PHGroup_BuildValueKey(sKey, sContextKey, sValueKey, charsmax(sValueKey));

    TrieSetString(groupData[PHGroup_Values], sValueKey, sValue);
}

static bool:PHGroup_GetValue(const T_PHGroup:hGroup, const sKey[], const sContextKey[], sOut[], const iOutLen) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    static sValueKey[PH_KEY_MAX_LEN + PH_CONTEXT_KEY_MAX_LEN + 2];
    PHGroup_BuildValueKey(sKey, sContextKey, sValueKey, charsmax(sValueKey));

    return bool:TrieGetString(groupData[PHGroup_Values], sValueKey, sOut, iOutLen);
}

static bool:PHGroup_IsKeyRegistered(const T_PHGroup:hGroup, const sKey[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);
    return bool:TrieKeyExists(groupData[PHGroup_Callbacks], sKey);
}

static PHGroup_CallCallback(const T_PHGroup:hGroup, const sKey[], const sContextKey[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(hGroup, groupData);

    new iForward;
    if (!TrieGetCell(groupData[PHGroup_Callbacks], sKey, iForward) || iForward == INVALID_HANDLE) {
        return;
    }

    g_hCurrentPHGroup = hGroup;
    copy(g_sCurrentPHKey, charsmax(g_sCurrentPHKey), sKey);
    copy(g_sCurrentPHContext, charsmax(g_sCurrentPHContext), sContextKey);

    new iRet;
    ExecuteForward(iForward, iRet, sKey, sContextKey);

    g_hCurrentPHGroup = Invalid_PHGroup;
    g_sCurrentPHKey[0] = 0;
    g_sCurrentPHContext[0] = 0;
}

PHGroup_FormatString(const sFormat[], sOut[], const iOutLen) {
    if (iOutLen < 1) {
        return;
    }

    new iOutPos = 0;
    new iInPos = 0;
    new iFormatLen = strlen(sFormat);

    while (iInPos < iFormatLen && iOutPos < iOutLen - 1) {
        if (sFormat[iInPos] != '{') {
            sOut[iOutPos++] = sFormat[iInPos++];
            continue;
        }

        new iClosing = iInPos + 1;
        while (iClosing < iFormatLen && sFormat[iClosing] != '}') {
            ++iClosing;
        }

        if (iClosing >= iFormatLen) {
            // Незакрытая скобка — выводим как есть
            sOut[iOutPos++] = sFormat[iInPos++];
            continue;
        }

        static sPlaceholder[PH_GROUP_PREFIX_MAX_LEN + PH_KEY_MAX_LEN + 2];
        new iPlaceholderLen = min(iClosing - iInPos - 1, charsmax(sPlaceholder));

        for (new k = 0; k < iPlaceholderLen; ++k) {
            sPlaceholder[k] = sFormat[iInPos + 1 + k];
        }
        sPlaceholder[iPlaceholderLen] = 0;

        static sPrefix[PH_GROUP_PREFIX_MAX_LEN];
        static sKey[PH_KEY_MAX_LEN];

        new iColon = -1;
        for (new k = 0; k < iPlaceholderLen; ++k) {
            if (sPlaceholder[k] == ':') {
                iColon = k;
                break;
            }
        }

        if (iColon < 0) {
            sPrefix[0] = 0;
            copy(sKey, charsmax(sKey), sPlaceholder);
        } else {
            new iPrefixLen = min(iColon, charsmax(sPrefix));
            for (new k = 0; k < iPrefixLen; ++k) {
                sPrefix[k] = sPlaceholder[k];
            }
            sPrefix[iPrefixLen] = 0;
            copy(sKey, charsmax(sKey), sPlaceholder[iColon + 1]);
        }

        new T_PHGroup:hGroup = (sPrefix[0] == 0)
            ? g_hPHGlobalGroup
            : PHGroup_Find(sPrefix);

        if (hGroup != Invalid_PHGroup && PHGroup_IsKeyRegistered(hGroup, sKey)) {
            // Захватываем контекст до вызова колбека
            static sCurrentContext[PH_CONTEXT_KEY_MAX_LEN];
            new groupData[S_PHGroup];
            PHGroup_Get(hGroup, groupData);
            copy(sCurrentContext, charsmax(sCurrentContext), groupData[PHGroup_CurrentContext]);

            PHGroup_CallCallback(hGroup, sKey, sCurrentContext);

            static sValue[PH_VALUE_MAX_LEN];
            if (PHGroup_GetValue(hGroup, sKey, sCurrentContext, sValue, charsmax(sValue))) {
                new iValueLen = strlen(sValue);
                for (new k = 0; k < iValueLen && iOutPos < iOutLen - 1; ++k) {
                    sOut[iOutPos++] = sValue[k];
                }
            }
        } else {
            // Неизвестная группа или незарегистрированный ключ — сохраняем как есть
            sOut[iOutPos++] = '{';
            for (new k = 0; k < iPlaceholderLen && iOutPos < iOutLen - 1; ++k) {
                sOut[iOutPos++] = sPlaceholder[k];
            }
            if (iOutPos < iOutLen - 1) {
                sOut[iOutPos++] = '}';
            }
        }

        iInPos = iClosing + 1;
    }

    sOut[iOutPos] = 0;
}

bool:PHGroup_IsInCallback() {
    return g_hCurrentPHGroup != Invalid_PHGroup;
}

PHGroup_SetCurrentValue(const sValue[]) {
    PHGroup_SetValue(g_hCurrentPHGroup, g_sCurrentPHKey, g_sCurrentPHContext, sValue);
}

API_CheckCurrentPH() {
    if (!PHGroup_IsInCallback()) {
        abort(AMX_ERR_PARAMS, "Attempt to set PH value outside the key callback.");
    }
}
