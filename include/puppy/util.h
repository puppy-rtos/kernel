
/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PUPPY_INC_UTIL_H__
#define PUPPY_INC_UTIL_H__

int p_hw_borad_init(void);
int p_hw_cons_getc(void);
int p_hw_cons_output(const char *str, int len);
int printk(const char *fmt, ...);

#define P_ROM_SECTION "puppy_rom_sym."
#define P_RAM_SECTION "puppy_ram_sym."

#define P_SECTION_DATA(x) p_section(x ".1")
#define P_SECTION_START_DEFINE(x, name) p_used static p_base_t name p_section(x ".0")
#define P_SECTION_END_DEFINE(x, name)   p_used static p_base_t name p_section(x ".2")
#define P_SECTION_START_ADDR(name) (&name + 1)
#define P_SECTION_END_ADDR(name)   (&name)

#define P_TC_PASS()         printk("Test Passed! at %s:%d\r\n", __FUNCTION__, __LINE__);
#define P_TC_FAIL()         printk("Test Failed! at %s:%d\r\n", __FUNCTION__, __LINE__); 
#define P_TC_LOG(...)   do {printk(__VA_ARGS__); printk("\r\n");} while (0);

struct p_tc_fn
{
    const char *name;
    void       (*tc)(void);
};

#define P_TC_SECTION P_ROM_SECTION "P_TC_LIST"
#define P_TC_FUNC(fn, name)                         \
    p_used const static struct p_tc_fn _p_tc_##fn   \
    P_SECTION_DATA(P_TC_SECTION) = { #name, fn}

/**
 * p_container_of - return the start address of struct type, while ptr is the
 * member of struct type.
 */
#define p_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * @brief list
 * 
 */
struct _list_node {
    union {
        struct _list_node *head; /* ptr to head of list (p_list_t) */
        struct _list_node *next; /* ptr to next node    (p_node_t) */
    };
    union {
        struct _list_node *tail; /* ptr to tail of list (p_list_t) */
        struct _list_node *prev; /* ptr to previous node (p_node_t) */
    };
};
typedef struct _list_node p_list_t;
typedef struct _list_node p_node_t;



/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define p_list_entry(node, type, member) \
    p_container_of(node, type, member)

/**
 * @brief Provide the primitive to iterate on a list
 * Note: the loop is unsafe and thus node should not be removed
 * @param list A pointer on a p_list_t to iterate on.
 * @param node A p_node_t pointer to peek each node of the list
 */
#define p_list_for_each_node(list, node) \
    for (node = (list)->head; node != (list); node = node->next)

/**
 * @brief Provide the primitive to safely iterate on a list
 * Note: node can be removed, it will not break the loop.
 * @param list A pointer on a p_list_t to iterate on.
 * @param node A p_node_t pointer to peek each node of the list
 * @param node_s A p_node_t pointer for the loop to run safely
 */
#define p_list_for_each_node_safe(list, node, node_s) \
    for (node = (list)->head, node_s = node->next; node != (list); \
        node = node_s, node_s = node->next)

#define P_LIST_STATIC_INIT(list_ptr) { {(list_ptr)}, {(list_ptr)} }

static inline void p_list_init(p_list_t *list)
{
    list->head = list->tail = list;
}

static inline bool p_list_is_empty(p_list_t *list)
{
    return list->head == list;
}

static inline bool p_node_is_linked(p_node_t *node)
{
    return node->next != NULL;
}

/**
 * @brief add node to tail of list
 *
 * This and other p_list_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 * @param node the element to append
 */

static inline void p_list_append(p_list_t *list, p_node_t *node)
{
    p_node_t *const tail = list->tail;

    node->next = list;
    node->prev = tail;

    tail->next = node;
    list->tail = node;
}

/**
 * @brief add node to head of list
 *
 * This and other p_list_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 * @param node the element to append
 */

static inline void p_list_prepend(p_list_t *list, p_node_t *node)
{
    p_node_t *const head = list->head;

    node->next = head;
    node->prev = list;

    head->prev = node;
    list->head = node;
}

/**
 * @brief Insert a node into a list
 *
 * Insert a node before a specified node in a dlist.
 *
 * @param successor the position before which "node" will be inserted
 * @param node the element to insert
 */
static inline void p_list_insert(p_node_t *successor, p_node_t *node)
{
	p_node_t *const prev = successor->prev;

	node->prev = prev;
	node->next = successor;
	prev->next = node;
	successor->prev = node;
}

/**
 * @brief remove node from list.
 * @param node the node to remove from the list.
 */
static inline void p_list_remove(p_node_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;

    node->prev = node->next = NULL;
}

typedef struct {
  uint8_t *buffer;
  size_t size;
  atomic_size_t head;
  atomic_size_t tail;
} p_rb_t;

static inline bool p_rb_init(p_rb_t *rb, uint8_t *buffer, size_t size)
{
    if (rb == NULL || buffer == NULL || size == 0)
    {
        return false;
    }
    rb->buffer = buffer;
    rb->size = size;
    atomic_init(&rb->head, 0);
    atomic_init(&rb->tail, 0);
    return true;
}

static inline bool p_rb_write(p_rb_t *rb, const uint8_t *data, size_t length)
{
    if (rb == NULL || data == NULL || length == 0) {
        return false;
    }

    size_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    if (tail + rb->size - head < length) {
        return false;
    }

    for (size_t i = 0; i < length; ++i) {
        rb->buffer[head % rb->size] = data[i];
        ++head;
    }

    atomic_store_explicit(&rb->head, head, memory_order_release);

    return true;
}

static inline bool p_rb_read(p_rb_t *rb, uint8_t *data, size_t length)
{
    if (rb == NULL || data == NULL || length == 0) {
        return false;
    }

    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);

    if (head - tail < length) {
        return false;
    }

    for (size_t i = 0; i < length; ++i) {
        data[i] = rb->buffer[tail % rb->size];
        ++tail;
    }

    atomic_store_explicit(&rb->tail, tail, memory_order_release);

    return true;
}

#endif
