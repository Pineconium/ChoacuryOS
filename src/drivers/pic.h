#pragma once

#include "types.h"
#include <stdbool.h>

/* Initialize PIC. This has to be called once during kernel initialization */
void pic_init();

/* Unmask interrupt from PIC. Calling this allows hardware to trigger irq specified */
void pic_unmask(u8 irq);

/* Used to send end of interrupt to PIC */
void pic_send_eoi(u8 irq);

bool pic_is_in_service(u8 irq);
