#pragma once

#include <algorithm>
// #include <functional>
// #include <optional>
#include <string>
#include <vector>

namespace Threader::Threads
{

class CommandLineParser
{
public:
    CommandLineParser()
    {
    }

    CommandLineParser(int argc, char** argv)
    {
        _token.reserve(argc);
        for (int i = 1; i < argc; i++)
        {
            _token.push_back(std::string(argv[i]));
        }
    }

    CommandLineParser(const std::vector<std::string> &arguments)
        : _token(arguments)
    {
    }

    const std::string& optionValue(const std::string& option) const
    {
        auto iterator = std::find(this->_token.begin(), this->_token.end(), option);
        if (iterator != this->_token.end() && ++iterator != this->_token.end())
        {
            return *iterator;
        }
        static const std::string emptyString("");
        return emptyString;
    }

    bool optionExist(const std::string& option) const
    {
        auto it = std::find(this->_token.begin(), this->_token.end(), option);
        return (it != this->_token.end());
    }

    size_t tokenCount() const
    {
        return _token.size();
    }

    std::string token(const size_t index) const
    {
        if (index >= _token.size())
            return std::string();
        return _token[index];
    }

private:
    std::vector<std::string> _token;
};

}
