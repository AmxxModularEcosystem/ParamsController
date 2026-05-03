# Placeholder System

Плейсхолдеры — это механизм подстановки динамических значений в строки вида `{key}` или `{prefix:key}`. Используется для форматирования строк с данными, которые меняются в рантайме (имя карты, authid игрока и т.п.).

## Концепции

### Группа (`T_PHGroup`)

Группа объединяет логически связанные ключи под одним **префиксом**. В строках плейсхолдеры этой группы выглядят как `{prefix:key}`.

Специальная **глобальная группа** (пустой префикс) — для плейсхолдеров без префикса: `{key}`. Доступна через `PCPH_GetGlobalGroup()`.

### Ключ

Конкретный плейсхолдер внутри группы, например `authid` в группе `p`. Ключи регистрируются заранее; незарегистрированные плейсхолдеры сохраняются в строке как есть.

### Контекст

Когда одна группа описывает несколько сущностей (например, разные игроки), каждый плейсхолдер может иметь разное значение в зависимости от **контекстного ключа** — произвольной строки-идентификатора (обычно индекс игрока).

Перед вызовом `PCPH_Format` нужный контекст кладётся на стек группы через `PCPH_PushContext` / `PCPH_PushIntContext`, а после — снимается через `PCPH_PopContext`.

### Режимы хранения значений

| Режим | Когда использовать | Как обновлять |
|---|---|---|
| **Проактивный** | Значение известно заранее и меняется по событиям (например, authid при авторизации) | `PCPH_Set` / `PCPH_SetForStr` / `PCPH_SetForInt` и аналоги |
| **Реактивный (колбек)** | Значение нужно вычислять на лету в момент форматирования | `PCPH_SetGroupKeyCallback`, внутри колбека — `PCPH_Cb_Set` / `PCPH_Cb_SetInt` / `PCPH_Cb_SetFloat` |

---

## Жизненный цикл

```
plugin_init / plugin_precache
    └─ ParamsController_Init()
        ├─ регистрирует встроенные типы параметров
        ├─ регистрирует встроенные плейсхолдеры ({map}, {p:authid})
        ├─ ParamsController_OnRegisterTypes       ← пользовательские типы
        ├─ PCPH_OnRegisterGroups                  ← пользовательские группы и ключи
        └─ ParamsController_OnInited              ← можно использовать API
```

Регистрировать группы и ключи нужно в `PCPH_OnRegisterGroups`. После `OnInited` регистрация тоже работает, но семантически правильнее делать это в форварде.

---

## API

### Форвард

```pawn
forward PCPH_OnRegisterGroups();
```

Вызывается до `OnInited`. Здесь нужно регистрировать свои группы и ключи.

---

### Группы

```pawn
native T_PHGroup:PCPH_RegisterGroup(const sPrefix[]);
```
Регистрирует новую группу с уникальным префиксом. Возвращает хендлер группы.

```pawn
native T_PHGroup:PCPH_FindGroup(const sPrefix[]);
```
Ищет группу по префиксу. Возвращает `Invalid_PHGroup` если не найдена.

```pawn
native T_PHGroup:PCPH_GetGlobalGroup();
```
Возвращает хендлер глобальной группы (плейсхолдеры без префикса).

---

### Регистрация ключей

```pawn
native PCPH_RegisterGroupKey(const T_PHGroup:hGroup, const sKey[]);
```
Регистрирует ключ в группе без колбека (проактивный режим).

```pawn
native PCPH_SetGroupKeyCallback(const T_PHGroup:hGroup, const sKey[], const sCallback[]);
```
Устанавливает колбек для **уже зарегистрированного** ключа. Если ключ не зарегистрирован — ошибка.

Сигнатура колбека:
```pawn
public MyCallback(const sKey[], const sContextKey[]) {
    // sKey        — ключ плейсхолдера
    // sContextKey — текущий контекст группы
    PCPH_Cb_Set("значение");
}
```

Внутри колбека **обязательно** вызвать один из `PCPH_Cb_Set` / `PCPH_Cb_SetInt` / `PCPH_Cb_SetFloat`.

**Вспомогательные stocks:**

```pawn
// Регистрация + колбек в одном вызове (для группы)
stock PCPH_RegGroupKey(const T_PHGroup:hGroup, const sKey[], const sCallback[]);

// Регистрация + колбек в глобальной группе
stock PCPH_Gl_RegKey(const sKey[], const sCallback[]);

// Регистрация в глобальной группе без колбека
stock PCPH_Gl_RegisterKey(const sKey[]);

// Установка колбека в глобальной группе
stock PCPH_Gl_SetKeyCallback(const sKey[], const sCallback[]);
```

---

### Управление контекстом

```pawn
native PCPH_PushContext(const T_PHGroup:hGroup, const sContextKey[]);
native PCPH_PopContext(const T_PHGroup:hGroup);

// Удобная обёртка для числового контекста (например, индекс игрока)
stock PCPH_PushIntContext(const T_PHGroup:hGroup, const any:iContextKey);
```

Контекст — стековый: можно пушить несколько уровней и попать обратно. При `PCPH_Format` используется текущий верхний контекст группы.

---

### Установка значений внутри колбека (`PCPH_Cb_*`)

Можно вызывать **только** из колбека ключа:

```pawn
native PCPH_Cb_Set(const sValue[]);
native PCPH_Cb_SetInt(const any:iValue);
native PCPH_Cb_SetFloat(const Float:fValue);  // форматируется как "%.2f"
```

---

### Проактивная установка значений (вне колбека)

Три семейства сеттеров по типу контекстного ключа:

| Суффикс | Контекст | Пример |
|---|---|---|
| *(нет)* | пустая строка `""` | `PCPH_Set(hGroup, "key", "value")` |
| `ForStr` | явная строка | `PCPH_SetForStr(hGroup, "key", "ctx", "value")` |
| `ForInt` | число (конвертируется в строку) | `PCPH_SetForInt(hGroup, "key", id, "value")` |

```pawn
// Пустой контекст
stock PCPH_Set(const T_PHGroup:hGroup, const sKey[], const sValue[]);
stock PCPH_SetInt(const T_PHGroup:hGroup, const sKey[], const any:iValue);
stock PCPH_SetFloat(const T_PHGroup:hGroup, const sKey[], const Float:fValue);

// Явный строковый контекст
native PCPH_SetForStr(const T_PHGroup:hGroup, const sKey[], const sContextKey[], const sValue[]);
native PCPH_SetIntForStr(const T_PHGroup:hGroup, const sKey[], const sContextKey[], const any:iValue);
native PCPH_SetFloatForStr(const T_PHGroup:hGroup, const sKey[], const sContextKey[], const Float:fValue);

// Числовой контекст
stock PCPH_SetForInt(const T_PHGroup:hGroup, const sKey[], const any:iContextKey, const sValue[]);
stock PCPH_SetIntForInt(const T_PHGroup:hGroup, const sKey[], const any:iContextKey, const any:iValue);
stock PCPH_SetFloatForInt(const T_PHGroup:hGroup, const sKey[], const any:iContextKey, const Float:fValue);
```

---

### Форматирование строк

```pawn
native PCPH_Format(out[], const iOutLen, const sFormat[]);
```

Заменяет все плейсхолдеры вида `{key}` и `{prefix:key}` в строке `sFormat`. Незарегистрированные ключи и неизвестные группы сохраняются как есть. Максимальная длина результата — `PH_FORMAT_MAX_LEN` (4096).

---

## Константы

```pawn
#define PH_GROUP_PREFIX_MAX_LEN 16
#define PH_KEY_MAX_LEN 64
#define PH_VALUE_MAX_LEN 512
#define PH_CONTEXT_KEY_MAX_LEN 64
#define PH_CALLBACK_MAX_LEN 64
#define PH_FORMAT_MAX_LEN 4096
```

**Встроенные плейсхолдеры:**

| Плейсхолдер | Константы | Описание |
|---|---|---|
| `{map}` | `DEFAULT_PH_GLOBAL_MAP_KEY` | Текущая карта (проактивный, устанавливается при инициализации) |
| `{p:authid}` | `DEFAULT_PH_PLAYER_GROUP_PREFIX`, `DEFAULT_PH_PLAYER_AUTHID_KEY` | AuthID игрока (проактивный, устанавливается при авторизации) |

---

## Примеры использования

### Форматирование строки

```pawn
new out[256];
PCPH_Format(out, charsmax(out), "Карта: {map}, игрок: {p:authid}");
// При активном контексте игрока: "Карта: de_dust2, игрок: STEAM_0:1:12345"
```

### Форматирование с контекстом игрока

```pawn
new T_PHGroup:hPlayerGroup = PCPH_FindGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

PCPH_PushIntContext(hPlayerGroup, id);
new out[256];
PCPH_Format(out, charsmax(out), "Привет, {p:authid}!");
PCPH_PopContext(hPlayerGroup);
```

---

## Примеры реализации

### Реактивный плейсхолдер (колбек)

Значение вычисляется каждый раз при форматировании. Подходит для данных, которые меняются часто или дорого хранить.

```pawn
#include <amxmodx>
#include <ParamsController>

public PCPH_OnRegisterGroups() {
    // {time} в глобальной группе
    PCPH_Gl_RegKey("time", "PH_TimeCallback");

    // {sv:name} и {sv:players}
    new T_PHGroup:hGroup = PCPH_RegisterGroup("sv");
    PCPH_RegGroupKey(hGroup, "name", "PH_ServerCallback");
    PCPH_RegGroupKey(hGroup, "players", "PH_ServerCallback");
}

public PH_TimeCallback(const sKey[], const sContextKey[]) {
    static sTime[16];
    format_time(sTime, charsmax(sTime), "%H:%M");
    PCPH_Cb_Set(sTime);
}

public PH_ServerCallback(const sKey[], const sContextKey[]) {
    if (equal(sKey, "name")) {
        static sName[64];
        get_hostname(sName, charsmax(sName));
        PCPH_Cb_Set(sName);
    } else if (equal(sKey, "players")) {
        PCPH_Cb_SetInt(get_playersnum());
    }
}
```

### Проактивный плейсхолдер с контекстом

Значение устанавливается по событиям. Подходит для данных игрока, которые нужно читать часто.

```pawn
#include <amxmodx>
#include <ParamsController>

static T_PHGroup:g_hGroup = Invalid_PHGroup;

public PCPH_OnRegisterGroups() {
    g_hGroup = PCPH_RegisterGroup("p");
    PCPH_RegisterGroupKey(g_hGroup, "name");
    PCPH_RegisterGroupKey(g_hGroup, "kills");
}

public client_putinserver(id) {
    UpdatePlayerPlaceholders(id);
}

UpdatePlayerPlaceholders(id) {
    static sName[32];
    get_user_name(id, sName, charsmax(sName));
    PCPH_SetForInt(g_hGroup, "name", id, sName);

    new kills, deaths;
    get_user_frags(id, kills, deaths);
    PCPH_SetIntForInt(g_hGroup, "kills", id, kills);
}

FormatForPlayer(id, const sTemplate[], out[], outLen) {
    PCPH_PushIntContext(g_hGroup, id);
    PCPH_Format(out, outLen, sTemplate);
    PCPH_PopContext(g_hGroup);
}
```
