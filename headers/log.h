#ifndef BULK_LOG_H
#define BULK_LOG_H

#include "all.h"

using namespace std;

class Log {
private:
    int fd;
    time_t _time;
    static atomic<int> id_file;

public:
    unsigned int count;

    Log();
    void Write(string &_str);
    void Close();
    void SetCurrentBlockTime();
};

#endif //BULK_LOG_H
