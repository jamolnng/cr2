__stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
PROVIDE(__stack_size = __stack_size);
__heap_size = DEFINED(__heap_size) ? __heap_size : 0x800;