#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace Threader::Threads
{

class CommandLineParser
{
public:
    CommandLineParser(int argc, char** argv)
    {
        for (int i = 1; i < argc; i++)
        {
            this->_tokens.push_back(std::string(argv[i]));
        }
    }

    const std::string& optionValue(const std::string& option) const
    {
        auto iterator = std::find(this->_tokens.begin(), this->_tokens.end(), option);
        if (iterator != this->_tokens.end() && ++iterator != this->_tokens.end())
        {
            return *iterator;
        }
        static const std::string emptyString("");
        return emptyString;
    }

    bool optionExist(const std::string& option) const
    {
        auto it = std::find(this->_tokens.begin(), this->_tokens.end(), option);
        return (it != this->_tokens.end());
    }

private:
    std::vector<std::string> _tokens;
};

}
