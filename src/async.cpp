#include "all.h"
#include "commands.h"
#include "async.h"
#include <map>
#include <memory>

using namespace std;

shared_mutex mtx;
map<unsigned int, shared_ptr<Commands>> commands;
atomic<unsigned int> kernel_context{0};

unsigned int Connect(unsigned int block_size){
    auto new_context = std::make_shared<Commands>(block_size);

    unique_lock<shared_mutex> lock{mtx};
    kernel_context.fetch_add(1);
    commands.insert(make_pair(kernel_context.load(), new_context));
    return kernel_context.load();
}

void Receive(const char *data, unsigned int data_len, unsigned int context){
    unique_lock<shared_mutex> lock{mtx};
    auto it = commands.find(context);
    if(it != commands.end()){
        string cmd(data, data_len);
        it->second->Add(cmd, context);
    }
}

void Disconnect(unsigned int context){
    unique_lock<shared_mutex> lock{mtx};
    auto it = commands.find(context);
    if(it != commands.end()){
        it->second->Stop();
        commands.erase(context);
    }
}