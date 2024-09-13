#pragma once

/* Enter critical section, disables interrupts */
#define ENTER_CRITICAL() u32 flags_ ## __func__; asm volatile("pushf; cli; pop %0" : "=r"(flags_ ## __func__) :: "memory", "cc")

/* Leave critical section, re-enables interrupts if they were on when entering */
#define LEAVE_CRITICAL() asm volatile("push %0; popf" :: "rm"(flags_ ## __func__) : "memory", "cc")