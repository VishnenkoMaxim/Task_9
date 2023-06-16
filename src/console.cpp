#include "console.h"

using namespace std;

Console::Console() : _count(0){};

void Console::PrintCommand(const string &_str, unsigned int ctx){
    if (_count == 0) {
        cout << "(context: " << ctx << ") ";
        PrintHead();
    }
    _count++;
    cout << _str;
}

void Console::PrintHead(){cout << "bulk: ";}
void Console::PrintComma(){cout << ",";}

void Console::PrintEnd(){
    _count = 0;
    cout << endl;
}




