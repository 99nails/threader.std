#pragma once

#include <list>
// #include <map>
#include <string>

/*
service-binary-name [global-options] [command] [command-option [value]|...]

Global options:
service-binary-name --console|-c
service-binary-name --log-level|-l -1..9
service-binary-name --config|-f file-name

Commands:
service-binary-name install [--name|-n service-name] [--exec-start|-e command-line]
service-binary-name uninstall [--name|-n service-name]
service-binary-name configure --group|-g group-name --short-name|-s short-name
service-binary-name outer-connect --app-id application-id
*/

namespace Threader::Utils
{

class CommandLineParser;

class CommandLineOption
{
    friend CommandLineParser;
public:
    explicit CommandLineOption(const std::string &description,
                               const std::string &longName,
                               const std::string &shortName,
                               const bool expectValue = false,
                               const bool expectOption = false);

    const std::string &description() const;

    const std::string &longName() const;

    const std::string &shortName() const;

    bool expectValue() const;

    bool expectOption() const;

    const std::string &value() const;

protected:
    void setValue(const std::string &value);

private:
    std::string _description;
    std::string _longName;
    std::string _shortName;
    bool _expectValue;
    bool _expectOption;
    std::string _value;

};


using OptionList = std::list<CommandLineOption>;

class CommandLineCommand
{
public:
    explicit CommandLineCommand(const std::string &description = std::string(),
                                const std::string &command = std::string(),
                                const OptionList &options = {});

    std::string description() const;
    std::string command() const;
    OptionList options();

    CommandLineOption *findOption(const std::string &name);

private:
    std::string _description;
    std::string _command;
    OptionList _options;
};

using CommandList = std::list<CommandLineCommand>;


class CommandLineParser
{
public:
    explicit CommandLineParser(const OptionList &globalOptions = {},
                               const CommandList &commands = {});

    explicit CommandLineParser(const CommandLineParser &other);

    explicit CommandLineParser(int argc, char** argv);

    explicit CommandLineParser(const std::list<std::string> &arguments);

    void setArguments(const std::list<std::string> &arguments);

    void setArguments(int argc, char** argv);

    const std::string& optionValue(const std::string& option) const;

    bool optionExist(const std::string& option) const;

    size_t tokenCount() const;

    std::string binaryFileName() const;

    bool parse(std::string *error = nullptr);

    const OptionList &parsedGlobalOptions();

    CommandLineCommand parsedCommand();

    std::string help();

    void addOption(const CommandLineOption &option);

    void addCommand(const CommandLineCommand &command);

    bool hasParsedOption(const std::string& name,
                         std::string *value = nullptr);

private:
    CommandLineOption *findGlobalOption(const std::string &name);

    const CommandLineCommand *findCommand(const std::string &name);

private:
    std::list<std::string> _token;

    std::string _binaryName;

    OptionList _globalOptionsList;
    CommandList _commandsList;

    OptionList _parsedGlobalOptions;
    CommandLineCommand _parsedCommand;
};


#define COMM_OPTION(DESC, LONG, SHORT, VALUE, OPTION) \
    appInfo().commandLine().addOption(CommandLineOption(DESC, LONG, SHORT, VALUE, OPTION))


#define COMM_COMMAND(DESC, COMMAND, OPTIONS) \
    appInfo().commandLine().addCommand(CommandLineCommand(DESC, COMMAND, OPTIONS))


}
