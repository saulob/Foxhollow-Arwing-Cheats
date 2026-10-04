#define _GNU_SOURCE

#include "platform_input.h"
#include "arwing_cheats.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stddef.h>

/* SDL_Scancode values from Foxhollow's SDL3 (SDL_scancode.h). */
#define SDL3_SCANCODE_5 34
#define SDL3_SCANCODE_6 35
#define SDL3_SCANCODE_7 36
#define SDL3_SCANCODE_8 37
#define SDL3_SCANCODE_KP_5 93
#define SDL3_SCANCODE_KP_6 94
#define SDL3_SCANCODE_KP_7 95
#define SDL3_SCANCODE_KP_8 96

/* Scancodes indexed by Cheat: 5 God Mode, 6 Rapid Fire, 7 Infinite Bombs, 8 Complete Rings.
   Each cheat accepts its number-row key or the matching numpad key. */
static const int kCheatScancodes[CHEAT_COUNT] = {SDL3_SCANCODE_5, SDL3_SCANCODE_6, SDL3_SCANCODE_7, SDL3_SCANCODE_8};
static const int kCheatKeypadScancodes[CHEAT_COUNT] = {SDL3_SCANCODE_KP_5, SDL3_SCANCODE_KP_6, SDL3_SCANCODE_KP_7,
                                                       SDL3_SCANCODE_KP_8};

typedef const bool* (*SdlGetKeyboardStateFn)(int* numkeys);
typedef void* (*SdlGetKeyboardFocusFn)(void);

static SdlGetKeyboardStateFn sGetKeyboardState;
static SdlGetKeyboardFocusFn sGetKeyboardFocus;

static void* resolve_sdl_symbol(FhMod* mod, const FhModHost* host, const char* name) {
  void* address = dlsym(RTLD_DEFAULT, name);

  if (address == NULL) {
    address = host->symbolAddress(mod, name);
  }
  if (address == NULL) {
    modLog(FH_LOG_ERROR, "could not resolve %s", name);
  }
  return address;
}

int platformInputInitialize(FhMod* mod, const FhModHost* host) {
  const bool* keys;
  int count = 0;

  sGetKeyboardState = (SdlGetKeyboardStateFn)resolve_sdl_symbol(mod, host, "SDL_GetKeyboardState");
  sGetKeyboardFocus = (SdlGetKeyboardFocusFn)resolve_sdl_symbol(mod, host, "SDL_GetKeyboardFocus");
  if (sGetKeyboardState == NULL || sGetKeyboardFocus == NULL) {
    platformInputShutdown();
    return 0;
  }
  keys = sGetKeyboardState(&count);
  if (keys == NULL || count <= SDL3_SCANCODE_KP_8) {
    modLog(FH_LOG_ERROR, "SDL keyboard state is unavailable");
    platformInputShutdown();
    return 0;
  }
  return 1;
}

void platformInputShutdown(void) {
  sGetKeyboardState = NULL;
  sGetKeyboardFocus = NULL;
}

int platformInputActive(void) {
  return sGetKeyboardFocus != NULL && sGetKeyboardFocus() != NULL;
}

static int key_down(const bool* keys, int count, int scancode) {
  return keys != NULL && scancode < count && keys[scancode];
}

/* Keys are read by physical position: the number-row 5-8 keys and numpad 5-8.
   Scancodes ignore Shift and Num Lock, so numpad 5-8 work with Num Lock on or
   off. Both keys of a cheat are one control: it is down while either key is
   down. SDL clears this state when the window loses focus and restores only
   modifier keys when it regains it, so a key held across a focus change reads
   as up until it is pressed again. */
int platformCheatKeyDown(int cheat) {
  const bool* keys;
  int count = 0;

  if (sGetKeyboardState == NULL || cheat < 0 || cheat >= CHEAT_COUNT) return 0;
  keys = sGetKeyboardState(&count);
  return key_down(keys, count, kCheatScancodes[cheat]) || key_down(keys, count, kCheatKeypadScancodes[cheat]);
}
