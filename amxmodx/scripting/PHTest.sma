#include <amxmodx>
#include <ParamsController>

// Группа тест-плагина для игроков: {t:name}, {t:health}
static T_PHGroup:g_testGroup = Invalid_PHGroup;

// Ссылка на встроенную группу игрока {p:authid} — кешируем, чтобы не искать по строке на каждый вызов
static T_PHGroup:g_defaultPlayerGroup = Invalid_PHGroup;

// Скомпилированные шаблоны
static T_PHTemplate:g_templateHud = Invalid_PHTemplate;
static T_PHTemplate:g_templateGreet = Invalid_PHTemplate;

public plugin_init() {
    register_plugin("PH Test", "1.0", "Test");

    register_clcmd("say /ph_test",     "CmdInline");
    register_clcmd("say /ph_compiled", "CmdCompiled");
    register_clcmd("say /ph_context",  "CmdContext");

    ParamsController_Init();
}

// Компилируем шаблоны после того как все группы и ключи зарегистрированы
public ParamsController_OnInited() {
    g_templateHud = PCPH_CompileTemplate(
        "Map: {map} | Player: {t:name} | HP: {t:health} | Auth: {p:authid}"
    );
    g_templateGreet = PCPH_CompileTemplate(
        "Hi {t:name}! Time: {time} | Server: {server} | Unknown: {bad:key}"
    );
}

// ===== Регистрация =====

public PCPH_OnRegisterGroups() {
    // Кешируем встроенную группу игрока — к этому моменту она уже зарегистрирована
    g_defaultPlayerGroup = PCPH_FindGroup(DEFAULT_PH_PLAYER_GROUP_PREFIX);

    // Глобальная группа: реактивный {time} и проактивный {server}
    PCPH_Gl_RegKey("time", "PH_Time");
    PCPH_Gl_RegisterKey("server");

    static hostname[64];
    get_user_name(0, hostname, charsmax(hostname));
    PCPH_Set(PCPH_GetGlobalGroup(), "server", hostname);

    // Группа тест-плагина: {t:name} — проактивный, {t:health} — реактивный
    g_testGroup = PCPH_RegisterGroup("t");
    PCPH_Group_RegisterKey(g_testGroup, "name");
    PCPH_Group_RegKey(g_testGroup, "health", "PH_Health");
}

// ===== Колбеки плейсхолдеров =====

public PH_Time(const key[], const contextKey[]) {
    static buf[16];
    format_time(buf, charsmax(buf), "%H:%M:%S");
    PCPH_Cb_Set(buf);
}

// contextKey содержит индекс игрока, переданный через PushIntContext
public PH_Health(const key[], const contextKey[]) {
    PCPH_Cb_SetInt(get_user_health(str_to_num(contextKey)));
}

// ===== Обновление проактивных значений =====

public client_putinserver(playerIndex) {
    UpdateName(playerIndex);
}

public client_infochanged(playerIndex) {
    UpdateName(playerIndex);
}

UpdateName(playerIndex) {
    static name[32];
    get_user_name(playerIndex, name, charsmax(name));
    PCPH_SetForInt(g_testGroup, "name", playerIndex, name);
}

// ===== Хелперы для управления контекстом =====

PushPlayerContexts(playerIndex) {
    PCPH_Group_PushIntContext(g_testGroup, playerIndex);
    PCPH_Group_PushIntContext(g_defaultPlayerGroup, playerIndex);
}

PopPlayerContexts() {
    PCPH_Group_PopContext(g_defaultPlayerGroup);
    PCPH_Group_PopContext(g_testGroup);
}

// ===== Команды =====

// /ph_test — инлайн форматирование: глобальный контекст, контекст игрока, неизвестный ключ
public CmdInline(playerIndex) {
    static out[512];

    // Только глобальные плейсхолдеры — контекст игрока не нужен
    PCPH_Format(out, charsmax(out), "Global: map={map} time={time} server={server}");
    client_print(playerIndex, print_chat, "[inline/global] %s", out);

    // Плейсхолдеры игрока — нужен контекст
    PushPlayerContexts(playerIndex);
    PCPH_Format(out, charsmax(out), "Player: name={t:name} hp={t:health} auth={p:authid}");
    client_print(playerIndex, print_chat, "[inline/player] %s", out);
    PopPlayerContexts();

    // Неизвестный плейсхолдер остаётся как есть в строке
    PCPH_Format(out, charsmax(out), "known={map} unknown={bad:key}");
    client_print(playerIndex, print_chat, "[inline/unknown] %s", out);

    return PLUGIN_HANDLED;
}

// /ph_compiled — скомпилированные шаблоны
public CmdCompiled(playerIndex) {
    static out[512];

    PushPlayerContexts(playerIndex);

    PCPH_FormatTemplate(g_templateHud, out, charsmax(out));
    client_print(playerIndex, print_chat, "[compiled/hud] %s", out);

    PCPH_FormatTemplate(g_templateGreet, out, charsmax(out));
    client_print(playerIndex, print_chat, "[compiled/greet] %s", out);

    PopPlayerContexts();

    return PLUGIN_HANDLED;
}

// /ph_context — демонстрация стека контекстов
// Показывает три уровня: текущий игрок → другой игрок → возврат к текущему
public CmdContext(playerIndex) {
    static out[512];

    // Уровень 1: контекст текущего игрока
    PCPH_Group_PushIntContext(g_testGroup, playerIndex);
    PCPH_Format(out, charsmax(out), "L1 (self): name={t:name} hp={t:health}");
    client_print(playerIndex, print_chat, "[ctx/L1] %s", out);

    // Уровень 2: поверх кладём другого игрока — стек g_testGroup теперь [playerIndex, other]
    new other = (playerIndex == 1) ? 2 : 1;
    PCPH_Group_PushIntContext(g_testGroup, other);
    PCPH_Format(out, charsmax(out), "L2: name={t:name} hp={t:health}");
    client_print(playerIndex, print_chat, "[ctx/L2 #%d] %s", other, out);

    // Pop — стек возвращается к playerIndex
    PCPH_Group_PopContext(g_testGroup);
    PCPH_Format(out, charsmax(out), "L1 again (self): name={t:name} hp={t:health}");
    client_print(playerIndex, print_chat, "[ctx/back] %s", out);

    PCPH_Group_PopContext(g_testGroup);

    return PLUGIN_HANDLED;
}
