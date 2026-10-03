#include "arwing_cheats.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Keys indexed by Cheat: 5 God Mode, 6 Rapid Fire, 7 Infinite Bombs, 8 Complete Rings.
   Each cheat accepts its number-row key or the matching numpad key. */
static const int kCheatKeys[CHEAT_COUNT] = {'5', '6', '7', '8'};
static const int kCheatNumpadKeys[CHEAT_COUNT] = {VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8};

static const FhModHost* H;
static FhMod* M;
static int sKeyDown[CHEAT_COUNT];

void modLog(FhLogLevel level, const char* format, ...) {
  char message[256];
  int prefix;
  va_list args;

  if (!H || !H->log || !M) return;
  prefix = snprintf(message, sizeof(message), "[Arwing Cheats] ");
  va_start(args, format);
  vsnprintf(message + prefix, sizeof(message) - (size_t)prefix, format, args);
  va_end(args);
  H->log(M, level, message);
}

static int game_window_focused(void) {
  HWND window = GetForegroundWindow();
  DWORD processId = 0;

  if (window == NULL) return 0;
  GetWindowThreadProcessId(window, &processId);
  return processId == GetCurrentProcessId();
}

static int key_down(int key) {
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

FH_MOD_EXPORT int fh_mod_initialize(FhMod* mod, const FhModHost* host) {
  if (!host || host->abiVersion != FH_MOD_ABI_VERSION || host->structSize < sizeof(FhModHost)) return FH_MOD_ERROR;
  if (!host->log || !host->symbolAddress || !host->hookInstall || !host->hookRemove) return FH_MOD_ERROR;
  H = host;
  M = mod;
  if (!arwingHooksInstall(mod, host)) {
    arwingHooksRemove(mod, host);
    modLog(FH_LOG_ERROR, "disabled: required host symbols or hooks are unavailable");
    return FH_MOD_ERROR;
  }
  modLog(FH_LOG_INFO, "v1.0.1 loaded (5 God Mode, 6 Rapid Fire, 7 Infinite Bombs, 8 Complete Rings)");
  return FH_MOD_OK;
}

FH_MOD_EXPORT void fh_mod_update(FhMod* mod) {
  int pressed[CHEAT_COUNT];
  int focused = game_window_focused();
  int i;
  (void)mod;

  /* Key state is tracked while unfocused too, so a key already held when the
     game regains focus is not seen as a new press. */
  for (i = 0; i < CHEAT_COUNT; i++) {
    int down = key_down(kCheatKeys[i]) || key_down(kCheatNumpadKeys[i]);

    pressed[i] = focused && down && !sKeyDown[i];
    sKeyDown[i] = down;
  }
  arwingCheatsUpdate(pressed);
}

FH_MOD_EXPORT void fh_mod_shutdown(FhMod* mod) {
  (void)mod;
  if (H && M) arwingHooksRemove(M, H);
  arwingCheatsReset();
  memset(sKeyDown, 0, sizeof(sKeyDown));
  H = 0;
  M = 0;
}
