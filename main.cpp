
#include <iostream>
#include <fstream>
#include <utility>
#include <cstring>
#include <string>
#include <thread>
#include <future>
#include <vector>

#include "async.h"

using namespace std;

void func(int commands_bulk, const vector<string> &commands){
    int res = connect(commands_bulk);

    for(const auto &it : commands){
        auto status = receive(it.c_str(), it.length(), res);
        if (status != 0) cout << "ERR" << endl;
    }

    disconnect(res);
}

int main() {
    string str;
    vector<string> vec;

    while(!cin.eof()){
        cin >> str;
        vec.push_back(str);
    }

    std::thread th1(func, 25, vec);
    std::thread th2(func, 50, vec);
    std::thread th3(func, 5, vec);

    th1.join();
    th2.join();
    th3.join();

    return 0;
}