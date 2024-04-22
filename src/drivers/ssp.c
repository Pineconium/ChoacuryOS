#include "types.h"
#include "../kernel/panic.h"

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0x7eef2b9e
#else
#define STACK_CHK_GUARD 0x14047e612c70ba90
#endif

/* FIXME: This should be randomized by the bootloader.
          As the the kernel is WIP and there are no attackers
		  this will find bugs and is much better than nothing. */
uptr __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_fail(void)
{
	panic("Stack smashing detected");
}

__attribute__((noreturn))
void __stack_chk_fail_local(void)
{
	__stack_chk_fail();
}
