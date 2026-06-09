
#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ring buffer */
struct rbuffer
{
    uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     */

    uint32_t read_mirror : 1;
    uint32_t read_index : 31;
    uint32_t write_mirror : 1;
    uint32_t write_index : 31;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    int32_t buffer_size;
};

typedef struct rbuffer rbuffer_t;

enum rbuffer_state
{
    RB_EMPTY,
    RB_FULL,
    /* half full is neither full nor empty */
    RB_HALFFULL,
};

/** return the size of empty space in rb */
#define rbuffer_space_len(rb) ((rb)->buffer_size - rbuffer_data_len(rb))


void rbuffer_init(rbuffer_t *rb, uint8_t *pool, int32_t size);
void rbuffer_reset(rbuffer_t *rb);
uint32_t rbuffer_put(rbuffer_t *rb, const uint8_t *ptr, uint32_t length);
uint32_t rbuffer_put_force(rbuffer_t *rb, const uint8_t *ptr, uint32_t length);
uint32_t rbuffer_putchar(rbuffer_t *rb, const uint8_t ch);
uint32_t rbuffer_putchar_force(rbuffer_t *rb, const uint8_t ch);
uint32_t rbuffer_del(rbuffer_t *rb, uint32_t length);
uint32_t rbuffer_get(rbuffer_t *rb, uint8_t *ptr, uint32_t length);
uint32_t rbuffer_peek(rbuffer_t *rb, uint8_t *ptr, uint32_t length);
uint32_t rbuffer_getchar(rbuffer_t *rb, uint8_t *ch);
uint32_t rbuffer_data_len(rbuffer_t *rb);
enum rbuffer_state rbuffer_status(rbuffer_t *rb);


#ifdef __cplusplus
}
#endif

#endif
