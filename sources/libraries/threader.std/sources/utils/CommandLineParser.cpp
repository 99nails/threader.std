#include "CommandLineParser.h"

#include "FileUtils.h"
#include "StdStringUtils.h"

#include <algorithm>

namespace Threader::Utils
{

/***************************************************************************************************
 * CommandLineOption
 **************************************************************************************************/

CommandLineOption::CommandLineOption(const std::string &description,
                                     const std::string &longName,
                                     const std::string &shortName,
                                     const bool expectValue,
                                     const bool expectOption)
    : _description(description)
    , _longName(longName)
    , _shortName(shortName)
    , _expectValue(expectValue)
    , _expectOption(expectOption)
    , _value(std::string())
{
}

const std::string &CommandLineOption::description() const
{
    return _description;
}

const std::string &CommandLineOption::longName() const
{
    return _longName;
}

const std::string &CommandLineOption::shortName() const
{
    return _shortName;
}

bool CommandLineOption::expectOption() const
{
    return _expectOption;
}

bool CommandLineOption::expectValue() const
{
    return _expectValue;
}

const std::string &CommandLineOption::value() const
{
    return _value;
}

void CommandLineOption::setValue(const std::string &value)
{
    _value = value;
}

/***************************************************************************************************
 * CommandLineCommand
 **************************************************************************************************/

CommandLineCommand::CommandLineCommand(const std::string &description,
                                       const std::string &command,
                                       const OptionList &options)
    : _description(description)
    , _command(command)
    , _options(options)
{
}

std::string CommandLineCommand::description() const
{
    return _description;
}

std::string CommandLineCommand::command() const
{
    return _command;
}

OptionList CommandLineCommand::options()
{
    return _options;
}

CommandLineOption *CommandLineCommand::findOption(const std::string &name)
{
    for (auto &option : _options)
    {
        if (option.longName() == name || option.shortName() == name)
            return &option;
    }
    return nullptr;

}

/***************************************************************************************************
 * CommandLineParser
 **************************************************************************************************/

CommandLineParser::CommandLineParser(const OptionList &globalOptions,
                                     const CommandList &commands)
    : _globalOptionsList(globalOptions)
    , _commandsList(commands)
{
}

CommandLineParser::CommandLineParser(const CommandLineParser &other)
    : _token(other._token)
{
}

CommandLineParser::CommandLineParser(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        _token.push_back(std::string(argv[i]));
    }
}

CommandLineParser::CommandLineParser(const std::list<std::string> &arguments)
    : _token(arguments)
{
}

void CommandLineParser::setArguments(const std::list<std::string> &arguments)
{
    _token = arguments;
}

void CommandLineParser::setArguments(int argc, char **argv)
{
    _token.clear();
    for (int i = 0; i < argc; i++)
        _token.push_back(argv[i]);
}

const std::string &CommandLineParser::optionValue(const std::string &option) const
{
    auto iterator = std::find(_token.begin(), _token.end(), option);
    if (iterator != _token.end() && ++iterator != _token.end())
    {
        return *iterator;
    }
    static const std::string emptyString("");
    return emptyString;
}

bool CommandLineParser::optionExist(const std::string &option) const
{
    auto it = std::find(this->_token.begin(), this->_token.end(), option);
    return (it != this->_token.end());
}

size_t CommandLineParser::tokenCount() const
{
    return _token.size();
}

std::string CommandLineParser::binaryFileName() const
{
    return _binaryName;
}

bool CommandLineParser::parse(std::string *error)
{
    enum ParseState
    {
        GlobalOptions,
        Command,
        CommandOption,
        CommandOptionValue,
        CommandTail
    };

    auto dequeue = [](std::list<std::string> &list, std::string &first) -> bool
    {
        if (list.empty())
            return false;

        first = list.front();
        return true;
    };

    auto localToken = _token;
    std::string token;

    if (!dequeue(localToken, _binaryName))
        return false;

    localToken.erase(localToken.begin());

    ParseState state = GlobalOptions;
    while (dequeue(localToken, token))
    {
        if (state == GlobalOptions)
        {
            if (token[0] == '-')
            {
                auto option = findGlobalOption(token);
                if (!option)
                {
                    if (error)
                        *error = "неизвестная опция " + token;
                    return false;
                }

                if (option->expectValue())
                {
                    localToken.erase(localToken.begin());
                    std::string argument;
                    if (!dequeue(localToken, argument))
                    {
                        if (error)
                            *error = "не задано значение " + token;
                        return false;
                    }
                     option->setValue(argument);
                }

                _parsedGlobalOptions.push_back(*option);
            }
            else
            {
                state = Command;
                continue;
            }
        }

        if (state == Command)
        {
            auto command = findCommand(token);
            if (!command)
            {
                if (error)
                    *error = "неизвестная команда " + token;
                return false;
            }

            _parsedCommand = *command;
            state = CommandOption;
            localToken.erase(localToken.begin());
            continue;
        }

        if (state == CommandOption)
        {
            auto option = _parsedCommand.findOption(token);

            if (!option)
            {
                if (error)
                    *error = "неизвестная опция " + token;
                return false;
            }

            if (option->expectValue())
            {
                localToken.erase(localToken.begin());
                std::string argument;

                if (!dequeue(localToken, argument))
                {
                    if (error)
                        *error = "не задано значение " + token;
                    return false;
                }

                option->setValue(argument);
            }
        }

        localToken.erase(localToken.begin());
    }

    // check all options are set
    for (const auto &option: _parsedCommand.options())
    {
        if (option.expectValue() && option.value().empty())
        {
            if (error)
                *error = "не задано значение " + option.longName();
            return false;
        }
    }

    return true;
}

CommandLineOption *CommandLineParser::findGlobalOption(const std::string &name)
{
    for (auto &option : _globalOptionsList)
    {
        if (option.longName() == name || option.shortName() == name)
            return &option;
    }
    return nullptr;
}

const CommandLineCommand *CommandLineParser::findCommand(const std::string &name)
{
    for (auto &command : _commandsList)
    {
        if (command.command() == name)
            return &command;
    }
    return nullptr;
}

CommandLineCommand CommandLineParser::parsedCommand()
{
    return _parsedCommand;
}

std::string CommandLineParser::help()
{
    std::string result = FileUtils::extractFileName(_binaryName);

    std::string globalOptions = "\r\nГлобальные опции:\r\n";

    if (_globalOptionsList.size() > 0)
    {
        result += " [global-options]";

        for (const auto &option : _globalOptionsList)
        {
            globalOptions += "\t" + option.shortName() + "\t" + option.longName() + "\t- " + option.description() + "\r\n";
        }
    }

    std::string commands = "\r\nКоманды:\r\n";

    if (_commandsList.size() > 0)
    {
        result += " [command [command-option [value|...]]]";

        for (auto &command : _commandsList)
        {
            commands += "    " +
                        StdStringUtils::appendChar(command.command(), 10, ' ')
                        + " - " + command.description() + "\r\n";

            for (const auto &option : command.options())
            {
                commands += "        " +
                            StdStringUtils::appendChar(option.shortName(), 8, ' ') +
                            StdStringUtils::appendChar(option.longName(), 15, ' ') + " - " + option.description() + "\r\n";
            }

            commands += "\r\n";
        }
    }

    result += "\r\n" + globalOptions + commands;

    return result;
}

const OptionList &CommandLineParser::parsedGlobalOptions()
{
    return _parsedGlobalOptions;
}

}
