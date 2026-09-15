# Пример: свои типы

Полный плагин с двумя типами: скаляр `Percent` и `Duration` с тегом `minutes`.

```pawn
#include <amxmodx>
#include <json>
#include <ParamsController>

public plugin_init() {
    ParamsController_Init();
}

public ParamsController_OnRegisterTypes() {
    ParamsController_RegSimpleType("Percent", "@OnReadPercent");
    ParamsController_RegSimpleType("Duration", "@OnReadDuration");
}

// Число 0..100.
bool:@OnReadPercent(const JSON:jValue, const Trie:tParams, const sParamKey[]) {
    if (!json_is_number(jValue)) {
        return false;
    }

    new value = json_get_number(jValue);
    if (value < 0 || value > 100) {
        return false;
    }

    return ParamsController_SetCell(value);
}

// Число; тег "minutes" трактует его как минуты.
bool:@OnReadDuration(const JSON:jValue, const Trie:tParams, const sParamKey[], const sParamTag[]) {
    if (!json_is_number(jValue)) {
        return false;
    }

    new value = json_get_number(jValue);
    if (equali(sParamTag, "minutes")) {
        value *= 60;
    }

    return ParamsController_SetCell(value);
}
```

Использование в параметрах:

```pawn
ParamsController_Param_Construct("Chance", "Percent", true);
ParamsController_Param_Construct("KickTime", "Duration:minutes", true);
```

## Что тут важно

- оба типа зарегистрированы в `ParamsController_OnRegisterTypes`;
- оба колбека записывают значение и возвращают `bool`;
- `Percent` пишет ячейку, `Duration` учитывает тег;
- `ParamsController_Init()` вызван до любого использования API.
