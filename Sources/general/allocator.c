/*
 * allocator.c
 *
 *  Created on: Oct 23, 2025
 *      Author: david
 */

#include <stdint.h>

#define MAX_WORDS           15000
#define MAX_ALLOCATIONS     30
#define INVALID_SLOT        0xFFFFFFFF

static uint32_t heap[MAX_WORDS];
static uint8_t reserved[MAX_WORDS / 8];

static uint32_t alloc_ptrs[MAX_ALLOCATIONS];
static uint32_t alloc_sizes[MAX_ALLOCATIONS];
static uint32_t num_allocs = 0;

#define bit_byte_index(w) (w >> 3)
#define bit_mask(w) ((uint8_t)(1 << (w & 7)))

static inline void reserve_word(uint32_t w) {
    reserved[bit_byte_index(w)] |= bit_mask(w);
}
static inline void free_word(uint32_t w) {
    reserved[bit_byte_index(w)] &= (uint8_t)~bit_mask(w);
}
static inline int check_word(uint32_t w) {
    return (reserved[bit_byte_index(w)] & bit_mask(w)) != 0;
}

void initHeap(void) {
    for (uint32_t i = 0; i < MAX_WORDS; ++i) heap[i] = 0;
    for (uint32_t i = 0; i < (MAX_WORDS / 8); ++i) reserved[i] = 0;
    for (uint32_t i = 0; i < MAX_ALLOCATIONS; ++i) {
        alloc_ptrs[i] = INVALID_SLOT;
        alloc_sizes[i] = 0;
    }
    num_allocs = 0;
}

void *alloc(const uint32_t sz) {
    if (sz == 0) return (void *)0;
    if (num_allocs >= MAX_ALLOCATIONS) return (void *)0;

    uint32_t words_needed = (sz + 3) / 4;
    if (words_needed == 0 || words_needed > MAX_WORDS) return (void *)0;

    uint32_t run = 0;
    uint32_t start = 0;

    for (uint32_t idx = 0; idx < MAX_WORDS; ++idx) {
        if (!check_word(idx)) {
            if (run == 0) start = idx;
            ++run;
            if (run >= words_needed) break;
        } else {
            run = 0;
        }
    }
    if (run < words_needed) return (void *)0;

    for (uint32_t w = 0; w < words_needed; ++w) reserve_word(start + w);

    uint32_t slot = INVALID_SLOT;
    for (uint32_t s = 0; s < MAX_ALLOCATIONS; ++s) {
        if (alloc_ptrs[s] == INVALID_SLOT) { slot = s; break; }
    }
    if (slot == INVALID_SLOT) {
        for (uint32_t w = 0; w < words_needed; ++w) free_word(start + w);
        return (void *)0;
    }

    alloc_ptrs[slot] = start;
    alloc_sizes[slot] = sz;
    ++num_allocs;

    return (void *)&heap[start];
}

void discard(const void *ptr) {
    if (ptr == (const void *)0) return;

    const uint8_t *byte_ptr = (const uint8_t *)ptr;
    const uint8_t *heap_base = (const uint8_t *)heap;

    if (byte_ptr < heap_base) return;
    uint32_t offset = (uint32_t)(byte_ptr - heap_base);
    if ((offset & 3) != 0) return;
    uint32_t start_word = offset / 4;
    if (start_word >= MAX_WORDS) return;

    int32_t found = -1;
    for (uint32_t s = 0; s < MAX_ALLOCATIONS; ++s) {
        if (alloc_ptrs[s] == start_word) { found = (int32_t)s; break; }
    }
    if (found < 0) return;

    uint32_t sz_bytes = alloc_sizes[found];
    uint32_t words = (sz_bytes + 3) / 4;
    for (uint32_t w = 0; w < words; ++w) free_word(start_word + w);

    alloc_ptrs[found] = INVALID_SLOT;
    alloc_sizes[found] = 0;
    if (num_allocs > 0) --num_allocs;
}

void memcopy(const void *src, void *dest, const uint32_t sz) {
    if (sz == 0 || src == dest) return;
    const uint8_t *s8 = (const uint8_t *)src;
    uint8_t *d8 = (uint8_t *)dest;
    uint32_t b4 = sz & ~3;
    for (uint32_t i = 0; i < b4; i += 4) {
        uint32_t v = *((const uint32_t *)(s8 + i));
        *((uint32_t *)(d8 + i)) = v;
    }
    for (uint32_t i = b4; i < sz; ++i) d8[i] = s8[i];
}
