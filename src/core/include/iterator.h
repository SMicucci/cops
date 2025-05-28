#ifndef COPS_CORE_ITERATOR_H
#define COPS_CORE_ITERATOR_H

typedef struct cops_iterator cops_iterator_t;

/* function pointer declaration */

typedef void (*cops_iterator_next_FnPtr)(cops_iterator_t *);
typedef void (*cops_iterator_prev_FnPtr)(cops_iterator_t *);
typedef void *(*cops_iterator_value_FnPtr)(cops_iterator_t *);

struct cops_iterator {
        cops_iterator_next_FnPtr iter_next;
        cops_iterator_prev_FnPtr iter_prev;
        cops_iterator_value_FnPtr iter_val;
};

/* generic function wrap */

void cops_iter_next(cops_iterator_t *self);
void cops_iter_prev(cops_iterator_t *self);
void *cops_iter_value(cops_iterator_t *self);

#endif /* end of include guard: COPS_CORE_ITERATOR_H */
