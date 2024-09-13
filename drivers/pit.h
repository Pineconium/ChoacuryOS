#pragma once

#include "types.h"

void pit_init();
void pit_sleep_ms(u64);
u64 pit_current_time_ms();
