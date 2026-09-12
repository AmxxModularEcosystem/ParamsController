# Колбек чтения типа

Функция, которая разбирает JSON-значение параметра и пишет результат в `Trie`.

## Сигнатура

```pawn
bool:@OnReadMyType(const JSON:jValue, const Trie:tParams, const sParamKey[], const sParamTag[])
```

| Аргумент | Что это |
|---|---|
| `jValue` | JSON-значение параметра (то, что пришло из конфига) |
| `tParams` | гарантированно валидный `Trie`, куда писать результат |
| `sParamKey` | ключ параметра — под ним нужно записать значение |
| `sParamTag` | тег после `:` в имени типа (`Тип:тег`), иначе пусто |

## Возврат

- `true` — значение прочитано и записано;
- `false` — значение некорректно для этого типа (параметр считается невалидным).

## Запись результата

Три способа:

1. `ParamsController_SetCell(value)` — число / float / bool / хендлер;
2. `ParamsController_SetString(str)` — строка (до 4096 ячеек);
3. напрямую в `tParams` по `sParamKey`: `TrieSetCell`, `TrieSetString`,
   `TrieSetArray`.

`SetCell` / `SetString` работают **только внутри колбека чтения** (иначе аварийное
завершение `Attempt to set param value outside the read callback.`) и не требуют
ключа — контроллер знает текущий параметр.

> Колбек **обязан** записать значение под своим ключом. Если вернуть `true`, ничего
> не записав, контроллер сообщит об ошибке вида «тип не пишет значение в trie».

## Разбор входа

- `json_get_type(jValue)` → `JSONNumber` / `JSONString` / `JSONBoolean` /
  `JSONArray` / `JSONObject` / `JSONNull`;
- `json_is_array` / `json_is_object` / `json_is_string` / `json_is_number` / `json_is_bool`;
- `json_get_number` / `json_get_real` / `json_get_bool` / `json_get_string`;
- массивы/объекты: `json_array_get_count`, `json_array_get_number`,
  `json_object_has_value`, `json_object_get_number` и т.п.

## Теги

Тег приходит в `sParamTag`; пустой тег (`sParamTag[0] == EOS`) — обычный режим.

```pawn
bool:@OnReadDuration(const JSON:jValue, const Trie:tParams, const sParamKey[], const sParamTag[]) {
    new value = json_get_number(jValue);
    if (equali(sParamTag, "minutes")) {
        value *= 60;
    }
    return ParamsController_SetCell(value);
}
```

Такой тип объявляется как `Duration` или `Duration:minutes`.

## Ошибки и мягкие преобразования

- Всё, что не должно приниматься, — `return false`.
- Осторожно с `str_to_num` / `str_to_float`: нечисловая строка даёт `0` **без**
  ошибки. Если «мусор» должен быть ошибкой — проверяйте вручную
  (`json_get_type`, `is_strnum` и т.п.).
- Некорректный вход можно залогировать: `PCJson_LogForFile(jValue, "WARNING", "...")`.
