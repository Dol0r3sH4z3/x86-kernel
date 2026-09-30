#ifndef TYPES_H
#define TYPES_H

typedef struct list_head_t
{
    struct list_head_t *next, *prev;
} list_head_t;

#endif