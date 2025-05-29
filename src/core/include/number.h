#ifndef COPS_CORE_NUMBER_H
#define COPS_CORE_NUMBER_H

#include <cops/core/interface.h>

/* number interface for collection */

extern const cops_interface_t *const CopsByte;
extern const cops_interface_t *const CopsShort;
extern const cops_interface_t *const CopsInt;
extern const cops_interface_t *const CopsEnum;
extern const cops_interface_t *const CopsLong;
extern const cops_interface_t *const CopsFloat;
extern const cops_interface_t *const CopsDouble;

typedef enum cops_number_e cops_e_number_t;
typedef union cops_number_u cops_u_number_t;
typedef struct cops_number cops_number_t;

/* number X-macro */
#define COPS_NUMBER_LIST(X)                                                    \
        X(COPS_BYTE, char, b, 0)                                               \
        X(COPS_SHORT, short, s, 1)                                             \
        X(COPS_INT, int, i, 2)                                                 \
        X(COPS_LONG, long, l, 3)                                               \
        X(COPS_FLOAT, float, f, 4)                                             \
        X(COPS_DOUBLE, double, d, 5)

/* number enum */
enum cops_number_e {
#define X(l, t, n, v) l = v,
        COPS_NUMBER_LIST(X) COPS_ENUM = COPS_INT,
#undef X
};

/* number union */
union cops_number_u {
#define X(l, t, n, v) t n;
        COPS_NUMBER_LIST(X)
#undef X
};

#undef COPS_NUMBER_LIST

struct cops_number {
        cops_interface_t super;
        cops_u_number_t value;
        cops_e_number_t type;
};

void number_init(cops_number_t *self, cops_e_number_t type, void *value);

void number_clean(cops_number_t *self);

int number_compare(const cops_number_t *self, const cops_number_t *other);

unsigned long number_hash(const cops_number_t *self);

#endif /* end of include guard: COPS_CORE_NUMBER_H */
