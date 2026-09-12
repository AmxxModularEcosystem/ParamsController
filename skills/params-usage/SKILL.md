---
name: params-usage
description: >-
  Как читать параметры ParamsController в коде плагина-потребителя. Два кейса:
  PCSingle_* как типизированные обёртки над json_object_get_value/json_get_* для
  одного значения без регистрации параметра; регистрация параметров сущностей
  (ParamsController_Param_Construct + ParamsController_Param_ReadList, чтение
  результата через PCGet_*), включая приём наборов через нативы
  (ParamsController_Param_ListFromNativeParams и новый стиль PCParam()/PCParams()).
  Использовать, когда нужно прочитать параметры из JSON в плагине. Триггеры:
  PCSingle, PCGet, ParamsController_Param_Construct, Param_ReadList,
  ListFromNativeParams, PCParam, PCParams, читать параметры, consume params.
  Use when consuming ParamsController parameters in plugin code.
---

# Использование параметров (код плагина)

Как читать параметры в плагине-потребителе. Два кейса:

1. **Разовое значение из JSON** — `PCSingle_*` как типизированные обёртки над
   `json_*`, без регистрации параметра. → `references/single-values.md`
2. **Параметры сущности** — объявить набор параметров и прочитать им JSON-объект
   (`ParamsController_Param_Construct` + `ParamsController_Param_ReadList`),
   затем читать результат геттерами `PCGet_*`. Плюс приём наборов от других
   плагинов через нативы (`ListFromNativeParams`, `PCParam`).
   → `references/entity-params.md`, `references/native-params.md`,
   `references/reading-result.md`

Перед использованием — `ParamsController_Init()` в `plugin_init()`. Имена типов
берутся из скилла `param-types` (встроенные) или из скиллов конкретных плагинов
(их собственные типы).

## Когда какой кейс

| Задача | Инструмент |
|---|---|
| Достать одно значение из своего JSON | `PCSingle_*` (`single-values.md`) |
| Конфиг сущности: несколько полей, обязательность, ошибки | параметры (`entity-params.md`) |
| Набор параметров приходит от другого плагина через натив | `native-params.md` |

## Результат — это `Trie`

Оба кейса в итоге дают `Trie`, где ключ = ключ параметра. `PCSingle_*` кладёт
значение под служебный ключ `__SINGLE`. Значения из результата читаются геттерами
`PCGet_*` (см. `reading-result.md`).
