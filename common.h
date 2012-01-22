#pragma once

#define ENABLE_ASSERTIONS 1
#if ENABLE_ASSERTIONS
#define ASSERT(c, s) do {if (!(c)) panic(s);} while (0)
#else
#define ASSERT(c, s) do {} while (0)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef __SIZE_TYPE__ size_t;

void outb(u16 port, u8 value);
u8 inb(u16 port);
u16 inw(u16 port);

void *raw_malloc(size_t len);
void raw_free(void *start, size_t len);
void *kmalloc(size_t len);
void kfree(void *start);

void kmemcpy(void *dst, const void *src, size_t len);
void kmemset(void *mem, int value, size_t num);
int kstrcmp(const char *s1, const char *s2);
size_t kstrlen(const char *s);

void cls();
void newline();
void kputchar(int c);
void kputs(const char *s);
void kprintf(const char *format, ...);

void itoa(char *buf, int base, int d);

u32 randu32();
u32 randu32_limit(u32 upper);
u32 randu32_range(u32 lower, u32 upper);

void panic(const char *message);
