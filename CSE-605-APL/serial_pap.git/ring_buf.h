/*
 * ring_buf.h
 *
 *  Created on: May 28, 2014
 *      Author: j3
 */

#ifndef RING_BUF_H_
#define RING_BUF_H_

#define RBUF_SIZE    768

class ringBuffer{
public:
    unsigned char * buf;
    int head;
    int tail;
    int count;

    ringBuffer(){
         buf = new unsigned char [RBUF_SIZE];
        head=tail=count=0;
    }
    ~ringBuffer(){
        delete[] buf;
    }

    int isEmpty();
    int isFull();
    unsigned char get();
    void put(unsigned char c);
    void putBlock(unsigned char* c, int quantity);
    void flush(const int clearBuffer);
    int getSize();

private:
    unsigned int modulo_inc(const unsigned int value, const unsigned int modulus);
    unsigned int modulo_dec(const unsigned int value, const unsigned int modulus);

};

#endif /* RING_BUF_H_ */
