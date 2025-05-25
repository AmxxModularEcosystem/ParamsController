#if defined __params_controller_included
    #endinput
#endif
#define __params_controller_included

#include <amxmodx>
#include <json>

#define PARAM_TYPE_NAME_MAX_LEN 64
#define PARAM_KEY_MAX_LEN 64
#define PARAM_VALUE_MAX_LEN 512
#define PARAM_TYPE_READ_CALLBACK_MAX_LEN 64
#define PARAM_TYPE_TAG_MAX_LEN 64
#define PARAM_SHORT_STRING_MAX_LEN 64
#define PARAM_LONG_STRING_MAX_LEN 4096

stock const DEFAULT_PARAMS_BOOL_NAME[] = "Boolean";
stock const DEFAULT_PARAMS_INT_NAME[] = "Integer";
stock const DEFAULT_PARAMS_FLOAT_NAME[] = "Float";
stock const DEFAULT_PARAMS_STR_NAME[] = "String";
stock const DEFAULT_PARAMS_SHORT_STR_NAME[] = "ShortString";
stock const DEFAULT_PARAMS_LONG_STR_NAME[] = "LongString";
stock const DEFAULT_PARAMS_RGB_NAME[] = "RGB";
stock const DEFAULT_PARAMS_MODEL_NAME[] = "Model";
stock const DEFAULT_PARAMS_SOUND_NAME[] = "Sound";
stock const DEFAULT_PARAMS_RESOURCE_NAME[] = "Resource";
stock const DEFAULT_PARAMS_FILE_NAME[] = "File";
stock const DEFAULT_PARAMS_DIR_NAME[] = "Dir";

#define PARAMS_CONTROLLER_VERSION "1.1.0"
stock const PARAMS_CONTROLLER_LIBRARY[] = "PluginsController";
stock const PARAMS_CONTROLLER_VERSION_CVAR_NAME[] = "params_controller_version";

stock const PARAMS_CONTROLLER_TYPE_TAG_DELIMITER[] = ":";
#define PARAMS_CONTROLLER_TYPE_TAG_DELIMITER_CHAR ':'

enum T_Param { Invalid_Param = INVALID_HANDLE }
enum T_ParamType { Invalid_ParamType = INVALID_HANDLE }

enum _:E_NativeParam {
    ParamArg_sKey,
    ParamArg_sParamTypeName,
    ParamArg_bRequired,
}

enum E_ParamsReadErrorType {
    ParamsReadError_None,
    
    /**
     * Обязательный параметр отсутствует в переданном JSON-обьекте.
     *
     * Возникает только для обязательных параметров.
     */
    ParamsReadError_RequiredParamNotPresented,
    
    /**
     * Читаемое значение параметра некорректно.
     *
     * Значит что колбек чтения параметра вернул false.
     * Может возникнуть даже для необязательных параметров.
     */
    ParamsReadError_ParamValueIsInvalid,
}

/**
 * Вызывается в тот момент, когда контроллер готов регистрировать новые типы параметров
 *
 * @noreturn
 */
forward ParamsController_OnRegisterTypes();

/**
 * Вызывается после полной иницализации контроллера и регистрации всех типов
 *
 * @noreturn
 */
forward ParamsController_OnInited();

/**
 * Принудительная инициализация контроллера параметров
 *
 * @note    В ходе обработки этого натива будут вызваны форварды *_OnRegisterTypes и *_OnInited,
 *          а значит после его вызова можно создавать новые параметры (*_Param_Construct)
 *
 * @noreturn
 */
native ParamsController_Init();

/**
 * Сохранение прочитанного значения параметра (число, float, bool, и т.д.)
 *
 * @note    Можно вызывать только в рамках колбека чтения параметра
 *
 * @param iCell Сохраняемое значение 
 *
 * @return  true, если значение было установлено успешно, иначе false
 */
native bool:ParamsController_SetCell(const any:iCell);

/**
 * Сохранение прочитанного строкового значения параметра
 *
 * @note    Можно вызывать только в рамках колбека чтения параметра
 * @note    Буфер для сохраняемой строки имеет размер 4096 ячеек (PARAM_LONG_STRING_MAX_LEN)
 *          Если требуется сохранить бОльшую строку, то сохраняйте напрямую
 *
 * @param   sString Сохраняемая строка 
 *
 * @return  true, если значение было установлено успешно, иначе false
 */
native bool:ParamsController_SetString(const sString[]);

/**
 * Регистрация типа параметров
 *
 * @note    Можно вызывать только после вызова форварда *_OnRegisterTypes
 *
 * @param   sParamTypeName  Название типа параметра (будет указываться при создании параметра)
 *
 * @return  Хендлер зарегистрированного типа
 */
native T_ParamType:ParamsController_ParamType_Register(const sParamTypeName[]);

/**
 * Установка колбека для чтения параметра указанного типа
 *
 * @param   iParamType  Хендлер типа параметров, для которого будет установлен колбека
 * @param   sCallback   Название функции-колбека
 *
 * @note    Сигнатура колбека: bool:(const JSON:jValue, const Trie:tParams, const sParamKey[], const sParamTag[])
 *              jValue: JSON-значение читаемого параметра
 *              tParams: гарантированно валидный хендлер Trie-структуры
 *              sParamKey: ключ читаемого параметра
 *          Колбек должен либо вернуть false, либо записать в tParams какое-либо значение и вернуть true
 *          В противном случае контроллер выведет в консоль соответствующую ошибку.
 *
 * @noreturn
 */
native ParamsController_ParamType_SetReadCallback(const T_ParamType:iParamType, const sCallback[]);

/**
 * Регистрация типа параметров с установкой колбека для чтения
 *
 * @param   sParamTypeName  Название типа параметра (будет указываться при создании параметра)
 * @param   sCallback       Название функции-колбека
 *
 * @return  Хендлер зарегистрированного типа
 */
stock T_ParamType:ParamsController_RegSimpleType(const sParamTypeName[], const sCallback[]) {
    // Не то чтобы сейчас есть какие-то "сложные" типы,
    // но пусть пока будет так)
    new T_ParamType:iParamType = ParamsController_ParamType_Register(sParamTypeName);
    ParamsController_ParamType_SetReadCallback(iParamType, sCallback);

    return iParamType;
}

/**
 * Создание экземпляра параметра.
 *
 * @param   sKey            Ключ параметра (по этому ключу будет читаться параметр из JSON-обьекта и по нему же значение будет заноситься в trie)
 * @param   sParamTypeName  Название типа параметра
 * @param   bRequired       Является ли параметр обязательным
 * 
 * @note Если параметр был создан для разового использования, он может быть удалён нативом ParamsController_Param_Free().
 *
 * @return  Хендлер созданного параметра
 */
native T_Param:ParamsController_Param_Construct(const sKey[], const sParamTypeName[], const bool:bRequired = true);

/**
 * Удаление экземпляра параметра (очистка хендлера).
 *
 * @param   param Хендлер экземпляра параметра, который нужно удалить (очистить). Всегда заменяется на Invalid_Param.
 *
 * @return Всегда Invalid_Param.
 */
native T_Param:ParamsController_Param_Free(&T_Param:param);

stock Array:ParamsController_Param_FreeList(&Array:params) {
    if (params == Invalid_Array) {
        return params;
    }

    for (new i = 0, ii = ArraySize(params); i < ii; ++i) {
        ParamsController_Param_Free(ArrayGetCell(params, i));
    }
    ArrayDestroy(params);

    return params;
}

/**
 * Чтение массива параметров из JSON-обьекта
 *
 * @param   aParams             Массив параметров
 * @param   jObj                JSON-обьект
 * @param   tAppend             trie-хендлер, в который будут записываться прочитанные параметры (по умолчанию создаётся новый)
 * @param   iErrType            Переменная для записи типа ошибки
 * @param   sErrParamName       Буфер для записи ключа параметра, вызвавшего ошибку
 * @param   iErrParamNameLen    Длина буфера
 *
 * @note    В случае возникновения ошибки функция вернёт trie-хендлер только с теми параметрами,
 *          которые были прочитаны до возникновения ошибки. Если в таком случае хендлер не нужен,
 *          то нужно его освободить при помощи TrieDestroy().
 *
 * @return  Валидный trie-хендлер с прочитанными параметрами. Если был передан параметр tAppend, то будет возвращён тот же хендлер
 */
native Trie:ParamsController_Param_ReadList(const Array:aParams, const JSON:jObj, &Trie:tAppend = Invalid_Trie, &E_ParamsReadErrorType:iErrType = ParamsReadError_None,sErrParamName[] = "", const iErrParamNameLen = 0);

/**
 * Чтение одного параметра из JSON-значения.
 * 
 * @param   param     Хендлер параметра.
 * @param   valueJson JSON-значение.
 * @param   dest      Хендлер Trie, куда будет записан прочитанный параметр.
 * @param   orFailKey Название поля, для которого выводить ошибку в случае её возникновения. Если пусто - ошибка не будет выброшена.
 * 
 * @return  true, если параметр прочитан успешно, false в случае ошибки.
 */
native bool:ParamsController_Param_Read(const T_Param:param, const JSON:valueJson, const Trie:dest, const orFailKey[] = "");

/**
 * Создание параметра из параметров натива
 *
 * @note    Должно вызываться только в рамках обработки вызова натива
 *
 * @param   iParamOffset    Номер параметра, с которого начинать читать
 *
 * @note    Параметр создаётся по трём параметрам натива:
 *              - Ключ параметра
 *              - Название типа параметра
 *              - true/false является ли параметр обязательным
 *
 * @return  Хендлер созданного параметра
 */
stock T_Param:ParamsController_Param_FromNativeParams(const iParamOffset) {
    new sKey[PARAM_KEY_MAX_LEN];
    get_string(iParamOffset + ParamArg_sKey, sKey, charsmax(sKey));

    new sParamTypeName[PARAM_TYPE_NAME_MAX_LEN];
    get_string(iParamOffset + ParamArg_sParamTypeName, sParamTypeName, charsmax(sParamTypeName));
    
    new bool:bRequired = bool:get_param_byref(iParamOffset + ParamArg_bRequired);

    return ParamsController_Param_Construct(sKey, sParamTypeName, bRequired);
}

/**
 * Создание списка параметров из параметров натива
 *
 * @note    Должно вызываться только в рамках обработки вызова натива
 *
 * @param   iParamOffset    Номер параметра, с которого начинать читать
 * @param   iParamsCount    Общее количество параметров, переданных в натив (передаётся в обработчик натива первым параметром)
 * @param   aAppend         Массив, в который нужно дописывать созданные параметры (по умолчанию создастся новый)
 *
 * @return  Хендлер динамического массива с прочитанными параметрами (если был передан параметр aAppend, то вернётся его значение)
 */
stock Array:ParamsController_Param_ListFromNativeParams(const iParamOffset, const iParamsCount, &Array:aAppend = Invalid_Array) {
    if (aAppend == Invalid_Array) {
        aAppend = ArrayCreate(1, 1);
    }

    new iCount = (iParamsCount - iParamOffset + 1) / E_NativeParam;
    if (iCount < 1) {
        return aAppend;
    }

    for (new i = 0; i < iCount; ++i) {
        ArrayPushCell(aAppend, ParamsController_Param_FromNativeParams(iParamOffset + (i * E_NativeParam)));
    }

    return aAppend;
}

// Single

stock const PCSINGLE_PARAM_KEY[] = "__SINGLE";

/**
 * Чтение одного параметра из JSON-значения.
 * 
 * @param   valueJson JSON-значение.
 * @param   type      Название типа параметра. Может модержать тэг.
 * @param   orFailKey Название поля, для которого выводить ошибку в случае её возникновения. Если пусто - ошибка не будет выброшена.
 * 
 * @return  Хендлер Trie с прочитанным параметром по ключу "__SINGLE". Даже в случае ошибки хендлер будет создан.
 */
stock Trie:PCSingle_Read(const JSON:valueJson, const type[], const orFailKey[] = "") {
    new T_Param:param = ParamsController_Param_Construct(PCSINGLE_PARAM_KEY, type, true);

    new JSON:linkedJson;
    PCJson_HandleLinkedValue(valueJson, linkedJson);
    
    new Trie:p = TrieCreate();
    ParamsController_Param_Read(param, valueJson, p, orFailKey);

    ParamsController_Param_Free(param);
    PCJson_FreeLinked(linkedJson);
    return p;
}

/**
 * Чтение одного параметра из JSON-обьекта.
 * 
 * @param   objectJson JSON-обьект.
 * @param   key        Ключ, значение по которому нужно прочитать.
 * @param   type       Название типа параметра. Может модержать тэг.
 * @param   dotNot     Использовать ли вложенные ключи через точку.
 * @param   orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return  Хендлер Trie с прочитанным параметром по ключу "__SINGLE". Даже в случае ошибки хендлер будет создан.
 */
stock Trie:PCSingle_ReadFromObject(const JSON:objectJson, const key[], const type[], const bool:dotNot = false, const bool:orFail = false) {
    if (!json_is_object(objectJson)) {
        PCJson_ErrorForFile(objectJson, "Json value must be an object. (%d)", objectJson);
        return Invalid_Trie;
    }

    if (!json_object_has_value(objectJson, key, .dot_not = dotNot)) {
        if (orFail) {
            PCJson_ErrorForFile(objectJson, "Object must contain '%s' field.", key);
        }
        return Invalid_Trie;
    }

    new JSON:valueJson = json_object_get_value(objectJson, key, dotNot);
    new Trie:ret = PCSingle_Read(valueJson, type, orFail ? key : "");
    json_free(valueJson);

    return ret;
}

/**
 * Чтение числового типа параметра из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  type       Название типа параметра. Может модержать тэг.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Прочитанное значение.
 */
stock any:PCSingle_ObjCell(const JSON:objectJson, const key[], const type[], const any:def = 0, const bool:dotNot = false, const bool:orFail = false) {
    new Trie:p = PCSingle_ReadFromObject(objectJson, key, type, dotNot, orFail);

    new any:value = def;
    
    if (p != Invalid_Trie) {
        TrieGetCell(p, PCSINGLE_PARAM_KEY, value);
    }
    TrieDestroy(p);

    return value;
}

/**
 * Чтение параметра типа "Integer" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Прочитанное значение.
 */
stock PCSingle_ObjInt(const JSON:objectJson, const key[], const any:def = 0, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, DEFAULT_PARAMS_INT_NAME, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "Float" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Прочитанное значение.
 */
stock Float:PCSingle_ObjFloat(const JSON:objectJson, const key[], const Float:def = 0.0, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, DEFAULT_PARAMS_FLOAT_NAME, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "Bool" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Прочитанное значение.
 */
stock bool:PCSingle_ObjBool(const JSON:objectJson, const key[], const bool:def = false, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, DEFAULT_PARAMS_BOOL_NAME, def, dotNot, orFail);
}

/**
 * Чтение строкового типа параметра из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  type       Название типа параметра. Может модержать тэг.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock any:PCSingle_ObjStr(const JSON:objectJson, const key[], const type[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    new Trie:p = PCSingle_ReadFromObject(objectJson, key, type, dotNot, orFail);

    new ret;
    if (p != Invalid_Trie && TrieKeyExists(p, PCSINGLE_PARAM_KEY)) {
        ret = TrieGetString(p, PCSINGLE_PARAM_KEY, out, outLen);
    } else {
        ret = copy(out, outLen, def);
    }
    TrieDestroy(p);

    return ret;
}

/**
 * Чтение строкового типа параметра из JSON-обьекта. Inline вариант функуции PCSingle_ObjStr.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  type       Название типа параметра. Может модержать тэг.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Прочитанное значение.
 */
stock PCSingle_iObjStr(const JSON:objectJson, const key[], const type[] = DEFAULT_PARAMS_STR_NAME, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    new out[PARAM_VALUE_MAX_LEN];
    PCSingle_ObjStr(objectJson, key, type, out, charsmax(out), def, dotNot, orFail);
    return out;
}

/**
 * Чтение параметра типа "String" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjString(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_STR_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "ShortString" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjShortString(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_SHORT_STR_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "LongString" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjLongString(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_LONG_STR_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "Model" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjModel(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_MODEL_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "Sound" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjSound(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_SOUND_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "Resource" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjResource(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_RESOURCE_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "File" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjFile(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_FILE_NAME, out, outLen, def, dotNot, orFail);
}

/**
 * Чтение параметра типа "Dir" из JSON-обьекта.
 * 
 * @param  objectJson JSON-обьект.
 * @param  key        Ключ, значение по которому нужно прочитать.
 * @param  out        Буфер для записи прочитанного значения.
 * @param  outLen     Размер буфера.
 * @param  def        Значение по умолчанию, в случае ошибки или отсутствия поля.
 * @param  dotNot     Использовать ли вложенные ключи через точку.
 * @param  orFail     Выбрасывать ли ошибку в случае её возникновения.
 * 
 * @return Кол-во записанных в буфер ячеек.
 */
stock PCSingle_ObjDir(const JSON:objectJson, const key[], out[], const outLen, const def[] = "", const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjStr(objectJson, key, DEFAULT_PARAMS_DIR_NAME, out, outLen, def, dotNot, orFail);
}

// Getters

stock any:PCGet_Cell(const Trie:p, const key[], const any:def = 0) {
    new any:cell = def;
    if (p != Invalid_Trie) {
        TrieGetCell(p, key, cell);
    }
    return cell;
}

stock any:PCGet_Str(const Trie:p, const key[], out[], const outLen, const def[] = "") {
    if (p != Invalid_Trie && TrieKeyExists(p, key)) {
        return TrieGetString(p, key, out, outLen);
    } else {
        return copy(out, outLen, def);
    }
}

stock any:PCGet_iStr(const Trie:p, const key[], const def[] = "") {
    new str[PARAM_VALUE_MAX_LEN];
    PCGet_Str(p, key, str, charsmax(str), def);
    return str;
}

stock PCGet_Int(const Trie:p, const key[], const def = 0) {
    return PCGet_Cell(p, key, def);
}

stock Float:PCGet_Float(const Trie:p, const key[], const Float:def = 0.0) {
    return PCGet_Cell(p, key, def);
}

stock bool:PCGet_Bool(const Trie:p, const key[], const bool:def = false) {
    return PCGet_Cell(p, key, def);
}

stock PCGet_RGB(const Trie:p, const key[]) {
    new color[3];
    TrieGetArray(p, key, color, 3);

    return color;
}

stock PCGet_RGB_Red(const Trie:p, const key[]) {
    return PCGet_RGB(p, key)[0];
}

stock PCGet_RGB_Green(const Trie:p, const key[]) {
    return PCGet_RGB(p, key)[1];
}

stock PCGet_RGB_Blue(const Trie:p, const key[]) {
    return PCGet_RGB(p, key)[2];
}

// Path Utils

stock PCPath_IsPathSeparator(const ch) {
    return ch == '/' || ch == '\';
}

stock PCPath_Normalize(out[], const outLen) {
    replace_string(out, outLen, "\", "/");
    replace_string(out, outLen, "//", "/");
    replace_string(out, outLen, "./", "");
}

stock PCPath_MakePath(const path[], out[], const outLen, const absolutePrefix[] = "", const forceFileExtension[] = "") {
    static __amxx_configsdir[PLATFORM_MAX_PATH];
    if (!__amxx_configsdir[0]) {
        get_localinfo("amxx_configsdir", __amxx_configsdir, charsmax(__amxx_configsdir));
    }

    if (PCPath_IsPathSeparator(path[0])) {
        formatex(out, outLen, "%s/%s", __amxx_configsdir, path)
    } else {
        if (absolutePrefix[0] != EOS) {
            formatex(out, outLen, "%s/%s", absolutePrefix, path);
        } else {
            formatex(out, outLen, "%s/plugins/%s", __amxx_configsdir, path);
        }
    }

    PCPath_Normalize(out, outLen);

    if (forceFileExtension[0] != EOS) {
        static ext[PLATFORM_MAX_PATH];
        formatex(ext, charsmax(ext), ".%s", forceFileExtension);

        if (strfind(out, ext, .ignorecase = true, .pos = strlen(out) - strlen(ext)) < 0) {
            formatex(out, outLen, "%s%s", out, ext);
        }
    }
}

stock PCPath_iMakePath(const path[], const absolutePrefix[] = "", const forceFileExtension[] = "") {
    new out[PLATFORM_MAX_PATH];
    PCPath_MakePath(path, out, charsmax(out), absolutePrefix, forceFileExtension);
    return out;
}

// Json Wrapper

stock const PC_JSON_WRAPPER_FIELD_ROOT[] = "___PC_JSON_WRAPPER_FIELD_ROOT___dOnOtUsEtHiSkEyFoRrEaLdAtA___";
stock const PC_JSON_WRAPPER_FIELD_HANDLER[] = "Handler";
stock const PC_JSON_WRAPPER_FIELD_WORKDIR[] = "Workdir";
stock const PC_JSON_WRAPPER_FIELD_FILE[] = "File";
stock const PC_JSON_WRAPPER_FIELD_VALUE[] = "Value";
stock const PC_JSON_WRAPPER_FIELD_LINKED[] = "Linked";
#define PC_JSON_WRAPPER_MESSAGE_MAX_LEN 256

stock const PC_JSON_WRAPPER_LINK_PREFIX_FILE[] = "File:";

stock JSON:PCJson_ParseFile(const path[], const workdir[] = "") {
    if (!file_exists(path)) {
        abort(AMX_ERR_GENERAL, "[JSON][ERROR] File '%s' not found.", path);
        return Invalid_JSON;
    }

    new JSON:value = json_parse(path, .is_file = true, .with_comments = true);
    if (value == Invalid_JSON) {
        abort(AMX_ERR_GENERAL, "[JSON][ERROR] File '%s' contains invalid JSON value.", path);
        return Invalid_JSON;
    }

    new JSON:wrapper = json_init_object();
    json_object_set_bool(wrapper, PC_JSON_WRAPPER_FIELD_ROOT, true);
    json_object_set_number(wrapper, PC_JSON_WRAPPER_FIELD_HANDLER, _:wrapper);
    json_object_set_string(wrapper, PC_JSON_WRAPPER_FIELD_FILE, path);
    json_object_set_value(wrapper, PC_JSON_WRAPPER_FIELD_VALUE, value);

    if (workdir[0] != EOS) {
        json_object_set_string(wrapper, PC_JSON_WRAPPER_FIELD_WORKDIR, workdir);
    } else {
        static autoWorkdir[PLATFORM_MAX_PATH];
        copy(autoWorkdir, charsmax(autoWorkdir), path);
        for (new i = strlen(autoWorkdir) - 1; i >= 0; --i) {
            if (PCPath_IsPathSeparator(autoWorkdir[i]) || i == 0) {
                autoWorkdir[i] = EOS;
                break;
            }
        }
        json_object_set_string(wrapper, PC_JSON_WRAPPER_FIELD_WORKDIR, autoWorkdir);
    }

    return value;
}

stock JSON:PCJson_Free(&JSON:value, const bool:rootOnly = false) {
    if (value == Invalid_JSON) {
        return Invalid_JSON;
    }

    if (PCJson_IsWrapper(value)) {
        json_free(value);
        return Invalid_JSON;
    }
    
    if (!PCJson_IsWrappedRoot(value)) {
        if (rootOnly && !PCJson_IsRoot(value)) {
            return value;
        } else {
            json_free(value);
            return Invalid_JSON;
        }
    }
    
    new JSON:wrapper = PCJson_FindWrapper(value);
    PCJson_FreeWrapper(wrapper);

    json_free(value);
    return Invalid_JSON;
}

stock JSON:PCJson_FreeWrapper(&JSON:value) {
    if (!PCJson_IsWrapper(value)) {
        abort(AMX_ERR_PARAMS, "Attempt to free not a wrapper via PCJson_FreeWrapper() function.");
        return Invalid_JSON;
    }

    new JSON:mainWrapperHandler = JSON:json_object_get_number(value, PC_JSON_WRAPPER_FIELD_HANDLER);
    json_free(value);
    json_free(mainWrapperHandler);

    return Invalid_JSON;
}

stock bool:PCJson_IsWrapper(const JSON:value) {
    if (value == Invalid_JSON) {
        return false;
    }

    return json_is_object(value)
        && PCJson_IsRoot(value)
        && json_object_has_value(value, PC_JSON_WRAPPER_FIELD_ROOT, JSONBoolean);
}

stock bool:PCJson_IsRoot(const JSON:value) {
    if (value == Invalid_JSON) {
        return false;
    }
    
    new JSON:parent = json_get_parent(value);
    new bool:ret = parent == Invalid_JSON;
    json_free(parent);
    return ret;
}

stock bool:PCJson_IsWrappedRoot(const JSON:value) {
    if (value == Invalid_JSON) {
        return false;
    }
    
    new JSON:parent = json_get_parent(value);
    new bool:ret = PCJson_IsWrapper(parent);
    json_free(parent);
    return ret;
}

stock bool:PCJson_IsLink(const JSON:value) {
    if (value == Invalid_JSON || !json_is_string(value)) {
        return false;
    }

    static str[sizeof PC_JSON_WRAPPER_LINK_PREFIX_FILE];
    json_get_string(value, str, charsmax(str));

    return bool:equali(str, PC_JSON_WRAPPER_LINK_PREFIX_FILE, charsmax(str));
}

stock JSON:PCJson_FindRoot(const JSON:value, const Array:trace = Invalid_Array) {
    if (value == Invalid_JSON) {
        return Invalid_JSON;
    }

    if (trace != Invalid_Array) {
        ArrayPushCell(trace, json_get_type(value));
    }

    new JSON:parent;
    new JSON:iter = json_get_parent(value);
    if (iter == Invalid_JSON) {
        return value;
    }

    while (iter != Invalid_JSON) {
        if (trace != Invalid_Array) {
            ArrayPushCell(trace, json_get_type(iter));
        }

        parent = json_get_parent(iter);
        
        if (parent == Invalid_JSON) {
            break;
        }
        json_free(iter);
        iter = parent;
    }

    return iter;
}

stock JSON:PCJson_FindWrapper(const JSON:value, const Array:trace = Invalid_Array) {
    new JSON:root = PCJson_FindRoot(value, .trace = trace);

    if (!PCJson_IsWrapper(root)) {
        if (value != root) {
            json_free(root);
        }
        return Invalid_JSON;
    }
    
    return root;
}

stock PCJson_GetWrapperWorkdir(const JSON:value, out[] = "", const outLen = 0, const def[] = "") {
    new path[PLATFORM_MAX_PATH];

    new JSON:wrapper = PCJson_FindWrapper(value);
    if (wrapper != Invalid_JSON) {
        json_object_get_string(wrapper, PC_JSON_WRAPPER_FIELD_WORKDIR, path, charsmax(path));
    }
    
    if (wrapper != value) {
        json_free(wrapper);
    }

    if (path[0] == EOS) {
        copy(path, charsmax(path), def);
    }

    if (outLen > 0) {
        copy(out, outLen, path);
    }

    return path;
}

stock PCJson_GetWrapperFile(const JSON:value, out[] = "", const outLen = 0, const def[] = "", const Array:trace = Invalid_Array) {
    new path[PLATFORM_MAX_PATH];

    new JSON:wrapper = PCJson_FindWrapper(value, .trace = trace);
    if (wrapper != Invalid_JSON) {
        json_object_get_string(wrapper, PC_JSON_WRAPPER_FIELD_FILE, path, charsmax(path));
    }
    
    if (wrapper != value) {
        json_free(wrapper);
    }

    if (path[0] == EOS) {
        copy(path, charsmax(path), def);
    }

    if (outLen > 0) {
        copy(out, outLen, path);
    }

    return path;
}

stock PCJson_ExtractLinkPath(const JSON:link, out[], const outLen) {
    out[0] = EOS;
    if (!PCJson_IsLink(link)) {
        return 0;
    }

    json_get_string(link, out, outLen);
    return replace(out, outLen, PC_JSON_WRAPPER_LINK_PREFIX_FILE, "");
}

stock bool:PCJson_HandleLinkedValue(const JSON:value, &JSON:linkedOrSameValue) {
    linkedOrSameValue = value;
    if (!PCJson_IsLink(value)) {
        return false;
    }

    new JSON:wrapper = PCJson_FindWrapper(value);
    if (wrapper == Invalid_JSON) {
        return false;
    }

    static linkPath[PLATFORM_MAX_PATH];
    PCJson_ExtractLinkPath(value, linkPath, charsmax(linkPath));

    static workdir[PLATFORM_MAX_PATH];
    if (linkPath[0] == '.' && linkPath[1] == '/') {
        workdir[0] = EOS;
    } else {
        PCJson_GetWrapperWorkdir(wrapper, workdir, charsmax(workdir));
    }

    static path[PLATFORM_MAX_PATH];
    PCPath_MakePath(
        linkPath, path, charsmax(path),
        .absolutePrefix = workdir,
        .forceFileExtension = "json"
    );

    new JSON:linkedValue = PCJson_ParseFile(path, workdir);

    new JSON:deepLinkedValue;
    if (PCJson_HandleLinkedValue(linkedValue, deepLinkedValue)) {
        PCJson_Free(linkedValue);
        linkedOrSameValue = deepLinkedValue;
        return true;
    } else {
        linkedOrSameValue = linkedValue;
        return true;
    }
}

enum _:E_PCJson_CachedLinkedValueState {
    PCJson_CachedLinkedValue_NotLinked = 0,
    PCJson_CachedLinkedValue_Linked = 1,
    PCJson_CachedLinkedValue_Cached = 2,
}

stock PCJson_HandleCachedLinkedValue(const Trie:cache, const JSON:value, &JSON:linkedOrSameValue, &any:cachedCell, linkPath[], const linkPathLen) {
    linkedOrSameValue = value;

    if (cache == Invalid_Trie) {
        return _:PCJson_HandleLinkedValue(value, linkedOrSameValue);
    }

    PCJson_ExtractLinkPath(value, linkPath, linkPathLen);
    if (linkPath[0] == EOS) {
        return PCJson_CachedLinkedValue_NotLinked;
    }

    if (TrieGetCell(cache, linkPath, cachedCell)) {
        return PCJson_CachedLinkedValue_Cached;
    }

    return _:PCJson_HandleLinkedValue(value, linkedOrSameValue);
}

#pragma deprecated Dont use example function, please...
stock someTag:USAGE_EXAMPLE__PCJson_HandleCachedLinkedValue(const JSON:value) {
    // global
    new Trie:cache; // linkPath[] => any:cachedCell

    // func
    new JSON:linked, someTag:cachedCell = someTag:-1, linkPath[PLATFORM_MAX_PATH];
    if (PCJson_HandleCachedLinkedValue(cache, value, linked, cachedCell, linkPath, charsmax(linkPath)) == PCJson_CachedLinkedValue_Cached) {
        return cachedCell;
    }
    
    // cachedCell = YourJsonReadFunc(linked);

    if (linkPath[0] != EOS && cachedCell >= 0) {
        TrieSetCell(cache, linkPath, cachedCell);
    }

    PCJson_FreeLinked(linked);
    return cachedCell;
}

stock JSON:PCJson_FreeLinked(&JSON:value) {
    return PCJson_Free(value, .rootOnly = true);
}

stock PCJson_LogForFile(const JSON:value, const prefix[], const messageFormat[], any:...) {
    static msg[PC_JSON_WRAPPER_MESSAGE_MAX_LEN];
    vformat(msg, charsmax(msg), messageFormat, 4);

    new Array:trace = ArrayCreate(1, 1);

    static path[PLATFORM_MAX_PATH];
    PCJson_GetWrapperFile(value, path, charsmax(path), .def = "*unknown file*", .trace = trace);

    log_amx("[JSON][%s] File: %s", prefix, path);
    // log_amx("[JSON][%s] Trace: %s", prefix, PCJSon_TraceToString(trace, .reverse = true)); // Экспериментальное
    log_amx("[JSON][%s] Log: %s", prefix, msg);

    ArrayDestroy(trace);
}

stock PCJson_ErrorForFile(const JSON:value, const messageFormat[], any:...) {
    static msg[PC_JSON_WRAPPER_MESSAGE_MAX_LEN];
    vformat(msg, charsmax(msg), messageFormat, 3);

    static path[PLATFORM_MAX_PATH];
    PCJson_GetWrapperFile(value, path, charsmax(path), .def = "*unknown file*");

    abort(AMX_ERR_GENERAL, "[JSON][ERROR] %s (File '%s')", msg, path);
}
#define PCJson_AbortForFile PCJson_ErrorForFile

stock PCJson_FailForFile(const JSON:value, const messageFormat[], any:...) {
    static msg[PC_JSON_WRAPPER_MESSAGE_MAX_LEN];
    vformat(msg, charsmax(msg), messageFormat, 3);

    static path[PLATFORM_MAX_PATH];
    PCJson_GetWrapperFile(value, path, charsmax(path), .def = "*unknown file*");

    set_fail_state("[JSON][FATAL ERROR] %s (File '%s')", msg, path);
}

stock PCJSon_TraceToString(const Array:trace, const bool:reverse = false) {
    new str[256];
    for (new i = 0, ii = ArraySize(trace); i < ii; ++i) {
        if (i > 0) {
            add(str, charsmax(str), "->");
        }

        switch (ArrayGetCell(trace, reverse ? ii - i - 1 : i)) {
            case JSONError:
                // Хз зачем, но пусть будет)
                add(str, charsmax(str), "err");
            case JSONNull:
                add(str, charsmax(str), "null");
            case JSONString:
                add(str, charsmax(str), "str");
            case JSONNumber:
                add(str, charsmax(str), "num");
            case JSONObject:
                add(str, charsmax(str), "obj");
            case JSONArray:
                add(str, charsmax(str), "arr");
            case JSONBoolean:
                add(str, charsmax(str), "bool");
        }
    }

    return str;
}
