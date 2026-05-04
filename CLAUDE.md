# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

**Requirements:** AmxModX 1.9.0+, AMxModX compiler configured as `amxx190` in `config.bat`

```bat
build.bat
```

Output goes to `.build/` directory, final artifact is a ZIP package. Build settings are in `.build-config`. Нужен только для локальной сборки.

There are no test or lint commands — the CI pipeline (`.github/workflows/CI.yml`) compiles all `.sma` files as the validation step.

## Architecture

ParamsController is an AmxModX plugin library (written in PAWN) that provides a JSON-based parameter parsing framework for other plugins.

**Core flow:** A consumer plugin calls `ParamsController_Init()`, constructs typed parameters with `ParamsController_Param_Construct()`, then reads a JSON object using `ParamsController_Param_ReadList()`. Each parameter is validated against its registered type handler.

### Layer structure

| Layer | Path | Role |
|---|---|---|
| Public API | `amxmodx/scripting/include/ParamsController.inc` | All native/forward declarations; this is what consumers include |
| Main plugin | `amxmodx/scripting/ParamsController.sma` | Entry point, library registration, forward init |
| API impl | `ParamsController/API/` | Native implementations (General, Param, ParamType, Utils) |
| Object model | `ParamsController/Objects/` | `Param.inc` (Array + free-list storage), `ParamType.inc` (Trie-keyed type registry) |
| Built-in types | `ParamsController/DefaultObjects/ParamType/` | 14 type handlers: Boolean, Integer, Float, String, RGB, Model, Sound, Resource, File, Dir, ChatMessage, Time, TimeInterval, WeekDay, Flags |
| Placeholders | `ParamsController/Placeholders/` | Global and per-player placeholder substitution system |
| Registrars | `ParamsController/DefaultObjects/Registrar.inc`, `PlaceholderRegistrar.inc` | Wire up default types and placeholders on plugin load |
| Forwards | `ParamsController/Forwards.inc` | Generic forward registration utility used throughout |

### Key design patterns

- **Parameter storage:** Array-based with a free-list for handle reuse (`Param.inc`)
- **Type registry:** Trie-keyed by type name string (`ParamType.inc`)
- **Extensibility:** `CreateMultiForward` / `ExecuteForward` — consumers can register custom parameter types and placeholder groups via natives
- **Error reporting:** `E_ParamsReadErrorType` enum returned from `ReadList`; error details (param name, expected/got type) accessible via `ParamsController_GetErrorInfo()`

### Language notes

PAWN uses `#include` for all modular decomposition. `.inc` files are not independently compiled — they are all included into `ParamsController.sma`. The public header (`ParamsController.inc`) only declares natives/forwards; it contains no implementation.
