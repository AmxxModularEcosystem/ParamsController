# Params Controller

Плагин предоставляет API для удобного чтения параметров различных типов из JSON-значений. Также, API позволяет регистрировать новые типы параметров.

## Требования

- AmxModX 1.9.0 или новее.

## Применение

Перед началом использования контроллера параметров необходимо вызвать натив для принудительной его инициализации:

```pawn
public plugin_init() {
    ParamsController_Init();
}
```

После инициализации контроллера параметров все типы будут гарантировано зарегистрированы и готовы для использования.

### Стандартный набор типов параметров

- `Boolean` - булевое значение true/false
- `Integer` - целое число
- `Float` - дробное число
- `String` - строка длиной до 512 ячеек
- `ShortString` - строка длинной до 64 ячеек
- `LongString` - строка длиной до 4096 ячеек
- `RGB` - цвет в одном из форматов:
    - `"RRR,GGG,BBB"`;
    - `"RRR GGG BBB"`;
    - `[RRR, GGG, BBB]`;
    - `{"R": RRR, "G": GGG, "B": BBB}`;
    - `{"Red": RRR, "Green": GGG, "Blue": BBB}`.
- `Model` - модель, которая будет помещена в прекеш (`precache_model`) при чтении параметра.
- `Sound` - звук, который будет помещен в прекеш (`precache_sound`) при чтении параметра.
- `Resource` - файл, который будет помещен в прекеш (`precache_generic`) при чтении параметра.
- `File` - существующий файл.
- `Dir` - существующая директория.

### Создание параметров

Для создания параметров используется натив `ParamsController_Param_Construct`:

```pawn
new T_Param:iParam = ParamsController_Param_Construct("Damage", "Float", true);
```

`"Damage"` - ключ параметра, `"Float"` - тип параметра, а третий параметр определяет обязательность параметра. Отсутствие указанного типа параметра в контроллере вызовет ошибку. Если Вы используете тип параметра не из стандартного набора, то позаботьтесь о том, чтобы конечный пользователь нашёл расширение с нужным типом.

Для чтения созданных параметров необходимо поместить их в динамический массив (`Array:`). Для примера создадим несколько параметров:

```pawn
Array:PrepareParams() {
    new Array:aParams = ArrayCreate(1, 1);

    ArrayPushCell(aParams, ParamsController_Param_Construct("Name", "ShortString", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("Damage", "Float", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("ModelV", "String", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("ModelP", "String", false));

    return aParams;
}
```

**ВАЖНО! Если одни и те же параметры требуется использовать несколько раз, то сохраняйте их куда-нибудь или кешируйте через static-переменные, чтоб не очищать и пересоздавать их каждый раз**

Пример кеширования:

```pawn
Array:PrepareParams() {
    static Array:aParams;
    if (aParams != Invalid_Array) {
        return aParams;
    }

    aParams = ArrayCreate(1, 1);

    ArrayPushCell(aParams, ParamsController_Param_Construct("Name", "ShortString", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("Damage", "Float", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("ModelV", "String", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("ModelP", "String", false));

    return aParams;
}
```

Такая функция при первом вызове создаст набор параметров и вернёт его, а при последующих вызовах будет возвращать тот же ранее созданный набор параметров. Главное не удалять этот набор функцией `ArrayDestroy`.

### Чтение параметров

После создания набора параметров можно с его помощью прочитать какой-нибудь JSON-обьект используя натив `ParamsController_Param_ReadList`.
Например, спрасим JSON-обьект из файла и прочитаем его созданным ранее набором параметров:

```pawn
Trie:ReadSomeFile(const sFilePath[]) {
    new JSON:jFile = json_parse(sFilePath, true);
    new Trie:tParamValues = ParamsController_Param_ReadList(PrepareParams(), jFile);
    json_free(jFile);

    return tParamValues;
}
```

Но в таком случае мы не узнаем, произошла ли в ходе чтения параметров какая-то ошибка. А ошибки могут быть следующими:
- Обязательный параметр отсутствует в переданном JSON-обьекте
- Читаемое значение параметра некорректно (Может возникнуть даже для необязательного параметра)

Если ошибка всё же произошла, то возвращённый из натива Trie будет содержать только те параметры, которые были прочитаны до возникновения ошибки.

Чтобы поймать ошибку в случае её возникновения, необходимо передать в натив `ParamsController_Param_ReadList` параметры, в которые вернутся тип ошибки и название параметра, при чтении которого она возникла:

```pawn
Trie:ReadSomeFile(const sFilePath[]) {
    new E_ParamsReadErrorType:iErrType;
    new sErrParamName[PARAM_KEY_MAX_LEN];

    new JSON:jFile = json_parse(sFilePath, true);
    new Trie:tParamValues = ParamsController_Param_ReadList(
        PrepareParams(), jFile,
        .iErrType = iErrType,
        .sErrParamName = sErrParamName,
        .iErrParamNameLen = charsmax(sErrParamName)
    );
    json_free(jFile);

    if (iErrType == ParamsReadError_None) {
        return tParamValues;
    }
    TrieDestroy(tParamValues);
    
    log_amx("[ERROR] Param '%s' is invalid or required but not presented.", sErrParamName);
    // Тут уже на Ваше усмотрение. Можно выводить разные сообщения в зависимости от типа ошибки.

    return Invalid_Trie;
}
```

### Получение набора параметров через нативы

Если Вам надо читать параметры, которые определены другим плагином, который отправляет Вам их через Ваш натив, то для получения этого набора параметров можно использовать функцию `ParamsController_Param_ListFromNativeParams`. Пример натива, принимающего набор параметров:

```pawn
new Array:g_aParams = Invalid_Array;

public plugin_natives() {
    register_native("Example_AddParams", "@_AddParams");
}

@_AddParams(const PluginId, const iParamsCount) {
    enum {Arg_Params = 1}

    g_aParams = ParamsController_Param_ListFromNativeParams(Arg_Params, iParamsCount, g_aParams);
}
```

Описанный натив будет добавлять переданные в него параметры в глобальный набор параметров. Параметры будут читаться начиная с первого параметра натива. Привер вызова подобного натива из стороннего плагина:

```pawn
Example_AddParams(
    "Name", "ShortString", true,
    "Damage", "Float", true,
    "ModelV", "String", true,
    "ModelP", "String", false
);
```

Структура параметра аналогична параметрам натива `ParamsController_Param_Construct`, а значит:
- Первое значение определяет ключ параметра, по которому он будет читаться из JSON-обьекта и заноситься в итогоывй Trie.
- Второе значение определяет тип параметра.
- Третье значение определяет обязательность параметра.

## Создание типов параметров

Помимо стандартных типов параметров контроллер позволяет регистрировать свои типы.

### Регистрация нового типа параметров

Регистрация типов производится **обязательно** в рамках форварда `ParamsController_OnRegisterTypes`. Для регистрации типа параметров используется натив `ParamsController_ParamType_Register`, в который передаётся название регистрируемого типа:

```pawn
new const PARAM_TYPE_NAME[] = "ExampleType";

public ParamsController_OnRegisterTypes() {
    new T_ParamType:iParamType = ParamsController_ParamType_Register(PARAM_TYPE_NAME);
}
```

Натив возвращает хендлер зарегистрированного типа, который далее нужно будет использовать для установки функции-обработчика для чтения значения параметра:

```pawn
new const PARAM_TYPE_NAME[] = "ExampleType";

public ParamsController_OnRegisterTypes() {
    new T_ParamType:iParamType = ParamsController_ParamType_Register(PARAM_TYPE_NAME);
    ParamsController_ParamType_SetReadCallback(iParamType, "@OnReadParam");
}
```

Также, можно сократить подобную запись до вызова одной функции `ParamsController_RegSimpleType`, которая регистрирует новый тип и сразу устанавливает ему функцию-обработчик:

```pawn
new const PARAM_TYPE_NAME[] = "ExampleType";

public ParamsController_OnRegisterTypes() {
    ParamsController_RegSimpleType(PARAM_TYPE_NAME, "@OnReadParam");
}
```

### Обработка чтения значения параметра

Выше, при регистрации нового типа, была указана функция `@OnReadParam`. Она должна принимать следующие параметры:
- JSON-значение читаемого параметра.
- Гарантированно валидный Trie-хендлер.
- Ключ читаемого параметра.

Также, функция должна вернуть `true`, если параметр был прочитан успешно, или `false`, если JSON-значение некорректно для данного типа.

```pawn
@OnReadParam(const JSON:jValue, const Trie:tParams, const sParamKey[]) {
    // *чтение чего-то из jValue*
}
```

После прочтения значения Ваша функция должна записать его в Trie с параметрами. Сделать это можно двумя способами: напрямую или через специальные нативы.

Для записи напрямую достаточно при помощи обычных нативов для Trie записать значение в `tParams` по ключу `sParamKey`:

```pawn
@OnReadParam(const JSON:jValue, const Trie:tParams, const sParamKey[]) {
    return TrieSetCell(tParams, sParamKey, json_get_number(jValue));
}
```

Через специальные нативы можно записать только число или строку. При этом, максимально возможная длина строки в таком случае - 4096 ячеек.

```pawn
// Число
@OnReadParam(const JSON:jValue, const Trie:tParams, const sParamKey[]) {
    return ParamsController_SetCell(json_get_number(jValue));
}

// Строка
@OnReadParam(const JSON:jValue, const Trie:tParams, const sParamKey[]) {
    return ParamsController_SetString("some-string");
}
```

Оба натива возвращают `true` при успешной записи значения.

### Примеры регистрации типов параметров

Полноценные примеры регистрации типов можно посмотреть в файле `amxmodx/scripting/ParamsController/Core/DefaultParamTypes.inc`.
