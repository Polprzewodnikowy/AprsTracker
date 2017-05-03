/*
 * fifo.h
 *
 *  Created on: 02.05.2017
 *      Author: korgeaux
 */

#ifndef MISC_FIFO_H_
#define MISC_FIFO_H_

#include "misc/def.h"

typedef struct FifoBuffer {
    uint8_t *begin;
    uint8_t *end;
    uint8_t *volatile head;
    uint8_t *volatile tail;
} FifoBuffer;

bool FifoIsEmpty(const FifoBuffer *f);
bool FifoIsFull(const FifoBuffer *f);
void FifoPush(FifoBuffer *f, uint8_t c);
uint8_t FifoPop(FifoBuffer *f);
void FifoFlush(FifoBuffer *f);
void FifoInit(FifoBuffer *f, void *buffer, size_t size);
size_t FifoLen(FifoBuffer *f);

#endif /* MISC_FIFO_H_ */
