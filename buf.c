/* buf: a sized buffer type. */

#include <stdlib.h>
#include <string.h>
#include "buf.h"

buf_t * buf_size(buf_t *buf, size_t len)
{
    if (buf == NULL)
    {
        buf = malloc(sizeof(buf_t));
        buf->data = NULL;
        buf->len = 0;
    }

    buf->data = realloc(buf->data, len);

    if (buf->len > len)
        buf->len = len;
    buf->limit = len;

    return buf;
}

void buf_push(buf_t *buf, uint8_t c)
{
    buf->data[buf->len++] = c;
}

void buf_concat(buf_t *dst, uint8_t *src, size_t len)
{
    for (size_t i = 0; i < len; i++)
        dst->data[dst->len++] = src[i];
}

char * buf_tostr(buf_t *buf)
{
    char *str = malloc(buf->len + 1);

    memcpy(str, buf->data, buf->len);
    str[buf->len] = '\0';

    return str;
}
