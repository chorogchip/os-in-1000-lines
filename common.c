#include "common.h"

void putchar(char ch);



void* memset(void* buf, char c, size_t n) {
    uint8_t *p = (uint8_t*)buf;
    while (n--) *p++ = c;
    return buf;
}

void* memcpy(void* dst, const void* src, size_t n) {
    uint8_t *d = (uint8_t*)dst;
    const uint8_t *s = (const uint8_t*)src;
    while (n--)
        *d++ = *s++;
    return dst;
}

char* strcpy(char* dst, const char* src) {
    char *d = dst;
    while (*src)
        *d++ = *src++;
    *d = '\0';
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) break;
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void putc(char ch) {
    putchar(ch);    
}

void puts(char* str) {
    while (*str)
        putchar(*str++);
}

void puti(int32_t i) {
    uint64_t buf = 0;
    uint64_t u = (uint64_t)(uint32_t)i;
    if (i < 0) {
        putc('-');
        u = (uint64_t)-(uint32_t)i;
    }
    for (uint64_t i = 0; i < 9; ++i) {
        buf |= (uint64_t)(u % 10) << (i << 2);
        u /= 10;
    }
    uint32_t k = 8;
    if (u > 0)
        putc('0' + (int)u);
    else
        while (k > 0 && !(buf & (0xfULL << (k << 2)))) --k;

    for (; (int32_t)k >= 0; --k) {
        putc('0' + (int)(0xfULL & (
            buf >> (k<<2)
        )));
    }
}

void putu(uint32_t u) {
    uint64_t buf = 0;

    for (uint64_t i = 0; i < 9; ++i) {
        buf |= ((uint64_t)(u % 10)) << (i << 2);
        u /= 10;
    }

    uint32_t k = 8;
    if (u > 0)
        putc('0' + (int)u);
    else
        while (k > 0 && !(buf & (0xfULL << (k << 2)))) --k;

    for (; (int32_t)k >= 0; --k) {
        putc('0' + (int)(0xfULL & (buf >> (k << 2))));
    }
}

void putx(uint32_t x) {
    uint32_t k = 7;
    for (; k > 0 && !(x & (0xfU << (k << 2))); --k);
    for (; (int32_t)k >= 0; --k)
        putc("0123456789abcdef"[0xfU & (x >> (k << 2))]);
}

void printf(const char* fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case '\0': putchar('%'); goto end;
                case '%': putchar('%'); break;
                case 's': {
                    const char* s = va_arg(vargs, const char*);
                    while (*s) putchar(*s++);
                    break;
                }
                case 'd': {
                    int value = va_arg(vargs, int);
                    puti(value);
                    break;
                }
                case 'x': {
                    unsigned value = va_arg(vargs, unsigned);
                    putx(value);
                    break;
                }
            }
        } else {
            putchar(*fmt);
        }
        fmt++;
    }
end:
    va_end(vargs);
}
