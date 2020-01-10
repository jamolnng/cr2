MEMORY
{
  flash (rxai!w) : ORIGIN = 0x20010000, LENGTH = 0x6a120
  itim (wx!rai) : ORIGIN = 0x8000000, LENGTH = 0x2000
  ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 0x4000
}

PHDRS
{
  flash PT_LOAD;
  ram PT_LOAD;
  ram_init PT_LOAD;
  itim PT_LOAD;
  itim_init PT_LOAD;
}