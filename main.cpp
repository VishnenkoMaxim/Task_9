#include "all.h"

#include "async.h"

#include <list>

unsigned short N;
list<string> commands;

void Thread(){
    unsigned int context = Connect(N);

    for(const auto &it : commands){
        Receive(it.c_str(), it.size(), context);
    }

    Disconnect(context);
}

int main(int argc, char  **argv) {
    if (argc < 2){
        N = 3;
    } else N = atoi(argv[1]);

    while(true){
        if (cin.eof()){
            break;
        }
        string cmd;
        cin >> cmd;
        commands.push_back(cmd);
    }

    thread th{Thread};
    thread th2{Thread};

    th.join();
    th2.join();

    return 0;
}
