#include "log.h"

Log::Log() : fd(-1), _time() {}

void Log::Write(string &_str){
    if (fd == -1){
        char buf[64];
        sprintf(buf,"%ld",_time);
        string file_name = "bulk_";
        file_name.append(buf);
        file_name.append("_");

        auto s_id = std::to_string(id_file++);
        file_name.append(s_id);
        file_name.append(".log");
        fd = open(file_name.c_str(), O_WRONLY | O_CREAT);
    }
    if (fd > 0){
        _str += "\n";
        write(fd, _str.c_str(), _str.size());
    }
}

void Log::Close(){
    close(fd);
    fd = -1;
}

void Log::SetCurrentBlockTime(){
    _time = time(nullptr);
}

atomic<int> Log::id_file = 0;
