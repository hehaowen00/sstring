# sstring

`sstring` is a heap allocated string library for the C programming
language;

## usage
```c
#include <stdio.h>
#include "sstring.h"

int main(int argc, char* argv[])
{
    sstr_t message = sstr_from("Hello, World!", 13);
    printf("%s\n", message);

    return 0;
}
```

## interface
```c
sstr_t sstr_new(size_t cap);
sstr_t sstr_from(const char* str, size_t len);

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

int8_t sstr_push(sstr_t* s, char ch);
char sstr_pop(sstr_t s);
```
