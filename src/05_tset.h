#include "01_core.h"
#define __cops_tset_choose(...)                                                \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count,                    \
                         __cops_init_tset_3, __cops_init_tset_2,               \
                         __cops_err_arg_count, )
#define init_tset(...)                                                         \
        __cops_expand(__cops_tset_choose(__VA_ARGS__)(__VA_ARGS__))

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_tset_2(T, NAME)                                            \
        __cops_init_tset_2_decl(T, NAME) __cops_init_tset_2_impl(T, NAME)
#define __cops_init_tset_3(T, NAME, SLICE_T)                                   \
        __cops_init_tset_3_decl(T, NAME, SLICE_T)                              \
            __cops_init_tset_3_impl(T, NAME, SLICE_T)
#else
#define __cops_init_tset_2(T, NAME) __cops_init_tset_2_decl(T, NAME)
#define __cops_init_tset_3(T, NAME, SLICE_T)                                   \
        __cops_init_tset_3_decl(T, NAME, SLICE_T)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_tset_2_impl(NAME, T)                                       \
        typedef struct NAME##_rb_node NAME##_rb_node;                          \
        struct NAME##_rb_node {                                                \
                struct NAME##_rb_node *parent, *left, *right;                  \
                uint8_t isred;                                                 \
                T val;                                                         \
        };                                                                     \
        typedef struct NAME {                                                  \
                uint32_t len;                                                  \
                NAME##_rb_node *root;                                          \
                int (*cmp)(T, T);                                              \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } NAME;                                                                \
        NAME *NAME##_new(int (*cmp)(T, T));                                    \
        void NAME##_free(NAME *self);                                          \
        int NAME##_add(NAME *self, T val);                                     \
        int NAME##_has(NAME *self, T val);                                     \
        int NAME##_del(NAME *self, T *val);

#define __cops_init_tset_2_decl(NAME, T)                                       \
                                                                               \
        NAME *NAME##_new(int (*cmp)(T, T))                                     \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self));                        \
                COPS_ASSERT(self && "failed allocation");                      \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cmp = cmp;                                               \
                self->len = 0;                                                 \
                self->root = NULL;                                             \
                self->free = NULL;                                             \
                self->dup = NULL;                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (self->len) {                                               \
                        NAME##_rb_node *n = self->root;                        \
                        while (n) {                                            \
                                if (n->left) {                                 \
                                        n = n->left;                           \
                                } else if (n->right) {                         \
                                        n = n->right;                          \
                                } else {                                       \
                                        NAME##_rb_node *p = n->parent;         \
                                        if (p && p->left == n)                 \
                                                p->left = NULL;                \
                                        if (p && p->right == n)                \
                                                p->right = NULL;               \
                                        if (self->free)                        \
                                                self->free(p->val);            \
                                        COPS_FREE(n);                          \
                                        n = p;                                 \
                                }                                              \
                        }                                                      \
                }                                                              \
                self->root = NULL;                                             \
                COPS_FREE(self);                                               \
                return;                                                        \
        }                                                                      \
                                                                               \
        /* rotate node to became right child of his left child*/               \
        static inline int NAME##_node_rotate_right(NAME *self,                 \
                                                   NAME##_rb_node *p)          \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(p && "invalid reference");                         \
                COPS_ASSERT(p->left && "empty reference");                     \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *x = p->left, *g = p->parent;                   \
                if (!x)                                                        \
                        return COPS_INVALID; /* link parent and child*/        \
                x->parent = g;                                                 \
                if (!g)                                                        \
                        self->root = x;                                        \
                else if (g->left == p)                                         \
                        g->left = x;                                           \
                else if (g->right == p)                                        \
                        g->right = x;                                          \
                /* link child.right and node*/                                 \
                p->left = x->right;                                            \
                if (x->right)                                                  \
                        x->right->parent = p;                                  \
                /* link parent and child*/                                     \
                p->parent = x;                                                 \
                x->right = p;                                                  \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        /* rotate node to became left child of his right child*/               \
        static inline int NAME##_node_rotate_left(NAME *self,                  \
                                                  NAME##_rb_node *p)           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(p && "invalid reference");                         \
                COPS_ASSERT(p->right && "empty reference");                    \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *x = p->right, *g = p->parent;                  \
                if (!x)                                                        \
                        return COPS_INVALID;                                   \
                /* link parent and child*/                                     \
                x->parent = g;                                                 \
                if (!g)                                                        \
                        self->root = x;                                        \
                else if (g->left == p)                                         \
                        g->left = x;                                           \
                else if (g->right == p)                                        \
                        g->right = x;                                          \
                /* link child.left and node*/                                  \
                p->right = x->left;                                            \
                if (x->left)                                                   \
                        x->left->parent = p;                                   \
                /* link parent and child*/                                     \
                p->parent = x;                                                 \
                x->left = p;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_add(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *e = COPS_ALLOC(sizeof(*e));                    \
                COPS_ASSERT(e && "failed allocation");                         \
                if (!e)                                                        \
                        return COPS_MEMERR;                                    \
                *e = (NAME##_rb_node){NULL, NULL, NULL, 1, val};               \
                /* insert*/                                                    \
                if (!self->root) {                                             \
                        self->root = e;                                        \
                        e->isred = 0;                                          \
                        self->len++;                                           \
                        return COPS_OK;                                        \
                }                                                              \
                NAME##_rb_node *n = self->root;                                \
                while (1) {                                                    \
                        int diff = self->cmp(key, n->key);                     \
                        if (!diff) {                                           \
                                COPS_FREE(e);                                  \
                                return COPS_INVALID;                           \
                        } else if (diff > 0) {                                 \
                                if (!n->right) {                               \
                                        e->parent = n;                         \
                                        n->right = e;                          \
                                        break;                                 \
                                }                                              \
                                n = n->right;                                  \
                        } else {                                               \
                                if (!n->left) {                                \
                                        e->parent = n;                         \
                                        n->left = e;                           \
                                        break;                                 \
                                }                                              \
                                n = n->left;                                   \
                        }                                                      \
                }                                                              \
                self->len++;                                                   \
                n = NULL;                                                      \
                /* balance*/                                                   \
                NAME##_rb_node *p, *u, *g, *x = e;                             \
                while (1) {                                                    \
                        /* init variables*/                                    \
                        p = x->parent;                                         \
                        g = u = NULL;                                          \
                        if (p)                                                 \
                                g = p->parent;                                 \
                        if (g)                                                 \
                                u = g->left == p ? g->right : g->left;         \
                        /* check cases*/                                       \
                        if (!p || !p->isred || !g)                             \
                                break;                                         \
                        /* from here we assume g is black and p is red*/       \
                        if (u && u->isred) {                                   \
                                /* repaint chunk*/                             \
                                p->isred = u->isred = 0;                       \
                                g->isred = 1;                                  \
                                /* reposition NAMEs*/                          \
                                x = g;                                         \
                                continue;                                      \
                        }                                                      \
                        /* from here we assume u is black or null*/            \
                        if (g->left == p) {                                    \
                                /* LR rot.*/                                   \
                                if (p->right == x) {                           \
                                        NAME##_node_rotate_left(self, p);      \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* LL rot.*/                                   \
                                NAME##_node_rotate_right(self, g);             \
                                p->isred = 0;                                  \
                                g->isred = 1;                                  \
                                break;                                         \
                        } else {                                               \
                                /* RL rot.*/                                   \
                                if (p->left == x) {                            \
                                        NAME##_node_rotate_right(self, p);     \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* RR rot.*/                                   \
                                NAME##_node_rotate_left(self, g);              \
                                p->isred = 0;                                  \
                                g->isred = 1;                                  \
                                break;                                         \
                        }                                                      \
                }                                                              \
                /* repaint root*/                                              \
                self->root->isred = 0;                                         \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_has(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *n = self->root;                                \
                while (n) {                                                    \
                        int cmp = self->cmp(val, n->val);                      \
                        if (!cmp)                                              \
                                return COPS_OK;                                \
                        else if (cmp > 0)                                      \
                                n = n->right;                                  \
                        else                                                   \
                                n = n->left;                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        int NAME##_del(NAME *self, T *val)                                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *x = self->root;                                \
                while (x) {                                                    \
                        int cmp = self->cmp(*val, x->val);                     \
                        if (!cmp)                                              \
                                break;                                         \
                        else if (cmp > 0)                                      \
                                x = x->right;                                  \
                        else                                                   \
                                x = x->left;                                   \
                }                                                              \
                if (!x)                                                        \
                        return COPS_INVALID;                                   \
                if (val)                                                       \
                        *val = x->val;     /* target found*/                   \
                NAME##_rb_node *n = x;     /* *n -> double black or target*/   \
                if (x->left && x->right) { /* leftmost right children*/        \
                        n = n->right;                                          \
                        while (n->left) {                                      \
                                n = n->left;                                   \
                        }                                                      \
                        T v = x->val;                                          \
                        x->val = n->val;                                       \
                        n->val = v;                                            \
                }                                                              \
                x = NULL;                      /* only one children (n)*/      \
                NAME##_rb_node *p = n->parent; /* *p -> n parent*/             \
                if (n->left || n->right) {                                     \
                        NAME##_rb_node *oc =                                   \
                            n->left ? n->left                                  \
                                    : n->right; /* *oc -> only child*/         \
                        if (!oc->isred) {                                      \
                                return COPS_ABORT;                             \
                        } else if (!p) {                                       \
                                self->root = oc;                               \
                        } else if (p->left == n) {                             \
                                p->left = oc;                                  \
                        } else if (p->right == n) {                            \
                                p->right = oc;                                 \
                        } /* parent (n) children is only child*/               \
                        oc->parent = p;                                        \
                        oc->isred = 0;                                         \
                        COPS_FREE(n);                                          \
                        self->len--;                                           \
                        return COPS_OK;                                        \
                } /* no children*/                                             \
                if (!p) {                                                      \
                        self->len--;                                           \
                        self->root = NULL;                                     \
                        COPS_FREE(n);                                          \
                        return COPS_OK;                                        \
                }                                                              \
                NAME##_rb_node *s = p->left == n ? p->right : p->left;         \
                NAME##_rb_node *cn, *dn;                                       \
                if (p->left == n)                                              \
                        p->left = NULL;                                        \
                else                                                           \
                        p->right = NULL;                                       \
                volatile int n_is_red = n->isred;                              \
                COPS_FREE(n);                                                  \
                n = NULL;                                                      \
                self->len--;                                                   \
                if (n_is_red)                                                  \
                        return COPS_OK; /* black node*/                        \
                while (1) {                                                    \
                        if (n)                                                 \
                                p = n->parent;                                 \
                        if (!p)                                                \
                                break; /* no root*/                            \
                        int n_is_right = p->right == n ? 1 : 0;                \
                        if (n_is_right) {                                      \
                                s = p->left;                                   \
                                cn = s->right;                                 \
                                dn = s->left;                                  \
                        } else {                                               \
                                s = p->right;                                  \
                                cn = s->left;                                  \
                                dn = s->right;                                 \
                        } /* resetted variable*/                               \
                        if (s && s->isred) {                                   \
                                if (n_is_right)                                \
                                        NAME##_node_rotate_right(self, p);     \
                                else                                           \
                                        NAME##_node_rotate_left(self, p);      \
                                s->isred = 0;                                  \
                                p->isred = 1;                                  \
                                continue;                                      \
                        } /* sibling is black*/                                \
                        if (cn && cn->isred) {                                 \
                                if (n_is_right)                                \
                                        NAME##_node_rotate_left(self, s);      \
                                else                                           \
                                        NAME##_node_rotate_right(self, s);     \
                                cn->isred = 0;                                 \
                                s->isred = 1;                                  \
                                continue;                                      \
                        } /* close nephew is black*/                           \
                        if (dn && dn->isred) {                                 \
                                if (n_is_right)                                \
                                        NAME##_node_rotate_right(self, p);     \
                                else                                           \
                                        NAME##_node_rotate_left(self, p);      \
                                s->isred = p->isred;                           \
                                p->isred = 0;                                  \
                                dn->isred = 0;                                 \
                                break;                                         \
                        } /* distant nephew is black*/                         \
                        if (p && p->isred) {                                   \
                                p->isred = 0;                                  \
                                s->isred = 1;                                  \
                                break;                                         \
                        } /* parent is black*/                                 \
                        s->isred = 1;                                          \
                        n = p;                                                 \
                }                                                              \
                return COPS_OK;                                                \
        }
