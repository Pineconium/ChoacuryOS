#include "panic.h"
#include "../drivers/vga.h"
#include "../shell/terminal.h"

__attribute__((noreturn))
void panic_impl(const char* location_prefix, const char* message)
{
	term_write(location_prefix, TC_LRED);
	term_putchar(' ', TC_LRED);
	term_write(message, TC_LRED);

	asm volatile("cli");
	for (;;) {
		asm volatile("hlt");
	}
	__builtin_unreachable();
}
