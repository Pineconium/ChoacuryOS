ENTRY(start)
OUTPUT_FORMAT(elf32-i386)
SECTIONS {
    . = 0x100000;

    .text ALIGN(4096) : {
        *(.text)
    }

    .data ALIGN(4096) : {
        *(.data)
    }

    .bss ALIGN(4096) : {
        *(.bss)
    }
}
