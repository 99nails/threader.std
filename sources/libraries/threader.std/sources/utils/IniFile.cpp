#include "IniFile.h"

#include <regex>
#include <iostream>
#include <fstream>

// #include "DateTimeUtils.h"
// #include <QFile>
// #include <QRegExp>
// #include <QRegularExpression>
// #include <QVariant>


IniLineBase::IniLineBase(const CommentList &commentList,
                         const LineType lineType)
    : _content(std::string())
    , _lineType(lineType)
    , _commentList(commentList)
{
}

const std::string &IniLineBase::content() const
{
    return _content;
}

LineType IniLineBase::lineType() const
{
    return _lineType;
}

bool IniLineBase::parse(const std::string &line)
{
    _content = line;
    return true;
}

std::string IniLineBase::toString()
{
    return _content;
}

std::string IniLineBase::commentString()
{
    std::string result;
    for (const auto &comment: _commentList)
    {
        if (!result.empty())
            result += "\r\n";
        result += comment->toString();
    }
    return result;
}

CommentList IniLineBase::commentList() const
{
    return _commentList;
}

/**************************************************************************************************/

IniLineComment::IniLineComment()
    : IniLineBase({},
                  LineType::Comment)
{
}

bool IniLineComment::parse(const std::string &line)
{
    if (line.size() < 1 || line[0] != ';')
        return false;

    _comment = line.substr(1, line.size() - 1);
    return true;
}

std::string IniLineComment::comment() const
{
    return _comment;
}

std::string IniLineComment::toString()
{
    return ";" + comment();
}

/**************************************************************************************************/

IniLineParameter::IniLineParameter(const CommentList &commentList)
    : IniLineBase(commentList,
                  LineType::Parameter)
    , _name(std::string())
    , _value(std::string())
{
}

bool IniLineParameter::parse(const std::string &line)
{
    auto position = line.find('=');
    if (std::string::npos == position)
        return false;

    _name = line.substr(0, position);
    _value = line.substr(position + 1, line.length() - position - 1);
    return true;
}

std::string IniLineParameter::name() const
{
    return _name;
}

std::string IniLineParameter::value() const
{
    return _value;
}

void IniLineParameter::setValue(const std::string &value)
{
    _value = value;
}

std::string IniLineParameter::toString()
{
    return _name + "=" +_value;
}

/**************************************************************************************************/

IniLineSection::IniLineSection(const CommentList &commentList)
    : IniLineBase(commentList,
                  LineType::Section)
{
}

void IniLineSection::clear()
{
    _parameterHash.clear();
    _parameterList.clear();
}

bool IniLineSection::parse(const std::string &line)
{
    auto start = line.find('[');
    auto finish = line.find(']');
    if (start == std::string::npos || finish == std::string::npos)
        return false;

    _name = line.substr(start + 1, finish - start - 1);

    return true;
}

std::string IniLineSection::name() const
{
    return _name;
}

std::string IniLineSection::toString()
{
    return "[" + _name + "]";
}

void IniLineSection::append(IniLineParameter *parameter)
{
    _parameterHash[parameter->name()] = parameter;
    _parameterList.push_back(parameter);
}

IniLineParameter *IniLineSection::parameterByName(const std::string &parameterName)
{
    if (_parameterHash.count(parameterName))
        return _parameterHash[parameterName];
    else
        return nullptr;
}

int IniLineSection::parameterCount() const
{
    return _parameterList.size();
}

IniLineParameter *IniLineSection::parameterByIndex(const int index) const
{
    if (index < 0 || index >= _parameterList.size())
        return nullptr;

    auto iterator = _parameterList.begin();
    std::advance(iterator, index);
    return *iterator;
    // return _parameterList[index];
}

/**************************************************************************************************/

IniFile::IniFile()
{
}

IniFile::~IniFile()
{
    clear();
}

bool IniFile::fileOpen(const std::string &fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in);
    if (!file.is_open())
        return false;

    std::string line;
    StdStringList lines;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    file.close();

    setContent(lines);

    return true;
}

bool IniFile::fileSave(const std::string &fileName,
                       const bool doBackup)
{

    // QFile file(fileName);

    // if (doBackup && file.exists())
    // {
    //     auto fileNameBackup = fileName + ".copy." +
    //                           QDateTime::currentDateTime().toString("yyMMdd-HHmmss") +
    //                           ".ini";
    //     file.copy(fileName, fileNameBackup);
    // }

    // if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
    //     return false;

    // auto contentString = _content.join("\r\n");
    // file.write(contentString.toUtf8());
    // file.close();

    return true;
}

void IniFile::clear()
{
    // for (int i = 0; i < _lines.size(); i++)
    // {
    //     auto line = _lines[i];
    //     if (line)
    //     {
    //         delete line;
    //         _lines[i] = nullptr;
    //     }
    // }

    for (auto &line : _lines)
    {
        if (line)
        {
            delete line;
            line = nullptr;
        }
    }

    _sectionHash.clear();
    _sectionList.clear();

    _lines.clear();
}

StdStringList IniFile::content() const
{
    return _content;
}

void IniFile::setContent(const StdStringList &content)
{
    _content = content;

    parseContent();
}

int IniFile::sectionCount()
{
    return _sectionList.size();
}

std::string IniFile::sectionNameByIndex(const int index) const
{
    if (index < 0 || index >= _sectionList.size())
        return std::string();

    auto iterator = _sectionList.begin();
    std::advance(iterator, index);
    return (*iterator)->name();
}

int IniFile::sectionParameterCountByName(const std::string &sectionName) const
{
    if (!_sectionHash.count(sectionName))
        return -1;

    auto section = _sectionHash.at(sectionName);
    return section->parameterCount();
}

std::string IniFile::sectionParameterNameByIndex(const std::string &sectionName,
                                                 const int index) const
{
    if (!_sectionHash.count(sectionName))
        return std::string();

    const auto section = _sectionHash.at(sectionName);
    if (!section)
        return std::string();

    const auto parameter = section->parameterByIndex(index);
    if (!parameter)
        return std::string();

    return parameter->name();
}

std::string IniFile::value(const std::string &sectionName,
                           const std::string &parameterName,
                           const std::string &defaultValue)
{
    if (!_sectionHash.count(sectionName))
        return defaultValue;

    auto section = _sectionHash[sectionName];
    if (!section)
        return defaultValue;

    auto parameter = section->parameterByName(parameterName);
    if (!parameter)
        return defaultValue;

    auto valueString = parameter->value();
    return valueString;
}

StdStringList IniFile::comments(const std::string &sectionName,
                                const std::string &parameterName)
{
    StdStringList result;

    if (!_sectionHash.count(sectionName))
        return result;

    auto section = _sectionHash[sectionName];
    if (!section)
        return result;

    CommentList commentList;
    if (parameterName.empty())
    {
        commentList = section->commentList();
    }
    else
    {
        auto parameter = section->parameterByName(parameterName);
        if (!parameter)
            return result;

        commentList = parameter->commentList();
    }

    for (const auto &item: commentList)
        result.push_back(item->comment());

    return result;
}

void IniFile::setValue(const std::string &sectionName,
                       const std::string &parameterName,
                       const std::string &value)
{
    if (!_sectionHash.count(sectionName))
        return;

    auto section = _sectionHash[sectionName];
    if (!section)
        return;

    auto parameter = section->parameterByName(parameterName);
    if (!parameter)
        return;

    parameter->setValue(value);
}



void IniFile::applyChangesToContent()
{
    _content.clear();
    // for (int i = 0; i < _lines.size(); i++)
    for (auto &line : _lines)
    {
        auto lineString = line->toString();
        _content.push_back(lineString);
    }
}

void IniFile::parseContent()
{
    clear();

    static const std::regex regExpComment("\\s*\\;.*");
    static const std::regex regExpSection("^\\[[A-z](\\w|\\.|\\d)*\\]\\s*$");
    static const std::regex regExpParameter("^[A-z](\\w|\\.|\\d)*\\=.*$");

    IniLineSection *currentSection = nullptr;

    std::list<IniLineComment*> commentList;

    // for (int i = 0; i < _content.size(); i++)
    for (const auto &contentLine : _content)
    {
        IniLineBase *line = nullptr;

        // const auto &contentLine = _content.at(i);

        std::cmatch m;
        // if (std::regex_match(line.c_str(), m, reSectionValue))
        // if (regExpComment.exactMatch(contentLine))
        if (std::regex_match(contentLine.c_str(), m, regExpComment))
        {
            auto comment = new IniLineComment();
            line = comment;
            line->parse(contentLine);

            commentList.push_back(comment);
        }
        // else if (regExpSection.exactMatch(contentLine))
        else if (std::regex_match(contentLine.c_str(), m, regExpSection))
        {
            currentSection = new IniLineSection(commentList);

            line = currentSection;
            line->parse(contentLine);

            _sectionHash[currentSection->name()] = currentSection;
            _sectionList.push_back(currentSection);
        }
        // else if (regExpParameter.exactMatch(contentLine))
        else if (std::regex_match(contentLine.c_str(), m, regExpParameter))
        {
            auto currenParameter = new IniLineParameter(commentList);

            line = currenParameter;
            line->parse(contentLine);

            if (currentSection)
                currentSection->append(currenParameter);
        }
        else
        {
            line = new IniLineBase({});
            line->parse(contentLine);
        }

        if (line->lineType() != LineType::Comment)
            commentList.clear();

        _lines.push_back(line);
    }
}
