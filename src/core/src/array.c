#include <alloca.h>
#include <cops/core/allocator.h>
#include <cops/core/array.h>
#include <cops/core/container.h>
#include <cops/core/iterator.h>
#include <stddef.h>

/* internal declaration */

void _init(void *self);
void _clean(cops_interface_t *self);
cops_iterator_t *_iter(cops_container_t *self);
void _merge(cops_container_t *self, cops_container_t *val);

/* external reference */

static cops_container_t __array = {
    {
        sizeof(__array),
        _init,
        _clean,
        NULL,
        NULL,
        NULL,
    },
    _iter,
    NULL,
    _merge,
    NULL,
};

const cops_interface_t *const Array = (cops_interface_t *)&__array;

/* external definition */

void array_init(cops_array_t *self, cops_interface_t *type, size_t len)
{
        self->super = *(cops_container_t *)Array;
        self->super.value = type;
        self->data = cops_alloc(len * type->size);
        self->len = len;
}

void array_clean(cops_array_t *self)
{
        cops_interface_t *target;
        size_t chunk_size = self->super.value->size;
        char *data = self->data;
        for (size_t i = 0; i < self->len; i++) {
                target = (cops_interface_t *)(data + i * chunk_size);
                if (target->clean)
                        target->clean(target);
        }
        cops_dealloc(self->data);
}

cops_iterator_t *array_iterator(cops_array_t *self);

void array_merge(cops_array_t *self, cops_container_t *input);

void array_insert(cops_array_t *self, size_t pos, void *val);

void *array_remove(cops_array_t *self, size_t pos);

void array_set(cops_array_t *self, size_t pos, void *val);

void *array_peek(cops_array_t *self, size_t pos);

/* internal declaration */

void _init(void *self)
{
        array_init(self, NULL, 0);
        return;
}

void _clean(cops_interface_t *self)
{
        array_clean((cops_array_t *)self);
        return;
}

cops_iterator_t *_iter(cops_container_t *self)
{
        return array_iterator((cops_array_t *)self);
}

void _merge(cops_container_t *self, cops_container_t *val)
{
        array_merge((cops_array_t *)self, val);
        return;
}
