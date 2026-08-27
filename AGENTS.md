# AGENTS.md

Guidance for AI agents working with code in this repository.

## Build

**Requirements:** [amxx-builder](https://github.com/AmxxModularEcosystem/amxx-builder) (`amxb`). The manifest is `amxbuild.yml` — it pins the AMXX compiler version (`amxmodx.version`) and declares include dependencies (`deps`).

```bat
build.bat        :: local wrapper around `amxb build`
amxb build       :: actual build command
```

Output goes to `dist/` (or `ParamsController.zip` at repo root when packaging). Build settings live in `amxbuild.yml`. No `deps` are currently declared — the plugin intentionally has no third-party include dependencies (ReAPI was removed; default placeholder groups use only AMXX core natives).

There are no test or lint commands — the CI pipeline (`.github/workflows/CI.yml`) runs the same `amxb build` as the validation step.

> The plugin must compile without warnings that are errors — CI treats compiler output as the gate. Run `amxb build` before pushing.

## Architecture

ParamsController is an AmxModX plugin library (written in PAWN) that provides:

1. **Parameter parsing** — a JSON-based typed parameter framework. A consumer plugin calls `ParamsController_Init()`, constructs typed parameters with `ParamsController_Param_Construct()`, then reads a JSON object via `ParamsController_Param_ReadList()`. Each parameter is validated against its registered type handler.
2. **Placeholder system** — a global/per-player string substitution framework (groups, keys, contexts, proxy groups, compiled templates). Full design is documented in `PLACEHOLDERS.md`.

### Layer structure

| Layer | Path | Role |
|---|---|---|
| Public API | `amxmodx/scripting/include/ParamsController.inc` | All native/forward declarations + stock helpers; this is what consumers include |
| Main plugin | `amxmodx/scripting/ParamsController.sma` | Entry point, library registration, init flow, forward registration |
| Params API impl | `ParamsController/API/` | Native implementations (General, Param, ParamType, Utils) |
| Placeholder API impl | `ParamsController/Placeholders/API/` | `PCPH_*` native implementations (General, Group, Key) |
| Object model | `ParamsController/Objects/` | `Param.inc` (Array + free-list storage), `ParamType.inc` (Trie-keyed type registry, current-param stack) |
| Placeholder objects | `ParamsController/Placeholders/Objects/` | `PHGroup.inc` (group registry, contexts, values, callback stack), `PHTemplate.inc` (compiled template cache) |
| Placeholder utils | `ParamsController/Placeholders/PHParse.inc` | Shared placeholder parsing (`PHParse_FindClosing`, `PHParse_Placeholder`) used by inline formatting and template compilation |
| Built-in types | `ParamsController/DefaultObjects/ParamType/` | 18 files registering 20 type names: Boolean, Integer, Float, String/ShortString/LongString, RGB, Model, PlayerModel, Sound, Resource, File, Dir, ChatMessage, Time, TimeInterval, WeekDay, Flags, Regexp, PH-Template |
| Default placeholders | `ParamsController/DefaultObjects/Placeholder/` | `Global.inc`, `Player.inc` — built-in groups (`{map}`, `{real-map}`, `{server-name}`, `{server-ip}`, `{date}`, `{time}`, `{datetime}`, `{players-count}`, `{max-players-count}`; `{p:authid}`, `{p:name}`, `{p:ip}`, `{p:userid}`) |
| Registrar | `ParamsController/DefaultObjects/Registrar.inc` | Wires up all default types and placeholder groups on plugin load |
| Forwards | `ParamsController/Forwards.inc` | Generic forward registration utility used throughout |

### Key design patterns

- **Parameter storage:** Array-based with a free-list for handle reuse (`Objects/Param.inc`)
- **Type registry:** Trie-keyed by type name string (`Objects/ParamType.inc`); read callbacks are `CreateOneForward`-based, so plugin-owned
- **Current param context:** `CurrentParams`/`CurrentParamName` globals + a save/restore stack, so read callbacks can nest (types that internally read other types via `PCSingle_*`)
- **Placeholder groups:** Array-backed, Trie-keyed by prefix (`Placeholders/Objects/PHGroup.inc`); values stored as `"key:contextKey"` strings; contexts are per-group stacks
- **Proxy groups:** alias a source group's keys/callbacks/values but keep an independent context stack (two players in one string)
- **Callback reentrancy:** the currently-active callback (group/key/context) is kept on a stack (`S_PHCurrentCallback`), so `PCPH_Format`/`PCPH_Cb_Set*` can be called from inside a callback — nested calls restore the outer state
- **Compiled templates:** `PCPH_CompileTemplate` pre-parses a format string into parts and caches group/forward handles (`Placeholders/Objects/PHTemplate.inc`)
- **Extensibility:** consumers can register custom parameter types (`ParamsController_ParamType_Register` / `ParamsController_RegSimpleType`) and placeholder groups/keys via `PCPH_*` natives
- **Error reporting:** `E_ParamsReadErrorType` enum returned from `ReadList`; error details (param name, expected/got type) accessible via out-params of `ParamsController_Param_ReadList`

### Init flow (`PluginInit` in `ParamsController.sma`)

1. `Forwards_Init()`, `Param_Init()`, `PHGroup_Init()` — allocate storages
2. `DefaultObjects_Register()` — register built-in param types + default placeholder groups
3. Forward `ParamsController_OnRegisterTypes` — consumers register custom param types
4. Forward `PCPH_OnRegisterGroups` — consumers register placeholder groups/keys
5. Forward `PCPH_OnRegisterProxyGroups` — consumers register proxy groups (all normal groups already exist)
6. Forward `ParamsController_OnInited` — everything is ready

### Language notes

PAWN uses `#include` for all modular decomposition. `.inc` files are not independently compiled — they are all included into `ParamsController.sma`. The public header (`ParamsController.inc`) only declares natives/forwards and stock helpers; it contains no plugin state or implementation logic.

## Conventions

- Natives are named `ParamsController_*` (params) and `PCPH_*` (placeholders); implementation handlers use the `@` prefix (e.g. `@API_Param_Construct`) and `register_native(name, "@handler")`.
- All buffers that cross the callback boundary should be local (`new`), not `static` — placeholder callbacks can re-enter the formatter.
- Keep the manifest free of undeclared includes: any `#include <...>` beyond AMXX stdlib must be declared in `amxbuild.yml` `deps` (or removed).
- Do not commit build output — `dist/`, `build/`, `.amxb-cache/` are git-ignored.
