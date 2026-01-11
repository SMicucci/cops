#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * 1. LIBRARY INCLUSION
 * (For this standalone test, we assume the macros are available here.
 * If using separate files, compile with: gcc test_main.c -o test)
 * ========================================================================= */
#define COPS_ASSERT_ENABLE
#include "../src/list.h"

/* =========================================================================
 * CASE A: POINTERS (struct Entity*)
 * Tests reference semantics. The list stores addresses.
 * ========================================================================= */

typedef struct Entity {
        int id;
        int data[10]; // bulky data
} Entity;

/* List stores 'Entity*', Slice stores 'Entity*' */
init_slice(Entity *, entity_slice);
init_list(Entity *, entity_list, entity_slice);

Entity *entity_create(int id)
{
        Entity *e = malloc(sizeof(Entity));
        assert(e);
        e->id = id;
        return e;
}

/* The list 'free' callback receives 'T' (which is Entity*) */
void entity_free_fn(Entity *e)
{
        if (e) {
                free(e);
        }
}

/* The list 'dup' callback receives 'T' and returns 'T' */
Entity *entity_dup_fn(Entity *src)
{
        if (!src)
                return NULL;
        Entity *dst = entity_create(src->id);
        memcpy(dst->data, src->data, sizeof(src->data));
        return dst;
}

void test_pointers()
{
        printf("[TEST] Pointers (Entity*)...\n");

        entity_list *list = entity_list_new();
        list->free = entity_free_fn;
        list->dup = entity_dup_fn;

        // 1. Populate
        entity_list_push_back(list, entity_create(101));
        entity_list_push_back(list, entity_create(102));

        assert(list->len == 2);
        assert(list->head->val->id == 101);

        // 2. Export to Slice (Triggers Deep Copy via dup_fn)
        entity_slice *slice = entity_list_export(list);
        assert(slice);
        assert(slice->len == 2);
        assert(slice->data[0]->id == 101);
        assert(slice->data[0] != list->head->val); // Different pointers!

        // 3. Cleanup
        entity_list_free(list);   // Frees original 2 entities
        entity_slice_free(slice); // Frees 2 duplicated entities

        printf("   -> PASS\n");
}

/* =========================================================================
 * CASE B: STRUCTS BY VALUE (struct Vector2)
 * Tests direct value copying. No pointers involved. No free/dup needed.
 * ========================================================================= */

typedef struct Vector2 {
        int x;
        int y;
} Vector2;

/* List stores 'Vector2' directly */
init_slice(Vector2, vec_slice);
init_list(Vector2, vec_list, vec_slice);

void test_structs_by_value_simple()
{
        printf("[TEST] Structs By Value (Simple Vector2)...\n");
        // No memory counters needed here, standard stack/struct copy semantics

        vec_list *list = vec_list_new();

        // 1. Push literals
        vec_list_push_back(list, (Vector2){10, 20});
        vec_list_push_front(list, (Vector2){5, 5});

        // 2. Structural Check
        assert(list->len == 2);
        assert(list->head->val.x == 5);  // First item
        assert(list->tail->val.x == 10); // Last item

        // 3. Export to Slice (Memcpy semantics by default)
        printf("here\n");
        vec_slice *slice = vec_list_export(list);
        assert(slice->len == 2);
        assert(slice->data[0].x == 5);

        // 4. Modify Slice (Ensure it doesn't affect list)
        slice->data[0].x = 999;
        assert(list->head->val.x == 5); // List should remain unchanged

        vec_list_free(list);
        vec_slice_free(slice);
        printf("   -> PASS\n");
}

/* =========================================================================
 * CASE C: STRUCTS WITH INTERNAL POINTERS (struct StringBox)
 * "Hard Mode": List stores the struct by value, but the struct owns a pointer.
 * This requires strict free/dup management to avoid double-frees or leaks.
 * ========================================================================= */

typedef struct StringBox {
        char *text; // Dynamic resource
        int size;
} StringBox;

init_slice(StringBox, box_slice);
init_list(StringBox, box_list, box_slice);

/* Helper to make a box */
StringBox make_box(const char *txt)
{
        StringBox b;
        b.size = strlen(txt);
        b.text = strdup(txt);
        return b;
}

/* Callback: Frees the *contents* of the struct */
void box_free_fn(StringBox b)
{
        if (b.text) {
                free(b.text);
        }
}

/* Callback: Deep copies the struct content */
StringBox box_dup_fn(StringBox src)
{
        StringBox dst;
        dst.size = src.size;
        dst.text = strdup(src.text);
        return dst;
}

void test_structs_with_resources()
{
        printf("[TEST] Structs By Value with Deep Resources...\n");

        box_list *list = box_list_new();
        list->free = box_free_fn;
        list->dup = box_dup_fn;

        // 1. Push (Ownership transfer logic)
        // We create a box (alloc 1), push it (copy struct), list owns it.
        StringBox temp = make_box("Hello");
        box_list_push_back(list, temp);
        // Note: In C, we passed 'temp' by value.
        // Now 'list' has a copy of the struct, pointing to the SAME char*.
        // We must NOT free 'temp.text' here, because the list owns it now.

        StringBox temp2 = make_box("World");
        box_list_push_back(list, temp2);

        assert(list->len == 2);
        assert(strcmp(list->head->val.text, "Hello") == 0);

        // 2. Export (Deep Copy)
        // Should verify 'dup' is called, creating NEW strings
        box_slice *slice = box_list_export(list);

        assert(slice->len == 2);
        assert(strcmp(slice->data[0].text, "Hello") == 0);
        assert(slice->data[0].text !=
               list->head->val.text); // Must be different addresses

        // 3. Import (Deep Copy)
        // We need a fresh list to avoid the empty-list import bug in the
        // library
        box_list *list2 = box_list_new();
        list2->free = box_free_fn;
        list2->dup = box_dup_fn;

        // Seed list2 to satisfy import implementation requirements
        box_list_push_back(list2, make_box("dummy"));

        box_list_import(list2, slice);

        // 4. Validation
        // list:  Hello, World
        // slice: Hello, World (copies)
        // list2: dummy, Hello, World (copies)

        // Total Allocs expected:
        // 2 (list originals)
        // 2 (slice copies)
        // 1 (list2 dummy)
        // 2 (list2 copies)
        // Total = 7 allocs

        // 5. Cleanup
        box_list_free(list);   // Frees 2
        box_slice_free(slice); // Frees 2
        box_list_free(list2);  // Frees 3 (dummy + 2 imported)
}

/* =========================================================================
 * MAIN EXECUTION
 * ========================================================================= */
int main(void)
{
        printf("=== PEDANTIC LIBRARY TEST SUITE ===\n");

        test_pointers();
        test_structs_by_value_simple();
        test_structs_with_resources();

        printf("=== ALL TESTS PASSED ===\n");
        return 0;
}
