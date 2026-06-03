#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

// we only declare the "BuiltInCommand" container and not call the object function.
class BuiltInCommand;
class ShellModel;

class BuiltInCommandContainer
{
    public:
    BuiltInCommandContainer(std::unique_ptr<ShellModel> theModel)
    {
        
    }
    // 1. recieves model for items 
    // 2. container for commands
    // 3. constrctor that creates objects
    // 4. findCommand - recieves command name and returns object. if not exists reuturn NULL.
    // 5. excuteInternalCommand - return true if command found. false if not.
    // on h file only builtincommandcontainer and abstract class builtincommand exists.

    private:
    // Using a map for O(1) lookup of built-in commands
    std::map<std::string, std::unique_ptr<BuiltInCommand>> builtins; 

    // Register built-in commands
    void addCommand(std::make_unique<CdCommand>());
    void addCommand(std::make_unique<ExitCommand>());
}