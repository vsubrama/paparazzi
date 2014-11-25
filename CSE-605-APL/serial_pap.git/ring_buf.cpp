/*
 * ring_buf.c
 *
 *  Created on: May 28, 2014
 *      Author: j3
 */
#include "ring_buf.h"
#include "common.h"
#include <iostream>
#include <QtCore/QDebug>



unsigned int ringBuffer::modulo_inc(const unsigned int value, const unsigned int modulus) {
	unsigned int my_value = value + 1;
	if (my_value >= modulus) {
		my_value = 0;
	}
	return (my_value);
}

unsigned int ringBuffer::modulo_dec(const unsigned int value, const unsigned int modulus) {
	unsigned int my_value = (0 == value) ? (modulus - 1) : (value - 1);
	return (my_value);
}

void ringBuffer::putBlock(unsigned char* c, int quantity) {
	//Take all of the bytes and put in the circular buffer
	while (quantity--)
        put(*c++);
}

void ringBuffer::put(unsigned char c) {
    if (count < RBUF_SIZE) {
        buf[head] = c;
        head = modulo_inc(head, RBUF_SIZE);
        ++count;
	}
}

int ringBuffer::isEmpty() {
    return (0 == count);
}

void ringBuffer::flush(const int clearBuffer) {
    count = 0;
    head = 0;
    tail = 0;
	if (clearBuffer) {
        memset(buf, 0, sizeof(buf));
	}
}

int ringBuffer::isFull() {
    return (count >= RBUF_SIZE);
}

unsigned char ringBuffer::get() {
	unsigned char c;
    if (count > 0) {
        c = buf[tail];
        tail = modulo_inc(tail, RBUF_SIZE);
        --count;
	} else {
		c = -1;
	}
	return (c);
}

int ringBuffer::getSize(){
    return count;
}

