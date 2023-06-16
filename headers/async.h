
#ifndef BULK_ASYNC_H
#define BULK_ASYNC_H

#include "all.h"

using namespace std;

unsigned int Connect(unsigned int block_size);
void Receive(const char *data, unsigned int data_len, unsigned int context);
void Disconnect(unsigned int context);

#endif //BULK_ASYNC_H
