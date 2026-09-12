# Resource

Путь к **существующему** ресурсу (любой файл, который нужно отдать клиенту).
При чтении помещается в прекеш (`precache_generic`).

> Общее для ресурсных типов (путь/имя, проверка существования) — `_resource-files.md`.

## Допустимые значения в JSON

Строка — путь относительно `cstrike/`.

```json
"sprites/weapon_ak47.txt"
```

## Что получится в Trie

- без тега — строка с путём, читается `PCGet_Str`/`PCGet_iStr`;
- с тегом `Resource:index` — **индекс прекеша** (`precache_generic`), читается `PCGet_Int`/`PCGet_Cell`.

## Когда будет ошибка

- файла по указанному пути нет на сервере;
- значение не строка.

## Примеры

```json
{
    "HudSprite": "sprites/weapon_ak47.txt",
    "MenuBanner": "gfx/vip_banner.tga"
}
```

Отличие от `Model`/`Sound` — произвольный файл и прекеш типа generic.
