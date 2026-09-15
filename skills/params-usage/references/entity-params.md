# Кейс 2. Параметры сущности

Объявить набор параметров и прочитать им JSON-объект. Так делается конфиг
сущностей: плагин один раз строит список параметров, дальше применяет его к
каждому объекту.

## 1. Создать параметры

```pawn
new T_Param:p = ParamsController_Param_Construct("Damage", "Float", true);
```

- `"Damage"` — ключ: по нему значение читается из JSON и пишется в результат;
- `"Float"` — имя типа; может содержать тег: `"Model:index"`;
- третий аргумент — обязательность (по умолчанию `true`).

Если тип не зарегистрирован: обязательный параметр логирует ошибку, необязательный
при чтении молча пропускается.

## 2. Собрать в массив

```pawn
new Array:aParams = ArrayCreate(1, 1);
ArrayPushCell(aParams, ParamsController_Param_Construct("Name", "ShortString", true));
ArrayPushCell(aParams, ParamsController_Param_Construct("Damage", "Float", true));
ArrayPushCell(aParams, ParamsController_Param_Construct("Model", "String", false));
```

> Набор следует **кешировать** (`static`), а не пересоздавать каждый раз.
> Кешированный массив не удаляйте.

## 3. Прочитать объект

```pawn
new E_ParamsReadErrorType:iErrType;
new sErrParam[PARAM_KEY_MAX_LEN];

new Trie:tParams = ParamsController_Param_ReadList(
    aParams, jObj,
    .iErrType = iErrType,
    .sErrParamName = sErrParam,
    .iErrParamNameLen = charsmax(sErrParam)
);
```

- `jObj` должен быть JSON-объектом;
- результат — `Trie` прочитанных значений;
- при ошибке `iErrType` содержит код, `sErrParam` — ключ виновника; в `Trie`
  попадут только параметры, прочитанные **до** ошибки.

Коды `E_ParamsReadErrorType`:

| Код | Когда |
|---|---|
| `ParamsReadError_None` | всё хорошо |
| `ParamsReadError_RequiredParamNotPresented` | нет обязательного поля |
| `ParamsReadError_ParamValueIsInvalid` | значение невалидно (бывает и для необязательного, если поле присутствует) |
| `ParamsReadError_UnknownParamType` | тип обязательного параметра не зарегистрирован |

## 4. Читать результат

Результат — `Trie`; значения из него читаются геттерами `PCGet_*` — см.
`reading-result.md`.

## Один параметр отдельно

```pawn
new Trie:t = TrieCreate();
ParamsController_Param_Read(param, valueJson, t, "Key");
```

## Освобождение

- одиночный — `ParamsController_Param_Free(param)` (заменяет хендлер на `Invalid_Param`);
- список — `ParamsController_Param_FreeList(aParams)` (освобождает все и уничтожает массив).

Кешированные наборы не освобождайте.

## Пример: кеш набора

```pawn
Array:GetEntityParams() {
    static Array:aParams;
    if (aParams != Invalid_Array) {
        return aParams;
    }

    aParams = ArrayCreate(1, 1);
    ArrayPushCell(aParams, ParamsController_Param_Construct("Name", "ShortString", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("Damage", "Float", true));
    ArrayPushCell(aParams, ParamsController_Param_Construct("Color", "RGB", false));

    return aParams;
}
```
