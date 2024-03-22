#include "panic.h"
#include "../drivers/vga.h"

__attribute__((noreturn))
void panic_impl(const char* location_prefix, const char* message)
{
	k_printf(location_prefix, 0, TC_LRED);
	k_printf(message, 1, TC_LRED);

	asm volatile("cli");
	for (;;) {
		asm volatile("hlt");
	}
	__builtin_unreachable();
}
