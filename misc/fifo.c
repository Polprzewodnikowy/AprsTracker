/*
 * fifo.c
 *
 *  Created on: 02.05.2017
 *      Author: korgeaux
 */

#include "misc/fifo.h"

bool FifoIsEmpty(const FifoBuffer *f) {
    return f->head == f->tail;
}

bool FifoIsFull(const FifoBuffer *f) {
    return ((f->head == f->begin) && (f->tail == f->end)) || (f->tail == f->head - 1);
}

void FifoPush(FifoBuffer *f, uint8_t c) {
    *(f->tail) = c;
    if (f->tail == f->end) {
        f->tail = f->begin;
    } else {
        f->tail++;
    }
}

uint8_t FifoPop(FifoBuffer *f) {
    if (f->head == f->end) {
        f->head = f->begin;
        return *(f->end);
    } else {
        return *(f->head++);
    }
}

void FifoFlush(FifoBuffer *f) {
    f->head = f->tail;
}

void FifoInit(FifoBuffer *f, void *buffer, size_t size) {
    f->head = f->tail = f->begin = buffer;
    f->end = buffer + size - 1;
}

size_t FifoLen(FifoBuffer *f) {
    return f->end - f->begin;
}
