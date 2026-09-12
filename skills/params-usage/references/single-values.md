# Кейс 1. Одно значение из JSON (`PCSingle_*`)

`PCSingle_*` — типизированные обёртки над `json_object_get_value` / `json_get_*`:
читают **одно** значение заданного типа и применяют правила этого типа
(преобразования, теги, проверки). Регистрировать параметр не нужно.

## Два входа

**Из сырого JSON-значения:**

```pawn
new value = PCSingle_Cell(valueJson, DEFAULT_PARAMS_INT_NAME);
```

**Из поля JSON-объекта по ключу:**

```pawn
new value = PCSingle_ObjInt(objectJson, "Damage");
```

## Общие функции

- `PCSingle_Cell(valueJson, type, def, orFailKey)` — «сырое» значение;
- `PCSingle_Str(valueJson, type, out[], outLen, def, orFailKey)` — строка;
- `PCSingle_iStr(valueJson, type, def, orFailKey)` — строка как временный буфер;
- `PCSingle_Read(valueJson, type, orFailKey)` → `Trie` (значение под ключом `__SINGLE`);
- `PCSingle_ReadFromObject(objectJson, key, type, dotNot, orFail)` → `Trie`
  (или `Invalid_Trie`);
- `PCSingle_ObjCell` / `PCSingle_ObjStr` / `PCSingle_iObjStr` — то же, но из объекта.

## Типизированные обёртки

Имена по шаблону `PCSingle_[Obj]<Type>` (вариант с `Obj` читает из объекта по
ключу, без `Obj` — из сырого значения):

- `PCSingle_ObjInt`, `PCSingle_ObjFloat`, `PCSingle_ObjBool` — из объекта по ключу;
  для сырого значения — `PCSingle_Cell(valueJson, DEFAULT_PARAMS_*_NAME, …)`;
- `PCSingle_String` / `..._ObjString` (+ `ShortString`, `LongString`);
- `PCSingle_Model`, `PCSingle_PlayerModel`, `PCSingle_Sound`, `PCSingle_Resource`,
  `PCSingle_File`, `PCSingle_Dir`, `PCSingle_ChatMessage` (+ `Obj`-варианты);
- `PCSingle_RGB` / `PCSingle_ObjRGB` — пишут в `out[3]`;
- `PCSingle_Time`, `PCSingle_TimeInterval`, `PCSingle_WeekDay`, `PCSingle_Flags`
  (+ `Obj`-варианты) — возвращают число;
- `PCSingle_Regexp` / `PCSingle_ObjRegexp` — возвращают `Regex`.

Строковые `-i`-варианты (`PCSingle_iStr`, `PCSingle_iObjString`) возвращают
временную строку из шаблонного буфера — удобно передавать сразу в вызов.

## Ошибки и значения по умолчанию

- `orFail` / `orFailKey` — писать ли ошибку при отсутствии/невалидности.
- Если поля нет, значение невалидно или **тип не зарегистрирован** — обёртка
  вернёт `def` (значение по умолчанию), а не упадёт.
- `PCSingle_ReadFromObject`: значение не объект → `Invalid_Trie`; нет ключа →
  `Invalid_Trie` (при `orFail` пишет ошибку).

## `dotNot`

`PCSingle_ReadFromObject(..., .dotNot = true)` включает вложенные ключи через
точку: например `"Sound/Volume"`.

## Известная проблема сигнатур

В текущем `ParamsController.inc` обёртки для **сырых** чисел `PCSingle_Int`,
`PCSingle_Float`, `PCSingle_Bool` имеют рассинхронизированную сигнатуру: они
передают в `PCSingle_Cell` лишний аргумент `key` (5 аргументов вместо 4), поэтому
при использовании не компилируются. Пока используйте:

- из объекта — `PCSingle_ObjInt` / `PCSingle_ObjFloat` / `PCSingle_ObjBool`;
- из сырого значения — `PCSingle_Cell(valueJson, DEFAULT_PARAMS_INT_NAME, …)`
  (и аналогично для `Float` / `Bool`).

## Когда `PCSingle_*`, а когда параметры

- одно значение «здесь и сейчас» → `PCSingle_*`;
- повторяющийся структурированный конфиг с обязательностью и обработкой ошибок →
  параметры (`entity-params.md`).
