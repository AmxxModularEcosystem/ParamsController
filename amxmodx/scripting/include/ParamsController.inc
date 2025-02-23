#if defined __params_controller_included
    #endinput
#endif
#define __params_controller_included

#include <amxmodx>

#define PARAM_TYPE_NAME_MAX_LEN 64
#define PARAM_KEY_MAX_LEN 64
#define PARAM_VALUE_MAX_LEN 512
#define PARAM_TYPE_READ_CALLBACK_MAX_LEN 64
#define PARAM_TYPE_TAG_MAX_LEN 64
#define PARAM_SHORT_STRING_MAX_LEN 64
#define PARAM_LONG_STRING_MAX_LEN 4096

#define _PARAMS_CONTROLLER_VERSION "1.0.0"
stock const PARAMS_CONTROLLER_VERSION[] = _PARAMS_CONTROLLER_VERSION;
stock const PARAMS_CONTROLLER_LIBRARY[] = "PluginsController";

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
 * Создание параметра
 *
 * @param   sKey            Ключ параметра (по этому ключу будет читаться параметр из JSON-обьекта и по нему же значение будет заноситься в trie)
 * @param   sParamTypeName  Название типа параметра
 * @param   bRequired       Является ли параметр обязательным
 *
 * @return  Хендлер созданного параметра
 */
native T_Param:ParamsController_Param_Construct(const sKey[], const sParamTypeName[], const bool:bRequired = true);

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
