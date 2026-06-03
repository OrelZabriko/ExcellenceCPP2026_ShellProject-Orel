#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

class ShellModel;
// --- Command Pattern: Interface ---
class BuiltInCommand 
{
private:
    ShellModel *model; //change to smart pointer
public:
    virtual ~BuiltInCommand() = default;
    virtual std::string getName() const = 0; // identify the command - "cd" "myhistory"
    virtual void execute(const std::vector<std::string>& args) = 0;
    
    public BuiltInCommand() {} // not needed and illegal
    public BuiltInCommand(ShellModel *theModel) { model = theModel;}
};

// --- Concrete Built-in: cd ---
class CdCommand : public BuiltInCommand 
{
public:
    std::string getName() const override { return "cd"; }
    void execute(const std::vector<std::string>& args) override 
    {
        if (args.size() < 2) 
        {
            std::cerr << "cd: missing argument\n";
            return;
        }
        std::error_code ec;
        fs::current_path(args[1], ec);
        if (ec) std::cerr << "cd failed: " << ec.message() << "\n";
    }
};

// --- Concrete Built-in: exit ---
class ExitCommand : public BuiltInCommand 
{
public:
    std::string getName() const override { return "exit"; }
    void execute(const std::vector<std::string>& args) override 
    {
        std::cout << "Exiting shell...\n";
        exit(0);
    }
};