---
name: param-types
description: >-
  Справочник по встроенным типам параметров ParamsController (AMXX-плагин чтения
  параметров из JSON): какие JSON-значения допустимы для каждого типа, что
  контроллер запишет в Trie, какие есть теги и каким геттером значение читается
  в коде. Только специфика типов. Использовать, когда нужно заполнить или
  проверить JSON-значение параметра известного типа: Boolean, Integer, Float,
  ShortString, String, LongString, RGB, Model, PlayerModel, Sound, Resource, File,
  Dir, ChatMessage, Flags, Time, TimeInterval, WeekDay, Regexp. Триггеры:
  ParamsController, типы параметров, JSON-параметр, значение параметра, параметры
  плагина, param types, plugin config value. Use when configuring JSON values for
  ParamsController parameter types.
---

# Встроенные типы параметров ParamsController

Справочник по **типам параметров**: что писать в JSON и что из этого попадёт в
`Trie` (и каким геттером читается). Каждый тип — отдельный файл в `references/`;
открывай только нужные.

Общие механизмы ParamsController — регистрация параметров, синтаксис тегов,
чтение значений кодом, ошибки чтения — здесь намеренно **не** описываются:
смотри скиллы «Param type registration» и «Params usage». Здесь — только
специфика конкретных типов.

## Типы

| Тип (имя в конфиге) | Что это | Файл |
|---|---|---|
| `Boolean` | логическое `true`/`false` (+ строковые и числовые варианты) | `references/boolean.md` |
| `Integer` | целое число | `references/integer.md` |
| `Float` | число с плавающей точкой | `references/float.md` |
| `ShortString` | строка до 64 ячеек | `references/short-string.md` |
| `String` | строка до 512 ячеек | `references/string.md` |
| `LongString` | строка до 4096 ячеек | `references/long-string.md` |
| `RGB` | цвет (R, G, B) | `references/rgb.md` |
| `Model` | существующая модель + прекеш | `references/model.md` |
| `PlayerModel` | имя модели игрока (`models/player/<name>/<name>.mdl`) | `references/player-model.md` |
| `Sound` | существующий звук (`sound/...`) + прекеш | `references/sound.md` |
| `Resource` | существующий ресурс + прекеш (`precache_generic`) | `references/resource.md` |
| `File` | существующий файл (без прекеша) | `references/file.md` |
| `Dir` | существующая директория | `references/dir.md` |
| `ChatMessage` | строка чата до 190 ячеек с цветами `^1`/`^3`/`^4` | `references/chat-message.md` |
| `Flags` | флаги доступа AMXX → битовая маска | `references/flags.md` |
| `Time` | время суток `HH[:MM[:SS]]` → секунды от полуночи | `references/time.md` |
| `TimeInterval` | интервал (`1d12h30i30`) → секунды | `references/time-interval.md` |
| `WeekDay` | день недели (рус/англ) → номер `0..6` | `references/week-day.md` |
| `Regexp` | регулярное выражение → скомпилированный хендлер | `references/regexp.md` |

## Общее для нескольких типов

Блоки, одинаковые для группы типов (отличается только имя/параметр типа):

- `references/_resource-files.md` — `Model`, `PlayerModel`, `Sound`, `Resource`,
  `File`, `Dir`: путь/имя и проверка существования.
- `references/_strings.md` — `ShortString`, `String`, `LongString`: строки и лимиты.
- `references/_numbers.md` — `Integer`, `Float`: разбор чисел.
