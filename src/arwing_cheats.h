#ifndef ARWING_CHEATS_H_
#define ARWING_CHEATS_H_

#include <stddef.h>
#include <stdint.h>

#include "foxhollow_mod_api.h"

/* Partial native 64-bit layouts of the game records the hooks touch, the same
   on every supported Foxhollow target. The game headers' STATIC_ASSERT offsets
   describe the 32-bit GameCube layout, so these offsets come from the headers
   compiled for the 64-bit Foxhollow build and match what arwarwing_readControls,
   arwarwing_getBombCount, arwarwing_getCollectedRingCount and arwarwingbo_update
   read and write in the Foxhollow build. */
typedef struct GameObject GameObject;

typedef struct ObjHitsPriorityState {
  uint8_t pad00[0x50];
  GameObject* lastHitObject;
} ObjHitsPriorityState;

struct GameObject {
  uint8_t pad000[0x52];
  int16_t romDefNo;
  uint8_t pad054[0x14];
  ObjHitsPriorityState* hitReactState;
};

typedef struct ArwingState {
  uint8_t pad000[0x43C];
  uint16_t inputFlags;
  uint16_t inputFlagsPrev;
  uint16_t inputFlags2;
  uint8_t pad442[0x5E];
  uint8_t bombCount;
  uint8_t pad4A1[0x3B];
  uint8_t collectedRings;
} ArwingState;

_Static_assert(offsetof(ObjHitsPriorityState, lastHitObject) == 0x50, "ObjHitsPriorityState.lastHitObject");
_Static_assert(offsetof(GameObject, romDefNo) == 0x52, "GameObject.anim.romDefNo");
_Static_assert(offsetof(GameObject, hitReactState) == 0x68, "GameObject.anim.hitReactState");
_Static_assert(offsetof(ArwingState, inputFlags) == 0x43C, "ArwingState.inputFlags");
_Static_assert(offsetof(ArwingState, inputFlags2) == 0x440, "ArwingState.inputFlags2");
_Static_assert(offsetof(ArwingState, bombCount) == 0x4A0, "ArwingState.bombCount");
_Static_assert(offsetof(ArwingState, collectedRings) == 0x4DC, "ArwingState.collectedRings");

typedef struct ArwingCheatsGame {
  int (*getGameState)(void);
  int (*getCurUiDll)(void);
  int (*getSaveGameLoadStatus)(void);
  GameObject* (*Obj_GetPlayerObject)(void);
  GameObject* (*getArwing)(void);
  int (*arwarwing_getHealth)(GameObject* arwing);
  int (*arwarwing_getMaxHealth)(GameObject* arwing);
  void (*arwarwing_addHealth)(GameObject* arwing, int amount);
  uint32_t (*VIGetRetraceCount)(void);
  GameObject** (*ObjList_GetObjects)(int* startIndex, int* objectCount);
} ArwingCheatsGame;

typedef enum Cheat {
  CHEAT_GOD_MODE,
  CHEAT_RAPID_FIRE,
  CHEAT_INFINITE_BOMBS,
  CHEAT_COMPLETE_RINGS,
  CHEAT_COUNT
} Cheat;

extern ArwingCheatsGame game;
extern const char* const gCheatNames[CHEAT_COUNT];
extern unsigned char gCheatOn[CHEAT_COUNT];
extern unsigned char gCheatAvailable[CHEAT_COUNT];

void modLog(FhLogLevel level, const char* format, ...);

int arwingHooksInstall(FhMod* mod, const FhModHost* host);
void arwingHooksRemove(FhMod* mod, const FhModHost* host);
void arwingHooksClearState(void);

void arwingCheatsUpdate(const int pressed[CHEAT_COUNT]);
void arwingCheatsReset(void);

#endif
