#ifndef COPS_OMAP_H
#define COPS_OMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

/**
 * when int is returned:
 *  0: no error
 * -1: invalid arguments
 * -2: memory unavaible
 * -3: logic internal error
 * */

#define __init_cops_omap(name, K, V)                                                               \
                                                                                                   \
        typedef struct name##_node {                                                               \
                struct name##_node *parent, *left, *right;                                         \
                uint8_t isred;                                                                     \
                K key;                                                                             \
                V val;                                                                             \
        } name##_node;                                                                             \
                                                                                                   \
        typedef struct name {                                                                      \
                uint32_t nelem;                                                                    \
                uint32_t rc;                                                                       \
                name##_node *root;                                                                 \
                int (*cmp)(K, K);                                                                  \
        } name;                                                                                    \
                                                                                                   \
        static inline name *name##_new(int (*cmp)(K, K))                                           \
        {                                                                                          \
                name *self = cops_default_allocator.alloc(sizeof(*self));                           \
                if (!self)                                                                         \
                        return NULL;                                                               \
                self->cmp = cmp;                                                                   \
                self->nelem = 0;                                                                   \
                self->rc = 1;                                                                      \
                self->root = NULL;                                                                 \
                return self;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline name *name##_dup(name *self)                                                 \
        {                                                                                          \
                if (self)                                                                          \
                        self->rc++;                                                                \
                return self;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline name *name##_free(name *self)                                                \
        {                                                                                          \
                if (self && self->rc > 0 && !(--self->rc)) {                                       \
                        if (self->nelem) {                                                         \
                                name##_node *n = self->root;                                       \
                                while (n) {                                                        \
                                        if (n->left) {                                             \
                                                n = n->left;                                       \
                                        } else if (n->right) {                                     \
                                                n = n->right;                                      \
                                        } else {                                                   \
                                                name##_node *p = n->parent;                        \
                                                if (p && p->left == n)                             \
                                                        p->left = NULL;                            \
                                                if (p && p->right == n)                            \
                                                        p->right = NULL;                           \
                                                cops_default_allocator.free(n);                     \
                                                n = p;                                             \
                                        }                                                          \
                                }                                                                  \
                        }                                                                          \
                        self->root = NULL;                                                         \
                        cops_default_allocator.free(self);                                          \
                }                                                                                  \
                return NULL;                                                                       \
        }                                                                                          \
                                                                                                   \
        /* rotate node to became right child of his left child*/                                   \
        static inline int name##_node_rotate_right(name *self, name##_node *p)                     \
        {                                                                                          \
                if (!self || !p)                                                                   \
                        return -1;                                                                 \
                name##_node *x = p->left, *g = p->parent;                                          \
                if (!x)                                                                            \
                        return -1; /* link parent and child*/                                      \
                x->parent = g;                                                                     \
                if (!g)                                                                            \
                        self->root = x;                                                            \
                else if (g->left == p)                                                             \
                        g->left = x;                                                               \
                else if (g->right == p)                                                            \
                        g->right = x;                                                              \
                /* link child.right and node*/                                                     \
                p->left = x->right;                                                                \
                if (x->right)                                                                      \
                        x->right->parent = p;                                                      \
                /* link parent and child*/                                                         \
                p->parent = x;                                                                     \
                x->right = p;                                                                      \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        /* rotate node to became left child of his right child*/                                   \
        static inline int name##_node_rotate_left(name *self, name##_node *p)                      \
        {                                                                                          \
                if (!self || !p)                                                                   \
                        return -1;                                                                 \
                name##_node *x = p->right, *g = p->parent;                                         \
                if (!x)                                                                            \
                        return -1;                                                                 \
                /* link parent and child*/                                                         \
                x->parent = g;                                                                     \
                if (!g)                                                                            \
                        self->root = x;                                                            \
                else if (g->left == p)                                                             \
                        g->left = x;                                                               \
                else if (g->right == p)                                                            \
                        g->right = x;                                                              \
                /* link child.left and node*/                                                      \
                p->right = x->left;                                                                \
                if (x->left)                                                                       \
                        x->left->parent = p;                                                       \
                /* link parent and child*/                                                         \
                p->parent = x;                                                                     \
                x->left = p;                                                                       \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_add(name *self, K key, V val)                                     \
        {                                                                                          \
                if (!self)                                                                         \
                        return -1;                                                                 \
                if (!self->cmp)                                                                    \
                        return -3; /* init node*/                                                  \
                name##_node *e = (name##_node *)cops_default_allocator.alloc(sizeof(*e));           \
                if (!e)                                                                            \
                        return -2;                                                                 \
                *e = (name##_node){NULL, NULL, NULL, 1, key, val};                                 \
                /* insert*/                                                                        \
                if (!self->root) {                                                                 \
                        self->root = e;                                                            \
                        e->isred = 0;                                                              \
                        self->nelem++;                                                             \
                        return 0;                                                                  \
                }                                                                                  \
                name##_node *n = self->root;                                                       \
                while (1) {                                                                        \
                        int diff = self->cmp(key, n->key);                                         \
                        if (!diff) {                                                               \
                                cops_default_allocator.free(e);                                     \
                                return -3;                                                         \
                        } else if (diff > 0) {                                                     \
                                if (!n->right) {                                                   \
                                        e->parent = n;                                             \
                                        n->right = e;                                              \
                                        break;                                                     \
                                }                                                                  \
                                n = n->right;                                                      \
                        } else {                                                                   \
                                if (!n->left) {                                                    \
                                        e->parent = n;                                             \
                                        n->left = e;                                               \
                                        break;                                                     \
                                }                                                                  \
                                n = n->left;                                                       \
                        }                                                                          \
                }                                                                                  \
                self->nelem++;                                                                     \
                n = NULL;                                                                          \
                /* balance*/                                                                       \
                name##_node *p, *u, *g, *x = e;                                                    \
                while (1) {                                                                        \
                        /* init variables*/                                                        \
                        p = x->parent;                                                             \
                        g = u = NULL;                                                              \
                        if (p)                                                                     \
                                g = p->parent;                                                     \
                        if (g)                                                                     \
                                u = g->left == p ? g->right : g->left;                             \
                        /* check cases*/                                                           \
                        if (!p || !p->isred || !g)                                                 \
                                break;                                                             \
                        /* from here we assume g is black and p is red*/                           \
                        if (u && u->isred) {                                                       \
                                /* repaint chunk*/                                                 \
                                p->isred = u->isred = 0;                                           \
                                g->isred = 1;                                                      \
                                /* reposition names*/                                              \
                                x = g;                                                             \
                                continue;                                                          \
                        }                                                                          \
                        /* from here we assume u is black or null*/                                \
                        if (g->left == p) {                                                        \
                                /* LR rot.*/                                                       \
                                if (p->right == x) {                                               \
                                        name##_node_rotate_left(self, p);                          \
                                        x = p;                                                     \
                                        p = x->parent;                                             \
                                }                                                                  \
                                /* LL rot.*/                                                       \
                                name##_node_rotate_right(self, g);                                 \
                                p->isred = 0;                                                      \
                                g->isred = 1;                                                      \
                                break;                                                             \
                        } else {                                                                   \
                                /* RL rot.*/                                                       \
                                if (p->left == x) {                                                \
                                        name##_node_rotate_right(self, p);                         \
                                        x = p;                                                     \
                                        p = x->parent;                                             \
                                }                                                                  \
                                /* RR rot.*/                                                       \
                                name##_node_rotate_left(self, g);                                  \
                                p->isred = 0;                                                      \
                                g->isred = 1;                                                      \
                                break;                                                             \
                        }                                                                          \
                }                                                                                  \
                /* repaint root*/                                                                  \
                self->root->isred = 0;                                                             \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_has(name *self, K key, V *val)                                    \
        {                                                                                          \
                if (!self)                                                                         \
                        return -1;                                                                 \
                if (!self->cmp)                                                                    \
                        return -3;                                                                 \
                name##_node *n = self->root;                                                       \
                while (n) {                                                                        \
                        int cmp = self->cmp(key, n->key);                                          \
                        if (!cmp) {                                                                \
                                if (val)                                                           \
                                        *val = n->val;                                             \
                                return 0;                                                          \
                        } else if (cmp > 0) {                                                      \
                                n = n->right;                                                      \
                        } else {                                                                   \
                                n = n->left;                                                       \
                        }                                                                          \
                }                                                                                  \
                return -3;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_mod(name *self, K key, V val)                                     \
        {                                                                                          \
                if (!self)                                                                         \
                        return -1;                                                                 \
                if (!self->cmp)                                                                    \
                        return -3;                                                                 \
                name##_node *n = self->root;                                                       \
                while (n) {                                                                        \
                        int cmp = self->cmp(key, n->key);                                          \
                        if (!cmp) {                                                                \
                                n->val = val;                                                      \
                                return 0;                                                          \
                        } else if (cmp > 0) {                                                      \
                                n = n->right;                                                      \
                        } else {                                                                   \
                                n = n->left;                                                       \
                        }                                                                          \
                }                                                                                  \
                return -3;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_del(name *self, K key, V *val)                                    \
        {                                                                                          \
                if (!self)                                                                         \
                        return -1;                                                                 \
                if (!self->cmp)                                                                    \
                        return -3; /* valid tree*/                                                 \
                name##_node *x = self->root;                                                       \
                while (x) {                                                                        \
                        int cmp = self->cmp(key, x->key);                                          \
                        if (!cmp)                                                                  \
                                break;                                                             \
                        else if (cmp > 0)                                                          \
                                x = x->right;                                                      \
                        else                                                                       \
                                x = x->left;                                                       \
                }                                                                                  \
                if (!x)                                                                            \
                        return -3;                                                                 \
                if (val)                                                                           \
                        *val = x->val;     /* target found*/                                       \
                name##_node *n = x;        /* *n -> double black or target*/                       \
                if (x->left && x->right) { /* leftmost right children*/                            \
                        n = n->right;                                                              \
                        while (n->left) {                                                          \
                                n = n->left;                                                       \
                        }                                                                          \
                        K k = x->key;                                                              \
                        V v = x->val;                                                              \
                        x->key = n->key;                                                           \
                        x->val = n->val;                                                           \
                        n->key = k;                                                                \
                        n->val = v;                                                                \
                }                                                                                  \
                x = NULL;                   /* only one children (n)*/                             \
                name##_node *p = n->parent; /* *p -> n parent*/                                    \
                if (n->left || n->right) {                                                         \
                        name##_node *oc = n->left ? n->left : n->right; /* *oc -> only child*/     \
                        if (!oc->isred) {                                                          \
                                return -3;                                                         \
                        } else if (!p) {                                                           \
                                self->root = oc;                                                   \
                        } else if (p->left == n) {                                                 \
                                p->left = oc;                                                      \
                        } else if (p->right == n) {                                                \
                                p->right = oc;                                                     \
                        } /* parent (n) children is only child*/                                   \
                        oc->parent = p;                                                            \
                        oc->isred = 0;                                                             \
                        cops_default_allocator.free(n);                                             \
                        self->nelem--;                                                             \
                        return 0;                                                                  \
                } /* no children*/                                                                 \
                if (!p) {                                                                          \
                        self->nelem--;                                                             \
                        self->root = NULL;                                                         \
                        cops_default_allocator.free(n);                                             \
                        return 0;                                                                  \
                }                                                                                  \
                name##_node *s = p->left == n ? p->right : p->left;                                \
                name##_node *cn, *dn;                                                              \
                if (p->left == n)                                                                  \
                        p->left = NULL;                                                            \
                else                                                                               \
                        p->right = NULL;                                                           \
                volatile int n_is_red = n->isred;                                                  \
                cops_default_allocator.free(n);                                                     \
                n = NULL;                                                                          \
                self->nelem--;                                                                     \
                if (n_is_red)                                                                      \
                        return 0; /* black node*/                                                  \
                while (1) {                                                                        \
                        if (n)                                                                     \
                                p = n->parent;                                                     \
                        if (!p)                                                                    \
                                break; /* no root*/                                                \
                        int n_is_right = p->right == n ? 1 : 0;                                    \
                        if (n_is_right) {                                                          \
                                s = p->left;                                                       \
                                cn = s->right;                                                     \
                                dn = s->left;                                                      \
                        } else {                                                                   \
                                s = p->right;                                                      \
                                cn = s->left;                                                      \
                                dn = s->right;                                                     \
                        } /* resetted variable*/                                                   \
                        if (s && s->isred) {                                                       \
                                if (n_is_right)                                                    \
                                        name##_node_rotate_right(self, p);                         \
                                else                                                               \
                                        name##_node_rotate_left(self, p);                          \
                                s->isred = 0;                                                      \
                                p->isred = 1;                                                      \
                                continue;                                                          \
                        } /* sibling is black*/                                                    \
                        if (cn && cn->isred) {                                                     \
                                if (n_is_right)                                                    \
                                        name##_node_rotate_left(self, s);                          \
                                else                                                               \
                                        name##_node_rotate_right(self, s);                         \
                                cn->isred = 0;                                                     \
                                s->isred = 1;                                                      \
                                continue;                                                          \
                        } /* close nephew is black*/                                               \
                        if (dn && dn->isred) {                                                     \
                                if (n_is_right)                                                    \
                                        name##_node_rotate_right(self, p);                         \
                                else                                                               \
                                        name##_node_rotate_left(self, p);                          \
                                s->isred = p->isred;                                               \
                                p->isred = 0;                                                      \
                                dn->isred = 0;                                                     \
                                break;                                                             \
                        } /* distant nephew is black*/                                             \
                        if (p && p->isred) {                                                       \
                                p->isred = 0;                                                      \
                                s->isred = 1;                                                      \
                                break;                                                             \
                        } /* parent is black*/                                                     \
                        s->isred = 1;                                                              \
                        n = p;                                                                     \
                }                                                                                  \
                return 0;                                                                          \
        }

#define init_cops_omap(K, V) __init_cops_omap(cops_##K##_##V##_omap, K, V)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_OMAP_H */
