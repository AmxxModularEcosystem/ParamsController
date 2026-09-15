#if defined __pc_src_phgroup_included
    #endinput
#endif
#define __pc_src_phgroup_included

#include <amxmodx>
#include <ParamsController>
#include "ParamsController/Placeholders/PHParse"

enum _:S_PHGroup {
    PHGroup_Prefix[PH_GROUP_PREFIX_MAX_LEN],
    Trie:PHGroup_Callbacks,                         // ключ → хендлер форварда (INVALID_HANDLE если колбека нет)
    Trie:PHGroup_Values,                            // "ключ:контекст" → текущее значение (строка)
    Stack:PHGroup_ContextStack,                     // стек сохранённых предыдущих контекстов
    PHGroup_CurrentContext[PH_CONTEXT_KEY_MAX_LEN], // текущий активный контекст (вершина стека)
    T_PHGroup:PHGroup_Source,                       // источник данных (Invalid_PHGroup для обычных групп)
}

static Array:PHGroups = Invalid_Array;
static Trie:PHGroupsMap = Invalid_Trie;
static T_PHGroup:PHGlobalGroup = Invalid_PHGroup;

static T_PHGroup:CurrentPHGroup = Invalid_PHGroup;
static CurrentPHKey[PH_KEY_MAX_LEN];
static CurrentPHContext[PH_CONTEXT_KEY_MAX_LEN];

// Стек состояния текущего колбека. Позволяет вызывать PCPH_Format (и другие
// колбеки) рекурсивно изнутри колбека ключа: после вложенного вызова состояние
// внешнего колбека (группа/ключ/контекст) восстанавливается, и PCPH_Cb_Set* 
// во внешнем колбеке продолжает работать с корректными значениями.
enum _:S_PHCurrentCallback {
    T_PHGroup:PHCurrentCallback_Group,
    PHCurrentCallback_Key[PH_KEY_MAX_LEN],
    PHCurrentCallback_Context[PH_CONTEXT_KEY_MAX_LEN],
}
static Stack:CurrentPHCallbacks = Invalid_Stack;

static PHGroup_PushCurrentCallback(const T_PHGroup:group, const key[], const contextKey[]) {
    if (CurrentPHCallbacks == Invalid_Stack) {
        CurrentPHCallbacks = CreateStack(S_PHCurrentCallback);
    }

    new entry[S_PHCurrentCallback];
    entry[PHCurrentCallback_Group] = CurrentPHGroup;
    copy(entry[PHCurrentCallback_Key], charsmax(entry[PHCurrentCallback_Key]), CurrentPHKey);
    copy(entry[PHCurrentCallback_Context], charsmax(entry[PHCurrentCallback_Context]), CurrentPHContext);
    PushStackArray(CurrentPHCallbacks, entry);

    CurrentPHGroup = group;
    copy(CurrentPHKey, charsmax(CurrentPHKey), key);
    copy(CurrentPHContext, charsmax(CurrentPHContext), contextKey);
}

static PHGroup_PopCurrentCallback() {
    new entry[S_PHCurrentCallback];
    if (
        CurrentPHCallbacks != Invalid_Stack
        && !IsStackEmpty(CurrentPHCallbacks)
    ) {
        PopStackArray(CurrentPHCallbacks, entry);
        CurrentPHGroup = T_PHGroup:entry[PHCurrentCallback_Group];
        copy(CurrentPHKey, charsmax(CurrentPHKey), entry[PHCurrentCallback_Key]);
        copy(CurrentPHContext, charsmax(CurrentPHContext), entry[PHCurrentCallback_Context]);
    } else {
        CurrentPHGroup = Invalid_PHGroup;
        CurrentPHKey[0] = EOS;
        CurrentPHContext[0] = EOS;
    }
}

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
    groupData[PHGroup_Source] = Invalid_PHGroup;

    new T_PHGroup:group = T_PHGroup:ArrayPushArray(PHGroups, groupData);

    if (prefix[0] != 0) {
        TrieSetCell(PHGroupsMap, prefix, group);
        log_amx("[INFO] PH group registered: prefix='%s'", prefix);
    } else {
        log_amx("[INFO] PH global group registered");
    }

    return group;
}

static T_PHGroup:PHGroup_DataGroup(const T_PHGroup:group) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);
    if (groupData[PHGroup_Source] != Invalid_PHGroup) {
        return groupData[PHGroup_Source];
    }
    return group;
}

T_PHGroup:PHGroup_ConstructProxy(const prefix[], const T_PHGroup:sourceGroup) {
    new srcData[S_PHGroup];
    PHGroup_Get(sourceGroup, srcData);

    new groupData[S_PHGroup];
    copy(groupData[PHGroup_Prefix], charsmax(groupData[PHGroup_Prefix]), prefix);
    groupData[PHGroup_Callbacks] = Invalid_Trie;
    groupData[PHGroup_Values] = Invalid_Trie;
    groupData[PHGroup_ContextStack] = Invalid_Stack;
    groupData[PHGroup_CurrentContext][0] = EOS;
    groupData[PHGroup_Source] = sourceGroup;

    new T_PHGroup:group = T_PHGroup:ArrayPushArray(PHGroups, groupData);
    TrieSetCell(PHGroupsMap, prefix, group);

    if (srcData[PHGroup_Prefix][0] != EOS) {
        log_amx("[INFO] PH proxy group registered: prefix='%s' -> source='%s'", prefix, srcData[PHGroup_Prefix]);
    } else {
        log_amx("[INFO] PH proxy group registered: prefix='%s' -> source='(global)'", prefix);
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

// Префикс → группа: пустой префикс — глобальная группа.
// Общий хелпер для инлайн-форматирования и компиляции шаблонов.
T_PHGroup:PHGroup_ResolveByPrefix(const prefix[]) {
    return (prefix[0] == EOS) ? PHGlobalGroup : PHGroup_Find(prefix);
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

    if (groupData[PHGroup_Source] != Invalid_PHGroup) {
        abort(AMX_ERR_PARAMS, "Cannot register key on a proxy PH group.");
        return;
    }

    if (!TrieKeyExists(groupData[PHGroup_Callbacks], key)) {
        TrieSetCell(groupData[PHGroup_Callbacks], key, INVALID_HANDLE);
    }
}

PHGroup_SetKeyCallback(const T_PHGroup:group, const key[], const callback[], const pluginIndex) {
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);

    if (groupData[PHGroup_Source] != Invalid_PHGroup) {
        abort(AMX_ERR_PARAMS, "Cannot set key callback on a proxy PH group.");
        return;
    }

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
    new T_PHGroup:dataGroup = PHGroup_DataGroup(group);
    new groupData[S_PHGroup];
    PHGroup_Get(dataGroup, groupData);

    static valueKey[PH_KEY_MAX_LEN + PH_CONTEXT_KEY_MAX_LEN + 2];
    PHGroup_BuildValueKey(key, contextKey, valueKey, charsmax(valueKey));

    TrieSetString(groupData[PHGroup_Values], valueKey, value);
}

static bool:PHGroup_GetValue(const T_PHGroup:group, const key[], const contextKey[], out[], const outLen) {
    new T_PHGroup:dataGroup = PHGroup_DataGroup(group);
    new groupData[S_PHGroup];
    PHGroup_Get(dataGroup, groupData);

    static valueKey[PH_KEY_MAX_LEN + PH_CONTEXT_KEY_MAX_LEN + 2];
    PHGroup_BuildValueKey(key, contextKey, valueKey, charsmax(valueKey));

    return bool:TrieGetString(groupData[PHGroup_Values], valueKey, out, outLen);
}

bool:PHGroup_IsKeyRegistered(const T_PHGroup:group, const key[]) {
    new T_PHGroup:dataGroup = PHGroup_DataGroup(group);
    new groupData[S_PHGroup];
    PHGroup_Get(dataGroup, groupData);
    return bool:TrieKeyExists(groupData[PHGroup_Callbacks], key);
}

static PHGroup_CallCallback(const T_PHGroup:group, const key[], const contextKey[]) {
    new T_PHGroup:dataGroup = PHGroup_DataGroup(group);
    new groupData[S_PHGroup];
    PHGroup_Get(dataGroup, groupData);

    new fwd;
    if (!TrieGetCell(groupData[PHGroup_Callbacks], key, fwd) || fwd == INVALID_HANDLE) {
        return;
    }

    PHGroup_PushCurrentCallback(group, key, contextKey);

    new ret;
    ExecuteForward(fwd, ret, key, contextKey);

    PHGroup_PopCurrentCallback();
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

        new closing;
        if (!PHParse_FindClosing(src, srcLen, inPos, closing)) {
            // Незакрытая скобка — выводим как есть
            out[outPos++] = src[inPos++];
            continue;
        }

        new prefix[PH_GROUP_PREFIX_MAX_LEN], key[PH_KEY_MAX_LEN];
        new bool:isPlaceholder = PHParse_Placeholder(src, inPos, closing, prefix, charsmax(prefix), key, charsmax(key));

        new T_PHGroup:group = Invalid_PHGroup;
        if (isPlaceholder) {
            group = PHGroup_ResolveByPrefix(prefix);
        }

        if (isPlaceholder && group != Invalid_PHGroup && PHGroup_IsKeyRegistered(group, key)) {
            // Захватываем контекст до вызова колбека (локальный буфер — 
            // вложенный вызов PCPH_Format из колбека не должен его затирать)
            new currentContext[PH_CONTEXT_KEY_MAX_LEN];
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
            // Слишком длинный, неизвестная группа или незарегистрированный ключ —
            // сохраняем как есть (литеральный текст с фигурными скобками)
            out[outPos++] = '{';
            for (new k = 0; k < closing - inPos - 1 && outPos < outLen - 1; ++k) {
                out[outPos++] = src[inPos + 1 + k];
            }
            if (outPos < outLen - 1) {
                out[outPos++] = '}';
            }
        }

        inPos = closing + 1;
    }

    out[outPos] = 0;
}

PHGroup_GetKeyForward(const T_PHGroup:group, const key[]) {
    new T_PHGroup:dataGroup = PHGroup_DataGroup(group);
    new groupData[S_PHGroup];
    PHGroup_Get(dataGroup, groupData);

    new fwd = INVALID_HANDLE;
    TrieGetCell(groupData[PHGroup_Callbacks], key, fwd);
    return fwd;
}

// Вызывает колбек (если fwd != INVALID_HANDLE) и записывает текущее значение ключа в out.
// Используется скомпилированными шаблонами: группа и форвард уже известны, контекст берётся в рантайме.
bool:PHGroup_FillValue(const T_PHGroup:group, const key[], const fwd, out[], const outLen) {
    new currentContext[PH_CONTEXT_KEY_MAX_LEN];
    new groupData[S_PHGroup];
    PHGroup_Get(group, groupData);
    copy(currentContext, charsmax(currentContext), groupData[PHGroup_CurrentContext]);

    if (fwd != INVALID_HANDLE) {
        PHGroup_PushCurrentCallback(group, key, currentContext);

        new ret;
        ExecuteForward(fwd, ret, key, currentContext);

        PHGroup_PopCurrentCallback();
    }

    return PHGroup_GetValue(group, key, currentContext, out, outLen);
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
