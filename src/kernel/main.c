void printString(const char* str)
{
    while (*str != '\0')
    {
        asm volatile("int $0x10" : : "a"(0x0E00 | *str), "b"(0x0007));
        str++;
    }
}

void kernel_main()
{
    const char* message = "ChoacuryKernel!";
    printString(message);

    while (1)
    {
        asm volatile("hlt");
    }
}

