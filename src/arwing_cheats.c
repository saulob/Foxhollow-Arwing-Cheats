#include "arwing_cheats.h"

#include <string.h>

#define GAME_STATE_RUNNING 1
#define UI_DLL_GAMEPLAY 1
#define UI_DLL_FRONTEND_FIRST 2
#define UI_DLL_FRONTEND_LAST 7

const char* const gCheatNames[CHEAT_COUNT] = {"God Mode", "Rapid Fire", "Infinite Bombs", "Complete Rings"};

unsigned char gCheatOn[CHEAT_COUNT];
unsigned char gCheatAvailable[CHEAT_COUNT];

static int sSessionActive;

static int gameplay_active(void) {
  return game.getGameState() == GAME_STATE_RUNNING && game.getCurUiDll() == UI_DLL_GAMEPLAY &&
         game.getSaveGameLoadStatus() == 0 && (game.Obj_GetPlayerObject() != NULL || game.getArwing() != NULL);
}

static int any_cheat_on(void) {
  int i;

  for (i = 0; i < CHEAT_COUNT; i++) {
    if (gCheatOn[i]) return 1;
  }
  return 0;
}

/* Cheats belong to the loaded save: they survive warps, loads and leaving the
   Arwing, and are cleared once the game stops running or a front-end screen
   (title, save select) takes over. */
static int update_session(void) {
  int uiDll;

  if (gameplay_active()) {
    sSessionActive = 1;
    return 1;
  }
  uiDll = game.getCurUiDll();
  if (sSessionActive && (game.getGameState() != GAME_STATE_RUNNING ||
                         (uiDll >= UI_DLL_FRONTEND_FIRST && uiDll <= UI_DLL_FRONTEND_LAST))) {
    sSessionActive = 0;
    arwingHooksClearState();
    if (any_cheat_on()) {
      memset(gCheatOn, 0, sizeof(gCheatOn));
      modLog(FH_LOG_INFO, "Save session ended, all cheats disabled");
    }
  }
  return 0;
}

static void toggle(Cheat cheat) {
  if (!gCheatAvailable[cheat]) {
    modLog(FH_LOG_WARN, "%s is unavailable in this Foxhollow build", gCheatNames[cheat]);
    return;
  }
  gCheatOn[cheat] = !gCheatOn[cheat];
  modLog(FH_LOG_INFO, "%s %s", gCheatNames[cheat], gCheatOn[cheat] ? "enabled" : "disabled");
}

void arwingCheatsUpdate(const int pressed[CHEAT_COUNT]) {
  int i;

  if (!update_session()) return;
  /* The keys only act while flying the Arwing. On foot they are ignored here
     without touching any state. */
  if (game.getArwing() == NULL) return;
  for (i = 0; i < CHEAT_COUNT; i++) {
    if (pressed[i]) toggle((Cheat)i);
  }
}

void arwingCheatsReset(void) {
  memset(gCheatOn, 0, sizeof(gCheatOn));
  memset(gCheatAvailable, 0, sizeof(gCheatAvailable));
  sSessionActive = 0;
}
