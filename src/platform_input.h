#ifndef PLATFORM_INPUT_H_
#define PLATFORM_INPUT_H_

#include "foxhollow_mod_api.h"

int platformInputInitialize(FhMod* mod, const FhModHost* host);
void platformInputShutdown(void);
int platformInputActive(void);
/* cheat is a Cheat index: 5 God Mode, 6 Rapid Fire, 7 Infinite Bombs, 8 Complete Rings. */
int platformCheatKeyDown(int cheat);

#endif
