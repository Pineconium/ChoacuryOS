OUTPUT_FORMAT("binary")
ENTRY(start)

SECTIONS {
   . = 0x7c00;

   bootloader 16K : {
       bootloader.obj
   }

   kernel : {
       kernel.obj
   }
}

