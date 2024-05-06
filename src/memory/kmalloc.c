#include "kmalloc.h"
#include <drivers/utils.h>
#include <kernel/panic.h>

#define KMALLOC_STATIC_SIZE 1024 * 1024

static u8 s_kmalloc_static[KMALLOC_STATIC_SIZE];

struct kmalloc_node {
    u32 data_size;
    bool free;
    _Alignas(16) u8 data[0];
};

struct kmalloc_node* next_node(struct kmalloc_node* node) {
    if (node->data + node->data_size >= s_kmalloc_static + sizeof(s_kmalloc_static)) {
        return NULL;
    }
    return (struct kmalloc_node*)(node->data + node->data_size);
}

void kmalloc_init() {
    struct kmalloc_node* node = (struct kmalloc_node*)s_kmalloc_static;
    node->free = 1;
    node->data_size = (s_kmalloc_static + sizeof(s_kmalloc_static)) - node->data;	
}

void* kmalloc(size_t size) {
    // Validate allocation size
    if (size == 0 || size >= sizeof(s_kmalloc_static)) {
        return NULL;
    }

    // Align size to 16 bytes
    if (size % 16 != 0) {
        size += 16 - (size % 16);
    }

    // Find a free node
    struct kmalloc_node* node = (struct kmalloc_node*)s_kmalloc_static;
    while (node->data_size < size || !node->free) {
        struct kmalloc_node* next = next_node(node);
        if (next == NULL) {
            return NULL;
        }
        if (node->free && next->free) {
            node->data_size += next->data_size + sizeof(struct kmalloc_node);
        } else {
            node = next;
        }
    }

    // Split node if it's too big
    if (node->data_size > size + sizeof(struct kmalloc_node)) {
        struct kmalloc_node* new_node = (struct kmalloc_node*)(node->data + size);
        new_node->data_size = node->data_size - size - sizeof(struct kmalloc_node);
        new_node->free = 1;
        node->data_size = size;
    }

    // Mark node as used
    node->free = false;

    return node->data;
}

void kfree(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    // Validate that the pointer is within the static memory
    if ((u8*)ptr < s_kmalloc_static || (u8*)ptr >= s_kmalloc_static + sizeof(s_kmalloc_static)) {
        panic("kfree called with pointer outside of kmalloc memory");
    }

    struct kmalloc_node* node = (struct kmalloc_node*)((u8*)ptr - sizeof(struct kmalloc_node));

    // Confirm that the node is used
    if (node->free) {
        panic("kfree called with pointer that is already free");
    }

    // Mark node as free
    node->free = true;
}
