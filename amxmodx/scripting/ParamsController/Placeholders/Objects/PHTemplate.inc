#if defined __pc_src_phtemplate_included
    #endinput
#endif
#define __pc_src_phtemplate_included

#include <amxmodx>
#include <ParamsController>
#include "ParamsController/Placeholders/Objects/PHGroup"
#include "ParamsController/Placeholders/PHParse"

enum E_PHTemplatePartType {
    PHTemplatePartType_Text,
    PHTemplatePartType_Placeholder,
}

enum _:S_PHTemplatePart {
    E_PHTemplatePartType:PHTemplatePart_Type,           // тип части шаблона
    PHTemplatePart_Content[PH_VALUE_MAX_LEN],           // текст сегмента или ключ плейсхолдера
    T_PHGroup:PHTemplatePart_Group,                     // хендлер группы (только для Placeholder)
    PHTemplatePart_Forward,                             // хендлер форварда (INVALID_HANDLE если проактивный)
}

static PHTemplate_AddTextPart(Array:parts, const text[], const textLen) {
    static partData[S_PHTemplatePart];
    partData[PHTemplatePart_Type] = PHTemplatePartType_Text;

    new copyLen = min(textLen, charsmax(partData[PHTemplatePart_Content]));
    for (new k = 0; k < copyLen; ++k) {
        partData[PHTemplatePart_Content][k] = text[k];
    }
    partData[PHTemplatePart_Content][copyLen] = EOS;

    partData[PHTemplatePart_Group] = Invalid_PHGroup;
    partData[PHTemplatePart_Forward] = INVALID_HANDLE;
    ArrayPushArray(parts, partData);
}

// Компилирует строку с плейсхолдерами в предварительно разобранный шаблон.
// Кеширует хендлеры групп и форвардов, чтобы при каждом PCPH_FormatTemplate
// не выполнялись сканирование строки, парсинг префикса/ключа и Trie-поиск группы/форварда.
// Вызывать следует после завершения регистрации всех групп, ключей и колбеков.
T_PHTemplate:PHTemplate_Compile(const src[]) {
    new Array:parts = ArrayCreate(S_PHTemplatePart, 4);

    new srcLen = strlen(src);
    new inPos = 0;

    static text[PH_FORMAT_MAX_LEN];
    new textLen = 0;

    static prefix[PH_GROUP_PREFIX_MAX_LEN];
    static key[PH_KEY_MAX_LEN];

    while (inPos < srcLen) {
        if (src[inPos] != '{') {
            if (textLen < charsmax(text)) {
                text[textLen++] = src[inPos];
            }
            ++inPos;
            continue;
        }

        new closing;
        if (!PHParse_FindClosing(src, srcLen, inPos, closing)) {
            // Незакрытая скобка — выводим как есть
            if (textLen < charsmax(text)) {
                text[textLen++] = src[inPos];
            }
            ++inPos;
            continue;
        }

        if (textLen > 0) {
            PHTemplate_AddTextPart(parts, text, textLen);
            textLen = 0;
        }

        new bool:isPlaceholder = PHParse_Placeholder(src, inPos, closing, prefix, charsmax(prefix), key, charsmax(key));

        new T_PHGroup:group = Invalid_PHGroup;
        if (isPlaceholder) {
            group = PHGroup_ResolveByPrefix(prefix);
        }

        if (isPlaceholder && group != Invalid_PHGroup && PHGroup_IsKeyRegistered(group, key)) {
            static partData[S_PHTemplatePart];
            partData[PHTemplatePart_Type] = PHTemplatePartType_Placeholder;
            copy(partData[PHTemplatePart_Content], charsmax(partData[PHTemplatePart_Content]), key);
            partData[PHTemplatePart_Group] = group;
            partData[PHTemplatePart_Forward] = PHGroup_GetKeyForward(group, key);
            ArrayPushArray(parts, partData);
        } else {
            // Слишком длинный, неизвестная группа или незарегистрированный ключ —
            // фиксируем как текст с фигурными скобками
            text[textLen++] = '{';
            for (new k = 0; k < closing - inPos - 1 && textLen < charsmax(text); ++k) {
                text[textLen++] = src[inPos + 1 + k];
            }
            if (textLen < charsmax(text)) {
                text[textLen++] = '}';
            }
            PHTemplate_AddTextPart(parts, text, textLen);
            textLen = 0;
        }

        inPos = closing + 1;
    }

    if (textLen > 0) {
        PHTemplate_AddTextPart(parts, text, textLen);
    }

    return T_PHTemplate:parts;
}

PHTemplate_Format(const T_PHTemplate:tmpl, out[], const outLen) {
    if (outLen < 1) {
        return;
    }

    new outPos = 0;
    new partCount = ArraySize(Array:tmpl);
    static partData[S_PHTemplatePart];
    static value[PH_VALUE_MAX_LEN];

    for (new i = 0; i < partCount && outPos < outLen - 1; ++i) {
        ArrayGetArray(Array:tmpl, i, partData);

        if (partData[PHTemplatePart_Type] == PHTemplatePartType_Text) {
            for (new k = 0; partData[PHTemplatePart_Content][k] != EOS && outPos < outLen - 1; ++k) {
                out[outPos++] = partData[PHTemplatePart_Content][k];
            }
        } else {
            if (PHGroup_FillValue(partData[PHTemplatePart_Group], partData[PHTemplatePart_Content], partData[PHTemplatePart_Forward], value, charsmax(value))) {
                for (new k = 0; value[k] != EOS && outPos < outLen - 1; ++k) {
                    out[outPos++] = value[k];
                }
            }
        }
    }

    out[outPos] = EOS;
}

PHTemplate_Destroy(const T_PHTemplate:tmpl) {
    if (tmpl != Invalid_PHTemplate) {
        new Array:arr = Array:tmpl;
        ArrayDestroy(arr);
    }
}
