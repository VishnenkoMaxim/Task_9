#include <iostream>
#include <fstream>
#include <utility>
#include <cstring>
#include <string>
#include <queue>
#include <mutex>
#include <filesystem>
#include <condition_variable>
#include <thread>
#include <shared_mutex>
#include <list>

#include "async.h"

#define OPEN_DYNAMIC "{"
#define CLOSE_DYNAMIC "}"

using namespace std;

class ICommand {
public:
    virtual void execute() = 0;
    virtual ~ICommand() = default;
};

class LogCommand : public ICommand{
public:
    explicit LogCommand(string _cmd) : cmd(std::move(_cmd)){};

    void execute() override {
        cout << cmd;
    }

private:
    string cmd;
};

class FileCommand : public ICommand{
public:
    FileCommand(shared_ptr<ofstream> _stream, string  _cmd) : stream(std::move(_stream)), cmd(std::move(_cmd)){}
    void execute() override {
        stream->write(cmd.c_str(), cmd.length());
        stream->flush();
    }
private:
    shared_ptr<ofstream> stream;
    string cmd;
};

class Log{
public:
    void add(ICommand * _cmd) {
        lock_guard<mutex> lock{mtx};
        command.push(_cmd);
        cond_var.notify_all();
    }

    void execute(){
        while(true) {
            unique_lock<mutex> lock{mtx};
            while(command.empty()){
                if (flag_stop && command.empty()) {
                    return;
                }
                cond_var.wait(lock);
            }

            auto cmd = command.front();
            command.pop();
            cmd->execute();
        }
    }

    void stop(){
        flag_stop = true;
        cond_var.notify_all();
    }
private:
    queue<ICommand *> command;
    mutex mtx;
    condition_variable cond_var;
    bool flag_stop = false;
};

class Commands{
public:
    explicit Commands(unsigned int NUM, int _context) : state(0), N(NUM), context(_context) {
        th_log = thread(&Log::execute, &logger);
        th_file = thread(&Log::execute, &file_logger);
        th_file2 = thread(&Log::execute, &file_logger);
    }

    void add(const string& _cmd){
        if (_cmd == OPEN_DYNAMIC){
            if (++state <= 1){
                execute();
                return;
            }
        } else if (_cmd == CLOSE_DYNAMIC) {
            if (--state <= 0) {
                execute();
                state = 0;
                return;
            }
        }

        if (_cmd != OPEN_DYNAMIC && _cmd != CLOSE_DYNAMIC){
            commands.push(_cmd);
        }

        if (commands.size() >= N && state == 0) {
            execute();
        }
    }

    ~Commands(){
        execute();

        logger.stop();
        file_logger.stop();

        th_log.join();
        th_file.join();
        th_file2.join();
    }

private:
    Log logger;
    Log file_logger;

    std::thread th_log;
    std::thread th_file;
    std::thread th_file2;

    int state;
    unsigned int N;
    int context;
    queue<string> commands;

    void execute(){
        static int file_count = 0;
        time_t cur_time = time(nullptr);
        file_count++;

        auto file = make_shared<ofstream>();
        file->open("bulk_" + std::to_string(cur_time) + "_" + std::to_string(context) + "_" + std::to_string(file_count) + ".log");

        while (!commands.empty()){
            auto cmd = commands.front();
            commands.pop();
            if (!commands.empty()) logger.add(new LogCommand(cmd + ','));
            else logger.add(new LogCommand(cmd + '\n'));
            file_logger.add(new FileCommand(file, cmd + '\n'));
        }
    }
};

class Commander{
public:
    int createContext(const unsigned int N){
        lock_guard<shared_mutex> lock{mtx};
        context_id++;
        commands_vec.emplace_back(context_id, make_unique<Commands>(N, context_id));

        return context_id;
    }

    void deleteContext(int context){
        lock_guard<shared_mutex> lock{mtx};
        for(auto it = commands_vec.begin(); it != commands_vec.end(); it++){
            if ((*it).first == context){
                commands_vec.erase(it);
                return;
            }
        }
    }

    int write(const char* data, const int data_len, const int context){
        shared_lock<shared_mutex> lock{mtx};
        for(auto &it : commands_vec){
            if (it.first == context){
                it.second->add(string(data, data_len));
                lock.unlock();
                return 0;
            }
        }
        lock.unlock();
        return -1;
    }

private:
    shared_mutex mtx;
    inline static int context_id = 0;
    list<pair<int, unique_ptr<Commands>>> commands_vec;
};

Commander commander;

int connect(const int size){
    auto context = commander.createContext(size);
    return context;
}

void disconnect(const int context){
    commander.deleteContext(context);
}

int receive(const char* data, const int data_len, const int context){
    return commander.write(data, data_len, context);
}

