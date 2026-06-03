#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

// --- Command Pattern: Interface ---
class BuiltInCommand 
{
public:
    virtual ~BuiltInCommand() = default;
    virtual std::string getName() const = 0;
    virtual void execute(const std::vector<std::string>& args) = 0;
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

// --- The Main Executor ---
class ShellExecutor 
{
private:
    // Using a map for O(1) lookup of built-in commands
    std::map<std::string, std::unique_ptr<BuiltInCommand>> builtins;

public:
    ShellExecutor() 
    {
        // Register built-in commands
        addCommand(std::make_unique<CdCommand>());
        addCommand(std::make_unique<ExitCommand>());
    }

    void addCommand(std::unique_ptr<BuiltInCommand> cmd) 
    {
        builtins[cmd->getName()] = std::move(cmd);
    }

    void run(const std::string& input) 
    {
        auto args = parse(input);
        if (args.empty()) return;

        // 1. Check if it's a Built-in
        if (builtins.contains(args[0])) {
            builtins[args[0]]->execute(args);
        } 
        // 2. If not, Fork & Execv (External Command)
        else {
            executeExternal(args);
        }
    }

private:
    std::vector<std::string> parse(const std::string& input) 
    {
        std::vector<std::string> args;
        std::istringstream iss(input);
        std::string token;
        while (iss >> token) args.push_back(token);
        return args;
    }

    void executeExternal(const std::vector<std::string>& args) 
    {
        // Prepare C-style arguments
        std::vector<char*> c_args;
        for (const auto& a : args) c_args.push_back(const_cast<char*>(a.c_str()));
        c_args.push_back(nullptr);

        if (pid_t pid = fork(); pid == -1) 
        {
            perror("fork failed");
        } 
        else if (pid == 0) 
        {
            // Child: execvp handles PATH searching automatically
            execvp(c_args[0], c_args.data());
            perror("Execution failed");
            exit(EXIT_FAILURE);
        } 
        else 
        {
            // Parent: Wait
            waitpid(pid, nullptr, 0);
        }
    }
};
/*
int main() {
    ShellExecutor shell;
    std::string line;

    while (true) {
        std::cout << fs::current_path().string() << " # ";
        if (!std::getline(std::cin, line)) break;
        shell.run(line);
    }
    return 0;
}*/
