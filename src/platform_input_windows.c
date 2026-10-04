#include "platform_input.h"
#include "arwing_cheats.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Keys indexed by Cheat: 5 God Mode, 6 Rapid Fire, 7 Infinite Bombs, 8 Complete Rings.
   Each cheat accepts its number-row key or the matching numpad key. */
static const int kCheatKeys[CHEAT_COUNT] = {'5', '6', '7', '8'};
static const int kCheatNumpadKeys[CHEAT_COUNT] = {VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8};

int platformInputInitialize(FhMod* mod, const FhModHost* host) {
  (void)mod;
  (void)host;
  return 1;
}

void platformInputShutdown(void) {
}

int platformInputActive(void) {
  HWND window = GetForegroundWindow();
  DWORD processId = 0;

  if (window == NULL) return 0;
  GetWindowThreadProcessId(window, &processId);
  return processId == GetCurrentProcessId();
}

static int key_down(int key) {
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

/* Both keys of a cheat are one control: it is down while either key is down.
   Windows reports VK_NUMPAD5-8 only with Num Lock on. */
int platformCheatKeyDown(int cheat) {
  if (cheat < 0 || cheat >= CHEAT_COUNT) return 0;
  return key_down(kCheatKeys[cheat]) || key_down(kCheatNumpadKeys[cheat]);
}
