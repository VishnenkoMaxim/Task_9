#ifndef BULK_COMMANDS_H
#define BULK_COMMANDS_H

#include "all.h"
#include "console.h"
#include "log.h"

#define OPEN_DYNAMIC "{"
#define CLOSE_DYNAMIC "}"

class ICommand{
public:
    virtual void Execute() = 0;
    virtual ~ICommand() = default;
};

class ConsoleCommand: public ICommand{
protected:
    shared_ptr<Console> console;
    explicit ConsoleCommand(shared_ptr<Console> _console);
public:
    ~ConsoleCommand() override = default;
};

class LogCommand: public ICommand{
protected:
    shared_ptr<Log> log;
    explicit LogCommand(shared_ptr<Log> _log);
public:
    ~LogCommand() override = default;
};

class PrintCommand: public ConsoleCommand {
    string cmd;
    unsigned int ctx;
public:
    PrintCommand(const shared_ptr<Console> &_console, string _cmd, unsigned int _ctx);
    void Execute() override;
};

class LogWriteCommand : public LogCommand{
private:
    string cmd;
public:
    LogWriteCommand(const shared_ptr<Log>& _lg, string _cmd, unsigned int _ctx);
    void Execute() override;
};

class Commands{
private:
    vector<shared_ptr<ICommand>> commands;
    queue<shared_ptr<ICommand>> log_commands;
    shared_ptr<Console> console;
    shared_ptr<Log> lg;

    unsigned short N;
    int state;
    atomic<bool> stop_flag{false};

    static mutex console_mutex;
    static condition_variable console_cond;

    mutex log_mutex;
    condition_variable log_cond;
    atomic<bool> wait{false};

    static void ConsoleThread(Commands *_com);
    static void LogThread(Commands *_com);
    void ExecuteConsole();
    void ExecuteLog();

public:
    explicit Commands(unsigned short n);
    void Start();
    void Stop();
    void Add(string &cmd, unsigned int _ctx);

    ~Commands() = default;
};

#endif //BULK_COMMANDS_H
