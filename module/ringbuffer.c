
#include "ringbuffer.h"

#include <string.h>

#define RB_ALIGN_SIZE 4
#define RB_ALIGN_DOWN(size,align)	((size)&~((align)-1)) 

enum rbuffer_state rbuffer_status(rbuffer_t *rb)
{
    if (rb->read_index == rb->write_index){
        if (rb->read_mirror == rb->write_mirror)
            return RB_EMPTY;
        else
            return RB_FULL;
    }
    return RB_HALFFULL;
}

/**
 * @brief Get the buffer size of the ring buffer object.
 *
 * @param rb        A pointer to the ring buffer object.
 *
 * @return  Buffer size.
 */
inline uint32_t rt_ringbuffer_get_size(rbuffer_t *rb)
{
    assert(rb != NULL);
    return rb->buffer_size;
}

/**
 * @brief Initialize the ring buffer object.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param pool      A pointer to the buffer.
 * @param size      The size of the buffer in bytes.
 */
void rbuffer_init(rbuffer_t *rb, uint8_t *pool, int32_t size)
{
    assert(rb != NULL);
    assert(size > 0);

    /* initialize read and write index */
    rb->read_mirror  = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr  = pool;
    rb->buffer_size = RB_ALIGN_DOWN(size, RB_ALIGN_SIZE);
}


/**
 * @brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will discard out-of-range data.
 *
 * @param rb            A pointer to the ring buffer object.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of data in bytes.
 *
 * @return Return the data size we put into the ring buffer.
 */
uint32_t rbuffer_put(rbuffer_t *rb, const uint8_t *ptr, uint32_t length)
{
    uint32_t size;

    assert(rb != NULL);

    /* whether has enough space */
    size = rbuffer_space_len(rb);

    /* no space */
    if (size == 0)
        return 0;

    /* drop some data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index], &ptr[0], rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0], &ptr[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index  = length - (rb->buffer_size - rb->write_index);

    return length;
}


/**
 * @brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will overwrite the existing data in the ring buffer.
 *
 * @param rb            A pointer to the ring buffer object.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of data in bytes.
 *
 * @return Return the data size we put into the ring buffer.
 */
uint32_t rbuffer_put_force(rbuffer_t *rb, const uint8_t *ptr, uint32_t length)
{
    uint32_t space_length;

    assert(rb != NULL);

    space_length = rbuffer_space_len(rb);

    if (length > rb->buffer_size)
    {
        ptr = &ptr[length - rb->buffer_size];
        length = rb->buffer_size;
    }

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;

        if (length > space_length)
            rb->read_index = rb->write_index;

        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index], &ptr[0], rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0], &ptr[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index  = length - (rb->buffer_size - rb->write_index);

    if (length > space_length)
    {
        if (rb->write_index <= rb->read_index)
            rb->read_mirror = ~rb->read_mirror;
        rb->read_index = rb->write_index;
    }

    return length;
}


/**
 * @brief Get data from the ring buffer.
 *
 * @param rb            A pointer to the ring buffer.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of the data we want to read from the ring buffer.
 *
 * @return Return the data size we read from the ring buffer.
 */
uint32_t rbuffer_get(rbuffer_t *rb, uint8_t *ptr, uint32_t length)
{
    uint32_t size;

    assert(rb != NULL);

    /* whether has enough data  */
    size = rbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->read_index > length)
    {
        /* copy all of data */
        memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    memcpy(&ptr[0], &rb->buffer_ptr[rb->read_index], rb->buffer_size - rb->read_index);
    memcpy(&ptr[rb->buffer_size - rb->read_index], &rb->buffer_ptr[0], length - (rb->buffer_size - rb->read_index));

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

/**
 * @brief Delete data from the ring buffer.
 *
 * @param rb            A pointer to the ring buffer.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of the data we want to read from the ring buffer.
 *
 * @return Return the data size we delete from the ring buffer.
 */
uint32_t rbuffer_del(rbuffer_t *rb, uint32_t length)
{
    uint32_t size;

    assert(rb != NULL);

    /* whether has enough data  */
    size = rbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->read_index > length){
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}


/**
 * @brief Peek data from the ring buffer.
          Just get data but not to modify the index internal used
 *
 * @param rb            A pointer to the ring buffer.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of the data we want to read from the ring buffer.
 *
 * @return Return the data size we read from the ring buffer.
 */

uint32_t rbuffer_peek(rbuffer_t *rb, uint8_t *ptr, uint32_t length)
{
    uint32_t size;

    assert(rb != NULL);

    /* whether has enough data  */
    size = rbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->read_index > length)
    {
        /* copy all of data */
        memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        return length;
    }

    memcpy(&ptr[0], &rb->buffer_ptr[rb->read_index], rb->buffer_size - rb->read_index);
    memcpy(&ptr[rb->buffer_size - rb->read_index], &rb->buffer_ptr[0], length - (rb->buffer_size - rb->read_index));

    return length;
}

/**
 * @brief Put a byte into the ring buffer. If ring buffer is full, this operation will fail.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param ch        A byte put into the ring buffer.
 *
 * @return Return the data size we put into the ring buffer. The ring buffer is full if returns 0. Otherwise, it will return 1.
 */
uint32_t rbuffer_putchar(rbuffer_t *rb, const uint8_t ch)
{
    assert(rb != NULL);

    /* whether has enough space */
    if (!rbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    }
    else
    {
        rb->write_index++;
    }

    return 1;
}


/**
 * @brief Put a byte into the ring buffer. If ring buffer is full, it will discard an old data and put into a new data.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param ch        A byte put into the ring buffer.
 *
 * @return Return the data size we put into the ring buffer. Always return 1.
 */
uint32_t rbuffer_putchar_force(rbuffer_t *rb, const uint8_t ch)
{
    enum rbuffer_state old_state;

    assert(rb != NULL);

    old_state = rbuffer_status(rb);

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index  = 0;
        if (old_state == RB_FULL)
        {
            rb->read_mirror = ~rb->read_mirror;
            rb->read_index = rb->write_index;
        }
    }
    else
    {
        rb->write_index++;
        if (old_state == RB_FULL)
            rb->read_index = rb->write_index;
    }

    return 1;
}


/**
 * @brief Get a byte from the ring buffer.
 *
 * @param rb        The pointer to the ring buffer object.
 * @param ch        A pointer to the buffer, used to store one byte.
 *
 * @return 0    The ring buffer is empty.
 * @return 1    Success
 */
uint32_t rbuffer_getchar(rbuffer_t *rb, uint8_t *ch)
{
    assert(rb != NULL);

    /* ringbuffer is empty */
    if (!rbuffer_data_len(rb))
        return 0;

    /* put byte */
    *ch = rb->buffer_ptr[rb->read_index];

    if (rb->read_index == rb->buffer_size - 1)
    {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index  = 0;
    }
    else
    {
        rb->read_index++;
    }

    return 1;
}

/**
 * @brief Get the size of data in the ring buffer in bytes.
 *
 * @param rb        The pointer to the ring buffer object.
 *
 * @return Return the size of data in the ring buffer in bytes.
 */
uint32_t rbuffer_data_len(rbuffer_t *rb)
{
    switch (rbuffer_status(rb))
    {
    case RB_EMPTY:
        return 0;
    case RB_FULL:
        return rb->buffer_size;
    case RB_HALFFULL:
    default:
    {
        uint32_t wi = rb->write_index, ri = rb->read_index;

        if (wi > ri)
            return wi - ri;
        else
            return rb->buffer_size - (ri - wi);
    }
    }
}

/**
 * @brief Reset the ring buffer object, and clear all contents in the buffer.
 *
 * @param rb        A pointer to the ring buffer object.
 */
void rbuffer_reset(rbuffer_t *rb)
{
    assert(rb != NULL);

    rb->read_mirror  = 0;
    rb->read_index   = 0;
    rb->write_mirror = 0;
    rb->write_index  = 0;
}

