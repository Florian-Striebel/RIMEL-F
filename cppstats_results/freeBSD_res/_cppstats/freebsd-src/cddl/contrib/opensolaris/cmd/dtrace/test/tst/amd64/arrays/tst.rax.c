void frax(void);
__attribute__((optnone)) void
frax(void)
{
asm volatile("mov $0x41414141, %%rax"
: : : "rax"
);
}
int
main(void)
{
while (1) {
frax();
}
return (0);
}
