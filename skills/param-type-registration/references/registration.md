# Регистрация типа

## 0. Инициализация контроллера

Перед работой плагин-потребитель вызывает:

```pawn
public plugin_init() {
    ParamsController_Init();
}
```

`ParamsController_Init()` проводит инициализацию: регистрирует встроенные типы,
затем вызывает форвард `ParamsController_OnRegisterTypes` (здесь регистрируются
свои типы), затем — `ParamsController_OnInited`. Пока init не выполнен, любой
вызов API аварийно завершается:

```
Attempt to interact with params before init them.
```

## 1. Форвард

Свои типы регистрируются **только** в обработчике форварда:

```pawn
public ParamsController_OnRegisterTypes() {
    // регистрация типов
}
```

К этому моменту встроенные типы уже зарегистрированы.

## 2. Способы регистрации

### Простой (обычно этого достаточно)

```pawn
public ParamsController_OnRegisterTypes() {
    ParamsController_RegSimpleType("MyType", "@OnReadMyType");
}
```

`ParamsController_RegSimpleType(name, callback)` регистрирует тип и сразу
назначает функцию чтения. Возвращает хендлер типа.

### Раздельный

```pawn
public ParamsController_OnRegisterTypes() {
    new T_ParamType:iType = ParamsController_ParamType_Register("MyType");
    ParamsController_ParamType_SetReadCallback(iType, "@OnReadMyType");
}
```

`ParamsController_ParamType_Register(name)` возвращает хендлер типа;
`ParamsController_ParamType_SetReadCallback(handle, callback)` назначает функцию
чтения. Раздельный стиль нужен, если хендлер типа пригодится позже.

## Ограничения

- Имя типа — до `PARAM_TYPE_NAME_MAX_LEN` (64) ячеек; тег — до `PARAM_TYPE_TAG_MAX_LEN` (64).
- Регистрировать можно только в рамках `ParamsController_OnRegisterTypes`.
- Не регистрируйте тип с уже занятым именем — перезапишете существующий.
- Функция чтения может лежать в любом файле плагина — она ищется по имени.
