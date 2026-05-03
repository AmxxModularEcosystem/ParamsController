#if defined __pc_src_phgroup_included
    #endinput
#endif
#define __pc_src_phgroup_included

#include <amxmodx>
#include <ParamsController>

enum _:S_PHGroup {
    PHGroup_Prefix[PH_GROUP_PREFIX_MAX_LEN],
    Trie:PHGroup_Callbacks,                         // ключ → хендлер форварда (INVALID_HANDLE если колбека нет)
    Trie:PHGroup_Values,                            // "ключ:контекст" → текущее значение (строка)
    Stack:PHGroup_ContextStack,                     // стек сохранённых предыдущих контекстов
    PHGroup_CurrentContext[PH_CONTEXT_KEY_MAX_LEN], // текущий активный контекст (вершина стека)
}

static Array:PHGroups = Invalid_Array;
static Trie:PHGroupsMap = Invalid_Trie;
static T_PHGroup:PHGlobalGroup = Invalid_PHGroup;

static T_PHGroup:CurrentPHGroup = Invalid_PHGroup;
static CurrentPHKey[PH_KEY_MAX_LEN];
static CurrentPHContext[PH_CONTEXT_KEY_MAX_LEN];

PHGroup_Init() {
    if (PHGroups == Invalid_Array) {
        PHGroups = ArrayCreate(S_PHGroup, 1);
    }

    if (PHGroupsMap == Invalid_Trie) {
        PHGroupsMap = TrieCreate();
    }

    PHGlobalGroup = PHGroup_Construct("");
}

T_PHGroup:PHGroup_Construct(const prefix[]) {
    new groupData[S_PHGroup];

    copy(groupData[PHGroup_Prefix], charsmax(groupData[PHGroup_Prefix]), prefix);
    groupData[PHGroup_Callbacks] = TrieCreate();
    groupData[PHGroup_Values] = TrieCreate();
    groupData[PHGroup_ContextStack] = Invalid_Stack;
    groupData[PHGroup_CurrentContext][0] = EOS;

    new T_PHGroup:group = T_PHGroup:ArrayPushArray(PHGroups, groupData);

    if (prefix[0] != 0) {
        TrieSetCell(PHGroupsMap, prefix, group);
    }

    return group;
}

T_PHGroup:PHGroup_Find(const prefix[]) {
    new T_PHGroup:group = Invalid_PHGroup;
    TrieGetCell(PHGroupsMap, prefix, group);
    return group;
}

T_PHGroup:PHGroup_GetGlobal() {
    return PHGlobalGroup;
}

static PHGroup_Get(const T_PHGroup:group, groupData[S_PHGroup]) {
    ArrayGetArray(PHGroups, _:group, groupData);
}

static PHGroup_Update(const T_PHGroup:group, const groupData[S_PHGroup]) {
    ArraySetArray(PHGroups, _:group, groupData);
}

// Составной ключ для Values Trie: "key:contextKey" или просто "key" если контекст пустой
static PHGroup_BuildValueKey(const key[], const contextKey[], out[], const outLen) {
    if (contextKey[0] == 0) {
        copy(out, outLen, key);
    } else {
        format(out, outLen, "%s:%s", key, contextKey);
    }
}

PHGroup_RegisterKey(const T_PHGroup:group, const key[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    if (!TrieKeyExists(groupData[PHGroup_Callbacks], key)) {
        TrieSetCell(groupData[PHGroup_Callbacks], key, INVALID_HANDLE);
    }
}

PHGroup_SetKeyCallback(const T_PHGroup:group, const key[], const callback[], const pluginIndex) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    if (!TrieKeyExists(groupData[PHGroup_Callbacks], key)) {
        abort(AMX_ERR_PARAMS, "PH key '%s' is not registered. Call RegisterGroupKey first.", key);
        return;
    }

    new oldFwd;
    if (TrieGetCell(groupData[PHGroup_Callbacks], key, oldFwd) && oldFwd != INVALID_HANDLE) {
        DestroyForward(oldFwd);
    }

    // (const key[], const contextKey[])
    new fwd = CreateOneForward(pluginIndex, callback, FP_STRING, FP_STRING);
    if (fwd == INVALID_HANDLE) {
        abort(AMX_ERR_GENERAL, "Can't create PH callback by '%s' func of #%d plugin.", callback, pluginIndex);
        return;
    }

    TrieSetCell(groupData[PHGroup_Callbacks], key, fwd);
}

PHGroup_PushContext(const T_PHGroup:group, const contextKey[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    if (groupData[PHGroup_ContextStack] == Invalid_Stack) {
        groupData[PHGroup_ContextStack] = CreateStack(PH_CONTEXT_KEY_MAX_LEN);
    }

    PushStackArray(groupData[PHGroup_ContextStack], groupData[PHGroup_CurrentContext], PH_CONTEXT_KEY_MAX_LEN);
    copy(groupData[PHGroup_CurrentContext], charsmax(groupData[PHGroup_CurrentContext]), contextKey);

    PHGroup_Update(group, groupData);
}

PHGroup_PopContext(const T_PHGroup:group) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    if (
        groupData[PHGroup_ContextStack] != Invalid_Stack
        && !IsStackEmpty(groupData[PHGroup_ContextStack])
    ) {
        PopStackArray(groupData[PHGroup_ContextStack], groupData[PHGroup_CurrentContext], PH_CONTEXT_KEY_MAX_LEN);
    } else {
        groupData[PHGroup_CurrentContext][0] = EOS;
    }

    PHGroup_Update(group, groupData);
}

PHGroup_SetValue(const T_PHGroup:group, const key[], const contextKey[], const value[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    static valueKey[PH_KEY_MAX_LEN + PH_CONTEXT_KEY_MAX_LEN + 2];
    PHGroup_BuildValueKey(key, contextKey, valueKey, charsmax(valueKey));

    TrieSetString(groupData[PHGroup_Values], valueKey, value);
}

static bool:PHGroup_GetValue(const T_PHGroup:group, const key[], const contextKey[], out[], const outLen) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    static valueKey[PH_KEY_MAX_LEN + PH_CONTEXT_KEY_MAX_LEN + 2];
    PHGroup_BuildValueKey(key, contextKey, valueKey, charsmax(valueKey));

    return bool:TrieGetString(groupData[PHGroup_Values], valueKey, out, outLen);
}

static bool:PHGroup_IsKeyRegistered(const T_PHGroup:group, const key[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);
    return bool:TrieKeyExists(groupData[PHGroup_Callbacks], key);
}

static PHGroup_CallCallback(const T_PHGroup:group, const key[], const contextKey[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    new fwd;
    if (!TrieGetCell(groupData[PHGroup_Callbacks], key, fwd) || fwd == INVALID_HANDLE) {
        return;
    }

    CurrentPHGroup = group;
    copy(CurrentPHKey, charsmax(CurrentPHKey), key);
    copy(CurrentPHContext, charsmax(CurrentPHContext), contextKey);

    new ret;
    ExecuteForward(fwd, ret, key, contextKey);

    CurrentPHGroup = Invalid_PHGroup;
    CurrentPHKey[0] = EOS;
    CurrentPHContext[0] = EOS;
}

PHGroup_FormatString(const src[], out[], const outLen) {
    if (outLen < 1) {
        return;
    }

    new outPos = 0;
    new inPos = 0;
    new srcLen = strlen(src);

    while (inPos < srcLen && outPos < outLen - 1) {
        if (src[inPos] != '{') {
            out[outPos++] = src[inPos++];
            continue;
        }

        new closing = inPos + 1;
        while (closing < srcLen && src[closing] != '}') {
            ++closing;
        }

        if (closing >= srcLen) {
            // Незакрытая скобка — выводим как есть
            out[outPos++] = src[inPos++];
            continue;
        }

        static placeholder[PH_GROUP_PREFIX_MAX_LEN + PH_KEY_MAX_LEN + 2];
        new placeholderLen = min(closing - inPos - 1, charsmax(placeholder));

        for (new k = 0; k < placeholderLen; ++k) {
            placeholder[k] = src[inPos + 1 + k];
        }
        placeholder[placeholderLen] = 0;

        static prefix[PH_GROUP_PREFIX_MAX_LEN];
        static key[PH_KEY_MAX_LEN];

        new colon = -1;
        for (new k = 0; k < placeholderLen; ++k) {
            if (placeholder[k] == ':') {
                colon = k;
                break;
            }
        }

        if (colon < 0) {
            prefix[0] = 0;
            copy(key, charsmax(key), placeholder);
        } else {
            new prefixLen = min(colon, charsmax(prefix));
            for (new k = 0; k < prefixLen; ++k) {
                prefix[k] = placeholder[k];
            }
            prefix[prefixLen] = 0;
            copy(key, charsmax(key), placeholder[colon + 1]);
        }

        new T_PHGroup:group = (prefix[0] == 0)
            ? PHGlobalGroup
            : PHGroup_Find(prefix);

        if (group != Invalid_PHGroup && PHGroup_IsKeyRegistered(group, key)) {
            // Захватываем контекст до вызова колбека
            static currentContext[PH_CONTEXT_KEY_MAX_LEN];
            new groupData[S_PHGroup];
            PHGroup_Get(group, groupData);
            copy(currentContext, charsmax(currentContext), groupData[PHGroup_CurrentContext]);

            PHGroup_CallCallback(group, key, currentContext);

            static value[PH_VALUE_MAX_LEN];
            if (PHGroup_GetValue(group, key, currentContext, value, charsmax(value))) {
                new valueLen = strlen(value);
                for (new k = 0; k < valueLen && outPos < outLen - 1; ++k) {
                    out[outPos++] = value[k];
                }
            }
        } else {
            // Неизвестная группа или незарегистрированный ключ — сохраняем как есть
            out[outPos++] = '{';
            for (new k = 0; k < placeholderLen && outPos < outLen - 1; ++k) {
                out[outPos++] = placeholder[k];
            }
            if (outPos < outLen - 1) {
                out[outPos++] = '}';
            }
        }

        inPos = closing + 1;
    }

    out[outPos] = 0;
}

static PHGroup_GetKeyForward(const T_PHGroup:group, const key[]) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    new fwd = INVALID_HANDLE;
    TrieGetCell(groupData[PHGroup_Callbacks], key, fwd);
    return fwd;
}

bool:PHGroup_IsInCallback() {
    return CurrentPHGroup != Invalid_PHGroup;
}

PHGroup_SetCurrentValue(const value[]) {
    PHGroup_SetValue(CurrentPHGroup, CurrentPHKey, CurrentPHContext, value);
}

API_CheckCurrentPH() {
    if (!PHGroup_IsInCallback()) {
        abort(AMX_ERR_PARAMS, "Attempt to set PH value outside the key callback.");
    }
}
