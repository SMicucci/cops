#include <cops/core/allocator.h>
#include <cops/core/interface.h>
#include <cops/core/number.h>

static void _init(void *self);
static void _clean(cops_interface_t *self);
static int _compare(const cops_interface_t *self, const cops_interface_t *oth);
static unsigned long _hash(const cops_interface_t *self);

/* static interfaces declaration */

static const cops_interface_t __number = {
    sizeof(__number), _init, _clean, _compare, _hash, NULL,
};

static const cops_number_t __b = {
    __number,
    {.b = 0},
    COPS_BYTE,
};
const cops_interface_t *const CopsByte = (cops_interface_t *)&__b;

static const cops_number_t __s = {
    __number,
    {.s = 0},
    COPS_SHORT,
};
const cops_interface_t *const CopsShort = (cops_interface_t *)&__s;

static const cops_number_t __e = {
    __number,
    {.i = 0},
    COPS_ENUM,
};
const cops_interface_t *const CopsInt = (cops_interface_t *)&__e;
const cops_interface_t *const CopsEnum = (cops_interface_t *)&__e;

static const cops_number_t __l = {
    __number,
    {.l = 0},
    COPS_LONG,
};
const cops_interface_t *const CopsLong = (cops_interface_t *)&__l;

static const cops_number_t __f = {
    __number,
    {.f = 0.0},
    COPS_FLOAT,
};
const cops_interface_t *const CopsFloat = (cops_interface_t *)&__f;

static const cops_number_t __d = {
    __number,
    {.d = 0.0},
    COPS_DOUBLE,
};
const cops_interface_t *const CopsDouble = (cops_interface_t *)&__d;

/* external definition */

void number_init(cops_number_t *self, cops_e_number_t type, void *value)
{
        self->super = __number;
        self->type = type;
        switch (type) {
                case COPS_BYTE:
                        self->value.b = *((char *)value);
                        break;
                case COPS_SHORT:
                        self->value.s = *((short *)value);
                        break;
                case COPS_INT:
                        self->value.i = *((int *)value);
                        break;
                case COPS_LONG:
                        self->value.l = *((long *)value);
                        break;
                case COPS_FLOAT:
                        self->value.f = *((float *)value);
                        break;
                case COPS_DOUBLE:
                        self->value.d = *((double *)value);
                        break;
                default:
                        break;
        }
}

void number_clean(cops_number_t *self) { return; }

int number_compare(const cops_number_t *self, const cops_number_t *other)
{
        long s_i__ = 0, o_i__ = 0;
        double s_r__ = 0.0, o_r__ = 0.0;
        switch (self->type) {
                case COPS_BYTE:
                        s_i__ = (long)self->value.b;
                        break;
                case COPS_SHORT:
                        s_i__ = (long)self->value.s;
                        break;
                case COPS_INT:
                        s_i__ = (long)self->value.i;
                        break;
                case COPS_LONG:
                        s_i__ = (long)self->value.l;
                        break;
                case COPS_FLOAT:
                        s_r__ = (double)self->value.f;
                        break;
                case COPS_DOUBLE:
                        s_r__ = (double)self->value.d;
                        break;
        }
        switch (other->type) {
                case COPS_BYTE:
                        o_i__ = (long)other->value.b;
                        break;
                case COPS_SHORT:
                        o_i__ = (long)other->value.s;
                        break;
                case COPS_INT:
                        o_i__ = (long)other->value.i;
                        break;
                case COPS_LONG:
                        o_i__ = (long)other->value.l;
                        break;
                case COPS_FLOAT:
                        o_r__ = (double)other->value.f;
                        break;
                case COPS_DOUBLE:
                        o_r__ = (double)other->value.d;
                        break;
        }
        if (!s_i__ || !o_i__) {
                return s_i__ > o_i__ ? 1 : s_i__ < o_i__ ? -1 : 0;
                (void)s_r__;
                (void)o_r__;
        } else {
                return s_r__ > o_r__ ? 1 : s_r__ < o_r__ ? -1 : 0;
                (void)s_i__;
                (void)o_i__;
        }
}

unsigned long number_hash(const cops_number_t *self) { return self->value.l; }

/* internal definition */

static void _init(void *self)
{
        number_init(self, COPS_LONG, &(long){0});
        return;
}

static void _clean(cops_interface_t *self)
{
        number_clean((cops_number_t *)self);
}

static int _compare(const cops_interface_t *self, const cops_interface_t *oth)
{
        return number_compare((cops_number_t *)self, (cops_number_t *)oth);
}

static unsigned long _hash(const cops_interface_t *self)
{
        return number_hash((cops_number_t *)self);
}
