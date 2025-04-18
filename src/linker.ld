OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    . = 0x100000;
    .text   BLOCK(4K) : ALIGN(4K) { *(.multiboot) *(.text) }
    .rodata BLOCK(4K) : ALIGN(4K) { *(.data) }
    .data   BLOCK(4K) : ALIGN(4K) { *(.data) }
    .bss    BLOCK(4K) : ALIGN(4K) { *(.bss) }
    .init_array : {
        __init_array = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP(*(.init_array .ctors))
        __init_array_end = .;
    }
}