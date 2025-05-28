#ifndef COPS_CORE_NUMBER_H
#define COPS_CORE_NUMBER_H

#include <cops/core/interface.h>

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

struct number {
        cops_interface_t super;
        cops_u_number_t value;
        cops_e_number_t type;
};

void number_init(cops_number_t *self, cops_e_number_t type, void *value);

#endif /* end of include guard: COPS_CORE_NUMBER_H */
