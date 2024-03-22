#pragma once

#define __panic_stringify_helper(s) #s
#define __panic_stringify(s) __panic_stringify_helper(s)

#define panic(message) panic_impl("kernel panic at " __FILE__ ":" __panic_stringify(__LINE__), message)

__attribute__((noreturn))
void panic_impl(const char* location_prefix, const char* message);
