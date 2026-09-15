---
name: param-type-registration
description: >-
  Как плагину зарегистрировать собственный тип параметра в ParamsController (AMXX):
  форвард ParamsController_OnRegisterTypes, ParamsController_RegSimpleType /
  ParamsController_ParamType_Register + ParamsController_ParamType_SetReadCallback,
  контракт колбека чтения bool:(const JSON, const Trie, const key[], const tag[]),
  запись значения (ParamsController_SetCell/SetString или Trie напрямую), теги,
  условия ошибки, а также создание хелперов чтения PCSingle_*/PCGet_* для своего
  типа. Использовать, когда плагин добавляет свой тип параметра.
  Триггеры: регистрация типа параметра, custom param type,
  ParamsController_OnRegisterTypes, ParamsController_RegSimpleType,
  ParamsController_ParamType_Register, PCSingle helper, PCGet helper,
  read callback, register parameter type.
  Use when a plugin adds its own ParamsController parameter type.
---

# Регистрация своих типов параметров

Как плагину добавить **собственный тип** параметра в ParamsController: объявить
имя типа, повесить на него функцию чтения и записывать результат в `Trie`.

Встроенные типы (`Boolean`, `Integer`, `Model`, …) уже зарегистрированы — их
описывать не надо. Здесь — про свои.

## Когда применять

- плагину нужен нестандартный формат значения параметра;
- нужно, чтобы конфиг плагина понимал свой тип.

Если нужен просто доступ к JSON без параметра — это другой скилл
(`params-usage`, `PCSingle_*`), регистрировать тип не обязательно.

## Коротко

1. `ParamsController_Init()` в `plugin_init()` — инициализация контроллера.
2. В `public ParamsController_OnRegisterTypes()` зарегистрировать тип:
   - `ParamsController_RegSimpleType("MyType", "@OnReadMyType")` — обычный путь;
   - либо `ParamsController_ParamType_Register("MyType")` +
     `ParamsController_ParamType_SetReadCallback(iType, "@OnReadMyType")`.
3. Написать колбек `bool:@OnReadMyType(const JSON:jValue, const Trie:tParams,
   const sParamKey[], const sParamTag[])`, который разбирает `jValue`, пишет
   результат в `tParams` и возвращает `true`/`false`.

## Детали

- `references/registration.md` — способы регистрации, тайминг, ограничения.
- `references/read-callback.md` — контракт колбека, запись значения, теги, ошибки.
- `references/helpers.md` — как написать свои хелперы `PCSingle_*` / `PCGet_*`.
- `references/examples.md` — полный пример плагина.

## Ключевые правила

- Регистрировать типы — строго в `ParamsController_OnRegisterTypes`.
- Пока `ParamsController_Init()` не вызван, любой вызов API аварийно падает.
- Колбек **обязан** записать значение в `Trie` — иначе контроллер сообщит об ошибке.
- `return false` = «значение невалидно для этого типа».
- Тег (`Тип:тег`) приходит в колбек четвёртым аргументом.
- К типу по конвенции пишутся хелперы `PCSingle_*` / `PCGet_*` (см. `helpers.md`) —
  чтобы потребитель читал значение единообразно с встроенными типами.
