# Кейс 2 (продолжение). Наборы параметров через нативы

Один плагин объявляет параметры, другой принимает их через натив. Набор
передаётся аргументами натива, а провайдер собирает его через
`ParamsController_Param_ListFromNativeParams`.

## Потребитель (передаёт параметры)

Три стиля, можно смешивать в одном вызове.

**Классический** — три аргумента на параметр:

```pawn
Example_AddParams(
    "Name", "ShortString", true,
    "Damage", "Float", true
);
```

**`PCParam()`** — один аргумент на параметр (компактнее):

```pawn
Example_AddParams(
    PCParam("Name", "ShortString", true),
    PCParam("Damage", "Float", true)
);
```

**`PCParams()`** — упаковать несколько `PCParam()` в один аргумент:

```pawn
Example_AddParams(
    PCParams(
        PCParam("Name", "ShortString", true),
        PCParam("Damage", "Float", true)
    )
);
```

`PCParam` / `PCParams` возвращают закодированное значение, которое передаётся в
натив как **обычный строковый аргумент**; это обёртки над
`ParamsController_Param_Construct`.

## Провайдер (принимает параметры)

```pawn
new Array:g_aParams = Invalid_Array;

public plugin_natives() {
    register_native("Example_AddParams", "@_AddParams");
}

@_AddParams(const PluginId, const iParamsCount) {
    enum { Arg_Params = 1 }

    g_aParams = ParamsController_Param_ListFromNativeParams(Arg_Params, iParamsCount, g_aParams);
}
```

- `Arg_Params` — номер первого аргумента натива;
- `iParamsCount` — общее число аргументов (первый параметр обработчика);
- `aAppend` — массив, куда дописывать (по умолчанию создаётся новый).

`ParamsController_Param_ListFromNativeParams` должна вызываться **только внутри
обработки натива**. Она понимает все три стиля и позволяет смешивать их.

## `PCParamsArray()`

Если нужен сразу `Array`, а не аргумент натива:

```pawn
new Array:a = PCParamsArray(
    PCParam("Name", "ShortString", true),
    PCParam("Damage", "Float", true)
);
```

## Важно

- `PCParam()` по умолчанию делает параметр **необязательным**
  (`required = false`), тогда как `ParamsController_Param_Construct` —
  **обязательным** (`required = true`). Не путайте.
- Полученный массив принадлежит провайдеру: освобождайте его
  `ParamsController_Param_FreeList`, когда список больше не нужен (кешированные
  наборы не освобождайте).
