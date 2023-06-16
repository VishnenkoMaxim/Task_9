#include "commands.h"
#include "async.h"

using namespace std;

ConsoleCommand::ConsoleCommand(shared_ptr<Console> _console) : console(std::move(_console)) {}
LogCommand::LogCommand(shared_ptr<Log> _log): log(std::move(_log)) {}

PrintCommand::PrintCommand(const shared_ptr<Console> &_console, string _cmd, const unsigned int _ctx) : ConsoleCommand(_console), cmd(std::move(_cmd)), ctx(_ctx){
    _console->count++;
}

void PrintCommand::Execute() {
    console->PrintCommand(cmd, ctx);
    console->count--;
    if(console->count != 0) console->PrintComma();
    else console->PrintEnd();
}

LogWriteCommand::LogWriteCommand(const shared_ptr<Log>& _lg, string _cmd, const unsigned int _ctx) : LogCommand(_lg), cmd(std::move(_cmd)) {
    _lg->count++;
    if (_lg->count == 1) log->SetCurrentBlockTime();
}

void LogWriteCommand::Execute() {
    log->Write(cmd);
    log->count--;
    if (log->count == 0) log->Close();

}

void Commands::ConsoleThread(Commands *_com){
    while(!_com->stop_flag){
        _com->ExecuteConsole();
    }
}

void Commands::LogThread(Commands *_com){
    while(!_com->stop_flag){
        _com->ExecuteLog();
    }
}

void Commands::ExecuteConsole(){
    unique_lock<mutex> lock{console_mutex};

    while(commands.empty() || !wait){
        console_cond.wait(lock);
        if(stop_flag) return;
    }

    for (const auto &it: commands){
        it->Execute();
    }
    commands.clear();

    wait = false;
    console_cond.notify_all();
}

void Commands::ExecuteLog(){
    unique_lock<mutex> lock{log_mutex};
    while(log_commands.empty() || !wait){
        log_cond.wait(lock);
        if(stop_flag) return;
    }

    while(!log_commands.empty()) {
        log_commands.front()->Execute();
        log_commands.pop();
    }
    log_cond.notify_all();
}

Commands::Commands(const unsigned short n) : N(n), state(0) {
    console = std::make_shared<Console>();
    lg = std::make_shared<Log>();
    Start();
}

void Commands::Start(){
    thread TConsole{ConsoleThread, this};
    TConsole.detach();

    thread Log{LogThread, this};
    thread Log2{LogThread, this};
    Log.detach();
    Log2.detach();
}

void Commands::Add(string &cmd, const unsigned int _ctx){
    unique_lock<mutex> lock{console_mutex};
    if(wait && commands.empty()) wait = false;

    while(wait){
        console_cond.wait(lock);
    }
    if (cmd == OPEN_DYNAMIC){
        if (++state <= 1){
            wait = true;
            console_cond.notify_all();
            log_cond.notify_all();
            return;
        }
    } else if (cmd == CLOSE_DYNAMIC){
        if (--state <= 0){
            state = 0;
            wait = true;
            console_cond.notify_all();
            log_cond.notify_all();
            return;
        }
    }

    unique_lock<mutex> log_lock{log_mutex};
    if (cmd != OPEN_DYNAMIC && cmd != CLOSE_DYNAMIC){
        commands.emplace_back(new PrintCommand(console, cmd, _ctx));
        log_commands.emplace(new LogWriteCommand(lg, cmd, _ctx));
    }

    if (commands.size() >= N){
        wait = true;
        lock.unlock();
        log_lock.unlock();
        console_cond.notify_all();
        log_cond.notify_all();
    }
}

void Commands::Stop() {
    stop_flag = true;
    console_cond.notify_all();
    log_cond.notify_all();
}

mutex Commands::console_mutex;
condition_variable Commands::console_cond;