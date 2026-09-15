#if defined __pc_src_phparse_included
    #endinput
#endif
#define __pc_src_phparse_included

#include <amxmodx>
#include <ParamsController>

// Общие утилиты парсинга плейсхолдеров вида {key} / {prefix:key}.
// Используются как инлайн-форматированием (PHGroup_FormatString), так и
// компиляцией шаблонов (PHTemplate_Compile), чтобы логика не дублировалась.

// Максимальная длина содержимого плейсхолдера "prefix:key" (без фигурных скобок).
// Содержимое длиннее этого значения не является валидным плейсхолдером
// и должно выводиться как есть (литеральный текст, без обрезки).
#define PH_PLACEHOLDER_CONTENT_MAX_LEN (PH_GROUP_PREFIX_MAX_LEN + PH_KEY_MAX_LEN + 1)

// Ищет закрывающую '}' для '{' на позиции openPos.
// @return true — найдена (closePos — позиция '}'); false — скобка не закрыта.
stock bool:PHParse_FindClosing(const src[], const srcLen, const openPos, &closePos) {
    closePos = openPos + 1;
    while (closePos < srcLen && src[closePos] != '}') {
        ++closePos;
    }
    return closePos < srcLen;
}

// Разбирает содержимое плейсхолдера между '{' (openPos) и '}' (closePos):
// извлекает текст и разбивает на "prefix:key" (без ':' — ключ глобальной группы).
//
// @return true  — содержимое валидно; prefix и key заполнены
//                 (пустой prefix = глобальная группа);
//         false — содержимое длиннее PH_PLACEHOLDER_CONTENT_MAX_LEN,
//                 это не плейсхолдер (вызывающий должен вывести его как есть).
stock bool:PHParse_Placeholder(const src[], const openPos, const closePos, prefix[], const prefixLen, key[], const keyLen) {
    new contentLen = closePos - openPos - 1;
    if (contentLen > PH_PLACEHOLDER_CONTENT_MAX_LEN) {
        return false;
    }

    new content[PH_PLACEHOLDER_CONTENT_MAX_LEN + 1];
    for (new k = 0; k < contentLen; ++k) {
        content[k] = src[openPos + 1 + k];
    }
    content[contentLen] = EOS;

    new colon = -1;
    for (new k = 0; k < contentLen; ++k) {
        if (content[k] == ':') {
            colon = k;
            break;
        }
    }

    if (colon < 0) {
        prefix[0] = EOS;
        copy(key, keyLen, content);
    } else {
        new copyLen = min(colon, prefixLen);
        for (new k = 0; k < copyLen; ++k) {
            prefix[k] = content[k];
        }
        prefix[copyLen] = EOS;
        copy(key, keyLen, content[colon + 1]);
    }

    return true;
}
