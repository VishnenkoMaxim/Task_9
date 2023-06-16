#ifndef BULK_CONSOLE_H
#define BULK_CONSOLE_H

#include "all.h"

class Console {
private:
    int _count;
public:
    unsigned int count;

    Console();
    void PrintCommand(const std::string &_str, unsigned int ctx);

    void PrintHead();
    void PrintComma();
    void PrintEnd();
};

#endif //BULK_CONSOLE_H
