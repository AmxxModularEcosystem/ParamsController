# Чтение результата (`PCGet_*`)

Оба кейса дают `Trie`, где ключ = ключ параметра (`PCSingle_*` кладёт значение
под служебный ключ `__SINGLE`). Значение из результата читается геттерами
`PCGet_*`. Они не падают: если ключа нет или `Trie` невалиден, возвращается
значение по умолчанию.

Базовые геттеры (все принимают `p`, `key` первыми аргументами):

- `PCGet_Cell(p, key, def)` — универсальный, для числовых значений (`any`);
- `PCGet_Int(p, key, def)` — целое;
- `PCGet_Str(p, key, out[], outLen, def)` — строка в буфер (возвращает длину);
- `PCGet_iStr(p, key, def)` — строка как временный буфер.

Геттеры для конкретных типов (float, bool, цвет, regexp, сообщение чата и т.п.)
описаны в скилле `param-types` — в файле соответствующего типа, в разделе
«Что получится в Trie».

## Пример

```pawn
new Trie:t = ParamsController_Param_ReadList(GetEntityParams(), jObj);

new damage = PCGet_Int(t, "Damage");
new name[32];
PCGet_Str(t, "Name", name, charsmax(name));
```
