# Хелперы для своего типа (`PCSingle_*` / `PCGet_*`)

Для работы типа контроллеру достаточно колбека чтения. Но по конвенции к типу
пишут пару хелперов-обёрток, чтобы потребителю было удобно читать значение — как
у встроенных типов:

- `PCSingle_*` — прочитать **одно** значение типа прямо из JSON, без параметра;
- `PCGet_*` — прочитать готовое значение из **результата** (`Trie`).

## Процесс

1. **Определи вид значения.** Что колбек пишет в `Trie`: число/`cell` или строку.
   Для хендлеров (напр. `Regex`) — тоже `cell`, с приведением типа.
2. **`PCSingle_*`** (сырое значение) — обёртка над `PCSingle_Cell` (число) или
   `PCSingle_Str` (строка).
3. **`PCSingle_Obj*`** (из объекта по ключу) — обёртка над `PCSingle_ObjCell` /
   `PCSingle_ObjStr`; принимает `key`, `dotNot`, `orFail`.
4. **`PCGet_*`** (из результата) — обёртка над `PCGet_Cell` / `PCGet_Str`; для
   строки добавь `i`-вариант.
5. **Положи хелперы в публичный `.inc` плагина**, если API типа доступно другим
   плагинам.

## Числовое значение

```pawn
// из сырого значения
stock any:PCSingle_MyType(const JSON:valueJson, const any:def = 0, const orFailKey[] = "") {
    return PCSingle_Cell(valueJson, "MyType", def, orFailKey);
}

// из поля объекта по ключу
stock any:PCSingle_ObjMyType(const JSON:objectJson, const key[], const any:def = 0, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, "MyType", def, dotNot, orFail);
}

// из результата
stock any:PCGet_MyType(const Trie:p, const key[], const any:def = 0) {
    return PCGet_Cell(p, key, def);
}
```

## Строковое значение

```pawn
stock PCSingle_MyStr(const JSON:valueJson, out[], const outLen, const def[] = "", const orFailKey[] = "") {
    return PCSingle_Str(valueJson, "MyType", out, outLen, def, orFailKey);
}

stock PCSingle_iMyStr(const JSON:valueJson, const def[] = "", const orFailKey[] = "") {
    new out[PARAM_VALUE_MAX_LEN];
    PCSingle_Str(valueJson, "MyType", out, charsmax(out), def, orFailKey);
    return out;
}

stock PCSingle_ObjMyStr(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, "MyType", out, outLen, def, dotNot, orFail);
}

stock PCGet_MyStr(const Trie:p, const key[], out[], const outLen, const def[] = "") {
    return PCGet_Str(p, key, out, outLen, def);
}

stock PCGet_iMyStr(const Trie:p, const key[], const def[] = "") {
    return PCGet_iStr(p, key, def);
}
```

## Соглашение об именах

- `..._Obj...` — значение из **объекта** по ключу; без `Obj` — из **сырого** значения;
- `i`-префикс — возвращает временную строку;
- внутри указывается **имя своего типа** строкой (`"MyType"`).

## Замечания

- Хелперы **не обязательны** для работы типа — это удобство и единый стиль.
- `stock` компилируется только при использовании, поэтому держать их в `.inc` дёшево.
- Свои хелперы стоит задокументировать в скилле типов плагина — см. скилл
  `param-types-authoring`.
