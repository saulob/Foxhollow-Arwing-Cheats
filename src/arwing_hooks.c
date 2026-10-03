#include "arwing_cheats.h"

#include <string.h>

/* Counts the cheat menu kept the Arwing at. */
#define ARWING_BOMBS 3
#define ARWING_RINGS 10
#define PAD_BUTTON_A 0x0100
#define PAD_BUTTON_B 0x0200

typedef struct Symbol {
  const char* name;
  void** address;
} Symbol;

typedef void (*HookFn)(void);

typedef struct Hook {
  const char* name;
  HookFn replacement;
  void** original;
  void* target;
} Hook;

ArwingCheatsGame game;

static GameObject* sDamageArwing;
static int sMaskBombButton;
static int sBombLaunched;
static uint32_t sBombLaunchRetrace;

static void (*origReadControls)(GameObject*, ArwingState*);
static void (*origHandlePathDamage)(GameObject*, ArwingState*);
static void (*origHandleObjectDamage)(GameObject*, ArwingState*);
static void (*origDoRumble)(float);
static void (*origCameraShakeSetOffset)(float);
static void (*origSpawnBomb)(GameObject*, ArwingState*, int);
static void (*origArwarwingboUpdate)(GameObject*);
static uint32_t (*origGetButtonsJustPressed)(int);

/* arwarwing_update calls this once per frame of normal flight, before the
   weapons run, so it is where the bomb and ring counts are kept topped up.
   Rapid Fire then turns a held A into a fresh press each frame; the native
   fire cooldown still decides when the next shot leaves. */
static void hookReadControls(GameObject* obj, ArwingState* state) {
  if (gCheatOn[CHEAT_COMPLETE_RINGS]) {
    state->collectedRings = ARWING_RINGS;
  }
  if (gCheatOn[CHEAT_INFINITE_BOMBS] && state->bombCount < ARWING_BOMBS) {
    state->bombCount = ARWING_BOMBS;
  }
  origReadControls(obj, state);
  if (gCheatOn[CHEAT_RAPID_FIRE] && (state->inputFlags2 & PAD_BUTTON_A) != 0) {
    state->inputFlags |= PAD_BUTTON_A;
  }
}

/* God Mode. Both damage handlers lower the health inline and test it for
   death a few instructions later, so the health is refilled in between, at
   the same points the cheat menu used: right before the doRumble that follows
   path damage, and right after the hit block of object damage, whose last call
   is CameraShake_SetOffset. Those two hooks only act while a handler runs. */

static void refill_health(GameObject* arwing) {
  int health = game.arwarwing_getHealth(arwing);
  int maxHealth = game.arwarwing_getMaxHealth(arwing);

  if (health < maxHealth) {
    game.arwarwing_addHealth(arwing, maxHealth - health);
  }
}

static void run_damage_handler(void (*handler)(GameObject*, ArwingState*), GameObject* obj, ArwingState* state) {
  GameObject* outer = sDamageArwing;

  sDamageArwing = NULL;
  if (gCheatOn[CHEAT_GOD_MODE] && obj != NULL) {
    refill_health(obj);
    sDamageArwing = obj;
  }
  handler(obj, state);
  sDamageArwing = outer;
}

static void hookHandlePathDamage(GameObject* obj, ArwingState* state) {
  run_damage_handler(origHandlePathDamage, obj, state);
}

static void hookHandleObjectDamage(GameObject* obj, ArwingState* state) {
  run_damage_handler(origHandleObjectDamage, obj, state);
}

static void hookDoRumble(float duration) {
  if (sDamageArwing != NULL) {
    refill_health(sDamageArwing);
  }
  origDoRumble(duration);
}

static void hookCameraShakeSetOffset(float offsetY) {
  if (sDamageArwing != NULL) {
    refill_health(sDamageArwing);
  }
  origCameraShakeSetOffset(offsetY);
}

/* Infinite Bombs keeps the two safety checks the cheat menu added to the bomb:
   the B press that launches a bomb does not also detonate it when the new bomb
   runs its first update in the same frame, and a hit from another live Arwing
   bomb (the previous one's blast) does not detonate a new bomb. */

static void hookSpawnBomb(GameObject* obj, ArwingState* state, int side) {
  uint8_t bombs = state->bombCount;

  origSpawnBomb(obj, state, side);
  /* spawnBomb spends a bomb only when it launches one. */
  if (gCheatOn[CHEAT_INFINITE_BOMBS] && state->bombCount != bombs) {
    sBombLaunchRetrace = game.VIGetRetraceCount();
    sBombLaunched = 1;
  }
}

static int is_live_object(const GameObject* obj) {
  int start = 0;
  int count = 0;
  GameObject** objects = game.ObjList_GetObjects(&start, &count);
  int i;

  if (objects == NULL) return 0;
  for (i = start; i < count; i++) {
    if (objects[i] == obj) return 1;
  }
  return 0;
}

static void hookArwarwingboUpdate(GameObject* bomb) {
  int outer = sMaskBombButton;
  ObjHitsPriorityState* hits;

  sMaskBombButton = 0;
  if (gCheatOn[CHEAT_INFINITE_BOMBS] && bomb != NULL) {
    /* Obj_UpdateObject clears lastHitObject right after this update, so
       dropping an ignored hit here only takes it out of the detonation test. */
    hits = bomb->hitReactState;
    if (hits != NULL && hits->lastHitObject != NULL && is_live_object(hits->lastHitObject) &&
        hits->lastHitObject->romDefNo == bomb->romDefNo) {
      hits->lastHitObject = NULL;
    }
    sMaskBombButton = sBombLaunched && game.VIGetRetraceCount() == sBombLaunchRetrace;
  }
  origArwarwingboUpdate(bomb);
  sMaskBombButton = outer;
}

/* Only masks B for the detonation test of a bomb updating in its launch frame. */
static uint32_t hookGetButtonsJustPressed(int port) {
  uint32_t buttons = origGetButtonsJustPressed(port);

  return sMaskBombButton ? buttons & ~(uint32_t)PAD_BUTTON_B : buttons;
}

static const Symbol kCoreSymbols[] = {
    {"getGameState", (void**)&game.getGameState},
    {"getCurUiDll", (void**)&game.getCurUiDll},
    {"getSaveGameLoadStatus", (void**)&game.getSaveGameLoadStatus},
    {"Obj_GetPlayerObject", (void**)&game.Obj_GetPlayerObject},
    {"getArwing", (void**)&game.getArwing},
};

static const Symbol kGodModeSymbols[] = {
    {"arwarwing_getHealth", (void**)&game.arwarwing_getHealth},
    {"arwarwing_getMaxHealth", (void**)&game.arwarwing_getMaxHealth},
    {"arwarwing_addHealth", (void**)&game.arwarwing_addHealth},
};

static const Symbol kBombSymbols[] = {
    {"VIGetRetraceCount", (void**)&game.VIGetRetraceCount},
    {"ObjList_GetObjects", (void**)&game.ObjList_GetObjects},
};

/* Shared by Rapid Fire, Infinite Bombs and Complete Rings. */
static Hook sFlightHooks[] = {
    {"arwarwing_readControls", (HookFn)hookReadControls, (void**)&origReadControls, NULL},
};

static Hook sGodModeHooks[] = {
    {"arwarwing_handlePathDamage", (HookFn)hookHandlePathDamage, (void**)&origHandlePathDamage, NULL},
    {"arwarwing_handleObjectDamage", (HookFn)hookHandleObjectDamage, (void**)&origHandleObjectDamage, NULL},
    {"doRumble", (HookFn)hookDoRumble, (void**)&origDoRumble, NULL},
    {"CameraShake_SetOffset", (HookFn)hookCameraShakeSetOffset, (void**)&origCameraShakeSetOffset, NULL},
};

static Hook sBombHooks[] = {
    {"arwarwing_spawnBomb", (HookFn)hookSpawnBomb, (void**)&origSpawnBomb, NULL},
    {"arwarwingbo_update", (HookFn)hookArwarwingboUpdate, (void**)&origArwarwingboUpdate, NULL},
    {"getButtonsJustPressed", (HookFn)hookGetButtonsJustPressed, (void**)&origGetButtonsJustPressed, NULL},
};

#define COUNT_OF(array) ((int)(sizeof(array) / sizeof((array)[0])))

static int resolve_symbols(FhMod* mod, const FhModHost* host, const Symbol* symbols, int count, FhLogLevel level) {
  int ok = 1;
  int i;

  for (i = 0; i < count; i++) {
    *symbols[i].address = host->symbolAddress(mod, symbols[i].name);
    if (*symbols[i].address == NULL) {
      modLog(level, "could not resolve %s", symbols[i].name);
      ok = 0;
    }
  }
  return ok;
}

/* A hook the host fails to remove keeps its original, so the still patched
   entry passes straight through while every cheat is off. */
static void remove_hooks(FhMod* mod, const FhModHost* host, Hook* hooks, int count) {
  int i;

  for (i = count - 1; i >= 0; i--) {
    if (hooks[i].target == NULL) continue;
    if (host->hookRemove(mod, hooks[i].target) != FH_MOD_OK) {
      modLog(FH_LOG_WARN, "could not unhook %s", hooks[i].name);
      continue;
    }
    hooks[i].target = NULL;
    *hooks[i].original = NULL;
  }
}

static int install_hooks(FhMod* mod, const FhModHost* host, Hook* hooks, int count) {
  int i;

  for (i = 0; i < count; i++) {
    void* target = host->symbolAddress(mod, hooks[i].name);

    if (target == NULL) {
      modLog(FH_LOG_WARN, "could not resolve %s", hooks[i].name);
      remove_hooks(mod, host, hooks, count);
      return 0;
    }
    if (host->hookInstall(mod, target, (void*)hooks[i].replacement, hooks[i].original) != FH_MOD_OK) {
      modLog(FH_LOG_WARN, "could not hook %s (no patch pad, or another mod already hooked it)", hooks[i].name);
      remove_hooks(mod, host, hooks, count);
      return 0;
    }
    hooks[i].target = target;
  }
  return 1;
}

int arwingHooksInstall(FhMod* mod, const FhModHost* host) {
  int flight;
  int godMode;
  int bombs;
  int available = 0;
  int i;

  if (!resolve_symbols(mod, host, kCoreSymbols, COUNT_OF(kCoreSymbols), FH_LOG_ERROR)) {
    return 0;
  }
  flight = install_hooks(mod, host, sFlightHooks, COUNT_OF(sFlightHooks));
  godMode = resolve_symbols(mod, host, kGodModeSymbols, COUNT_OF(kGodModeSymbols), FH_LOG_WARN) &&
            install_hooks(mod, host, sGodModeHooks, COUNT_OF(sGodModeHooks));
  bombs = flight && resolve_symbols(mod, host, kBombSymbols, COUNT_OF(kBombSymbols), FH_LOG_WARN) &&
          install_hooks(mod, host, sBombHooks, COUNT_OF(sBombHooks));

  gCheatAvailable[CHEAT_GOD_MODE] = (unsigned char)godMode;
  gCheatAvailable[CHEAT_RAPID_FIRE] = (unsigned char)flight;
  gCheatAvailable[CHEAT_INFINITE_BOMBS] = (unsigned char)bombs;
  gCheatAvailable[CHEAT_COMPLETE_RINGS] = (unsigned char)flight;
  for (i = 0; i < CHEAT_COUNT; i++) {
    if (gCheatAvailable[i]) {
      available++;
    } else {
      modLog(FH_LOG_WARN, "%s unavailable: required game symbols or hooks are missing", gCheatNames[i]);
    }
  }
  return available > 0;
}

void arwingHooksRemove(FhMod* mod, const FhModHost* host) {
  remove_hooks(mod, host, sBombHooks, COUNT_OF(sBombHooks));
  remove_hooks(mod, host, sGodModeHooks, COUNT_OF(sGodModeHooks));
  remove_hooks(mod, host, sFlightHooks, COUNT_OF(sFlightHooks));
  memset(gCheatAvailable, 0, sizeof(gCheatAvailable));
  arwingHooksClearState();
  memset(&game, 0, sizeof(game));
}

void arwingHooksClearState(void) {
  sDamageArwing = NULL;
  sMaskBombButton = 0;
  sBombLaunched = 0;
  sBombLaunchRetrace = 0;
}
