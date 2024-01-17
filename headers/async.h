#pragma once

int connect(int size);
void disconnect(int context);
int receive(const char* data, int data_len, int context);
