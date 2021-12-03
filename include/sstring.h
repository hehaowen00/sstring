#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
All non sstr_t strings must specify their length
The length of a non sstr_t string is defined as the number of sequential 
non zero characters contained in the block

Functions which (re)allocate memory have a return type of int8_t
0 if function was successful 
1 if function failed

Assumes a 64-bit platform
*/

#ifndef SSTRING_H
#define SSTRING_H

typedef char* sstr_t;

sstr_t sstr_new(size_t cap);
sstr_t sstr_from(const char* str, size_t len);
sstr_t sstr_clone(sstr_t s);

void sstr_free(sstr_t s);
int8_t sstr_reserve(sstr_t* s, size_t additional);
int8_t sstr_shrink_to_fit(sstr_t* s);

void sstr_clear(sstr_t s);
void sstr_debug(sstr_t s);

size_t sstr_count(const char* str);
size_t sstr_cap(sstr_t s);
size_t sstr_len(sstr_t s);

void sstr_to_lower(sstr_t s);
void sstr_to_upper(sstr_t s);

int8_t sstr_format(sstr_t* s, const char* fmt, ...);
int8_t sstr_insert(sstr_t* s, size_t index, const char* str, size_t len_s);

int8_t sstr_compare(sstr_t a, const char* b, size_t len_b);
int8_t sstr_find(sstr_t s, size_t* index, size_t start, const char* a, size_t len_a);
int8_t sstr_replace(
        sstr_t *s,
        size_t start,
        const char* a,
        size_t len_a,
        const char* b,
        size_t len_b);

int8_t sstr_pad_center(sstr_t* s, char ch, size_t n);
int8_t sstr_pad_left(sstr_t* s, char ch, size_t n);
int8_t sstr_pad_right(sstr_t* s, char ch, size_t n);

void sstr_trim_left(sstr_t s);
void sstr_trim_right(sstr_t s);

int8_t sstr_concat(sstr_t* s, const char* a, size_t len_a); 
int8_t sstr_prepend(sstr_t* s, const char* a, size_t len_b);
void sstr_remove(sstr_t s, size_t offset, size_t n);

char sstr_pop(sstr_t s);
int8_t sstr_push(sstr_t* s, char ch);

#endif // SSTRING_H

#ifndef SSTRING_IMPL
#define SSTRING_IMPL

void* sstr_ptr_(sstr_t s);
void sstr_set_cap(sstr_t s, size_t cap);
void sstr_set_len(sstr_t s, size_t len);

size_t pow2(size_t n);
void shift(void* dest, void * src, size_t len);
void zero(void* p, size_t n);

sstr_t sstr_new(size_t cap)
{
    cap = pow2(cap + 1);
    size_t* ptr = calloc((sizeof(size_t) * 2) + (sizeof(char) * cap), 1);

    ptr[0] = cap;
    ptr[1] = 0;

    return (char*)ptr + (sizeof(size_t) * 2);
}

sstr_t sstr_from(const char* str, size_t len)
{
    sstr_t s = sstr_new(len + 1);
    for (size_t i = 0; i <= len; i++)
    {
        sstr_push(&s, str[i]);
    }
    sstr_push(&s, '\0');

    return s;
}

void sstr_free(sstr_t s)
{
    free(sstr_ptr_(s));
}

int8_t sstr_reserve(sstr_t* s, size_t additional)
{
    if (!additional)
    {
        return 0;
    }

    size_t cap = sstr_cap(*s);
    size_t new_cap = pow2(cap + additional);
    size_t* ptr = realloc(sstr_ptr_(*s), sizeof(size_t) * 2 + new_cap);

    if (!ptr) 
    {
        return 1;
    }
    
    *s = (char*)ptr + (sizeof(size_t) * 2);
    sstr_set_cap(*s, new_cap);

    return 0;
}

int8_t sstr_shrink_to_fit(sstr_t* s)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);
    
    while (cap / 2 > len)
    {
        cap /= 2;
    }

    size_t* ptr = realloc(sstr_ptr_(*s), sizeof(size_t) * 2 + cap);
    if (!ptr)
    {
        return 1;
    }

    *s = (char*)ptr + (sizeof(size_t) * 2);
    sstr_set_cap(*s, cap);

    return 0;
}

void sstr_clear(sstr_t s)
{
    for (size_t i = 0; i < sstr_len(s); i++)
    {
        s[i] = '\0';
    }

    sstr_set_len(s, 0);
}

void sstr_debug(sstr_t s)
{
    size_t cap = sstr_cap(s);
    size_t len = sstr_len(s);
    printf("{ sstr(%p): \"%s\", cap: %lu, len: %lu }\n", (void*)s, s, cap, len);
}

size_t sstr_count(const char* s)
{
    const char* end = s;
    while (*end++);
    return (end - s - 1);
}

size_t sstr_cap(sstr_t s)
{
    size_t* ptr = (size_t*)sstr_ptr_(s);
    return *ptr;
}

size_t sstr_len(sstr_t s)
{
    size_t* ptr = (size_t*)(s - (sizeof(size_t)));
    return *ptr;
}

void* sstr_ptr_(sstr_t s)
{
    return (s - (sizeof(size_t) * 2));
}

void sstr_set_cap(sstr_t s, size_t len)
{
    size_t* ptr = (size_t*)sstr_ptr_(s);
    *ptr = len;
}

void sstr_set_len(sstr_t s, size_t len)
{
    size_t* ptr = (size_t*)(s - (sizeof(size_t)));
    *ptr = len;
}

void sstr_to_lower(sstr_t s)
{
    size_t len = sstr_len(s);
    for (size_t i = 0; i < len; i++)
    {
        s[i] = tolower(s[i]);
    }
}

void sstr_to_upper(sstr_t s)
{
    size_t len = sstr_len(s);
    for (size_t i = 0; i < len; i++)
    {
        s[i] = toupper(s[i]);
    }
}

int8_t sstr_format(sstr_t* s, const char* fmt, ...)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    va_list args, copy;

    va_start(args, fmt);
    va_copy(copy, args);

    int32_t bytes = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);

    if (len + bytes > cap)
    {
        if (sstr_reserve(s, bytes - (cap - len)))
        {
            return 1;
        }
    }

    vsnprintf(*s + len, bytes + 1, fmt, args);
    va_end(args);

    sstr_set_len(*s, len + bytes);

    return 0;
}

int8_t sstr_insert(sstr_t* s, size_t index, const char* str, size_t len_s)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (!len_s)
    {
        return 0;
    }

    if (index > len)
    {
        return 1;
    }

    if (len + len_s > cap)
    {
        if (sstr_reserve(s, len + len_s - cap + 1))
        {
            return 1;
        }
    }

    shift((*s) + index + len_s, (*s) + index, len - index);

    for (size_t i = 0; i < len_s; i++)
    {
        (*s)[index + i] = str[i];
    }

    sstr_set_len(*s, len + len_s);

    return 0;
}

int8_t sstr_compare(sstr_t a, const char* b, size_t len_b)
{
    size_t len_a = sstr_len(a);
    size_t min = (len_a < len_b) ? len_a : len_b;

    for (size_t i = 0; i < min; i++)
    {
        if (a[i] > b[i])
        {
            return 1;
        }
        if (a[i] < b[i])
        {
            return -1;
        }
    }

    if (len_a > len_b)
    {
        return 1;
    }

    if (len_b > len_a)
    {
        return -1;
    }

    return 0;
}

int8_t sstr_find(sstr_t s, size_t* index, size_t start, const char* a, size_t len_a)
{
    size_t len = sstr_len(s);
    
    if (start > len - len_a)
    {
        return 1;
    }

    for (size_t i = start; i < len - len_a + 1; i++)
    {
        if (s[i] == a[0])
        {
            for (size_t j = 0; j < len_a; j++)
            {
                if (s[i + j] != a[j])
                {
                    break;
                }

                if (j + 1 == len_a)
                {
                    *index = i;
                    return 0;
                }
            }
        }
    }

    return 1;
}

int8_t sstr_replace(sstr_t*s, size_t start, const char* a, size_t len_a, const char* b, size_t len_b)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (start > len - len_a)
    {
        return 1;
    }

    size_t idx = 0;

    if (sstr_find(*s, &idx, start, a, len_a))
    {
        return 1;
    }

    ssize_t required = (len - len_a + len_b);

    if ((size_t)required > cap)
    {
        if (sstr_reserve(s, required - cap))
        {
            return 1;
        }
    }

    cap = sstr_cap(*s);

    if (!len_b)
    {
        sstr_remove(*s, idx, len_a);
        return 0;
    }

    shift((*s) + idx + len_b, (*s) + idx + len_a, len - len_a); 

    if (len_b < len_a)
    {
        zero(*s + len - len_a + len_b, len_a - len_b);
    }

    for (size_t i = 0; i < len_b; i++)
    {
        (*s)[idx + i] = b[i];
    }

    sstr_set_len(*s, len - len_a + len_b);

    return 0;
}

int8_t sstr_pad_center(sstr_t *s, char ch, size_t n)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (len == n)
    {
        return 0;
    }

    if (cap < len + n)
    {
        sstr_reserve(s, n - len + n - cap + 1);
    }

    size_t diff = n - len;
    size_t right = diff / 2;
    size_t left = diff - right;

    sstr_pad_left(s, ch, left);
    sstr_pad_right(s, ch, right);
    
    return 0;
}

int8_t sstr_pad_left(sstr_t* s, char ch, size_t n)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (cap < len + n + 1)
    {
        if (sstr_reserve(s, len + n - cap + 1))
        {
            return 1;
        }
    }

    shift((*s) + n, *s, len);

    char* p = *s;

    for (size_t i = 0; i < n; i++)
    {
        p[i] = ch;
    }

    sstr_set_len(*s, len + n);

    return 0;
}

int8_t sstr_pad_right(sstr_t* s, char ch, size_t n)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (cap < len + n + 1)
    {
        if(sstr_reserve(s, len + n - cap + 1))
        {
            return 1;
        }
    }

    char* p = *s + len;

    for (size_t i = 0; i < n; i++)
    {
        p[i] = ch;
    }

    p[n] = '\0';
    sstr_set_len(*s, len + n);

    return 0;
}

void sstr_trim_left(sstr_t s)
{
    size_t len = sstr_len(s);

    if (!len)
    {
        return;
    }

    size_t i = 0;
    for (; i < len; i++)
    {
        if (!isspace(s[i]))
        {
            break;
        }
    }

    sstr_remove(s, 0, i);
}

void sstr_trim_right(sstr_t s)
{
    size_t len = sstr_len(s);

    if (!len)
    {
        return;
    }

    ssize_t i = len - 1;

    for (; i > 0; i--)
    {
        if (!isspace(s[i]))
        {
            i++;
            break;
        }
    }

    size_t n = len - i;
    sstr_remove(s, i, n);
}

int8_t sstr_concat(sstr_t* s, const char* a, size_t len_a)
{
    size_t len = sstr_len(*s);
    size_t cap = sstr_cap(*s);

    if (cap < len + len_a)
    {
        if (sstr_reserve(s, cap - len - len_a + 1))
        {
            return 0;
        }
    }

    char* p = (*s) + len;

    for (size_t i = 0; i < len_a; i++)
    {
        p[i] = a[i];
    }

    p[len_a + 1]= '\0';
    sstr_set_len(*s, len + len_a);

    return 0;
}

int8_t sstr_prepend(sstr_t* s, const char* a, size_t len_a)
{
    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (cap < len + len_a + 1)
    {
        if (sstr_reserve(s, len + len_a - cap + 1))
        {
            return 1;
        }
    }

    shift((*s) + len_a, *s, len);

    char* p = *s;

    for (size_t i = 0; i < len_a; i++)
    {
        p[i] = a[i];
    }

    sstr_set_len(*s, len + len_a);

    return 0;
}

void sstr_remove(sstr_t s, size_t offset, size_t n)
{
    size_t len = sstr_len(s);

    if (n > len)
    {
        zero(s, len);
        return;
    }

    shift(s + offset, s + offset + n, len - offset);
    zero(s + len + n, n);
    sstr_set_len(s, len - n);
}

int8_t sstr_push(sstr_t* s, char ch) {
    if (ch == '\0')
    {
        return 0;
    }

    size_t cap = sstr_cap(*s);
    size_t len = sstr_len(*s);

    if (len == cap)
    {
        if (sstr_reserve(s, 1) < 0)
        {
            return -1;
        }
    }

    (*s)[len++] = ch;
    (*s)[len] = '\0';
    sstr_set_len(*s, len);

    return 1;
}

char sstr_pop(sstr_t s) 
{
    size_t len = sstr_len(s);

    if (!len)
    {
        return '\0';
    }

    char ch = s[--len];
    s[len] = '\0';
    sstr_set_len(s, len);

    return ch;
}

size_t pow2(size_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}

void shift(void * dest, void * src, size_t len)
{
    const char* s = src;
    char* d = dest;

    if (d < s)
    {
        while (len--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        s = s + (len - 1);
        d = d + (len - 1);

        while (len--)
        {
            *d-- = *s--;
        }
    }
}

void zero(void* p, size_t n)
{
    char* block = (char*)p;
    for (size_t i = 0; i < n; i++)
    {
        block[i] = 0;
    }
}

#endif // SSTRING_IMPL

