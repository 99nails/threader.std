#include "IniFile.h"

#include "FileUtils.h"

#include <regex>
#include <iostream>
#include <fstream>


IniLineBase::IniLineBase(const CommentList &commentList,
                         const LineType lineType)
    : _content(std::string())
    , _lineType(lineType)
    , _commentList(commentList)
    , _section(nullptr)
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

IniLineSection *IniLineBase::section() const
{
    return _section;
}

void IniLineBase::setSection(IniLineSection *section)
{
    _section = section;
}

/***************************************************************************************************
* IniLineComment
***************************************************************************************************/

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

/***************************************************************************************************
* IniLineParameter
***************************************************************************************************/

IniLineParameter::IniLineParameter(const CommentList &commentList,
                                   const IniLineSectionPtr &section)
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

void IniLineParameter::setSection(IniLineSection *section)
{
    for (const auto &comment : commentList())
        comment->setSection(section);
    IniLineBase::setSection(section);
}

/***************************************************************************************************
* IniLineSection
***************************************************************************************************/

IniLineSection::IniLineSection(const CommentList &commentList)
    : IniLineBase(commentList,
                  LineType::Section)
    , _name(std::string())
    , _parameterHash({})
    , _parameterList({})
{
    for (const auto &comment : commentList)
        comment->setSection(this);
    setSection(this);
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

void IniLineSection::append(IniLineParameter::Ptr parameter)
{
    parameter->setSection(this);
    _parameterHash[parameter->name()] = parameter;
    _parameterList.push_back(parameter);
}

IniLineParameter::Ptr IniLineSection::parameterByName(const std::string &parameterName)
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

IniLineParameter::Ptr IniLineSection::parameterByIndex(const int index) const
{
    if (index < 0 || index >= _parameterList.size())
        return nullptr;

    auto iterator = _parameterList.begin();
    std::advance(iterator, index);
    return *iterator;
}

void IniLineSection::parameterDelete(const std::string &parameterName)
{
    auto parameter = parameterByName(parameterName);
    if (!parameter)
        return;

    _parameterHash.erase(parameterName);
    _parameterList.remove(parameter);
}

/***************************************************************************************************
* IniFile
***************************************************************************************************/

IniFile::IniFile()
    : _fileName{std::string()}
    , _content{}
    , _lines{}
    , _sectionHash{}
    , _sectionList{}
{
}

IniFile::~IniFile()
{
    clear();
}

const std::string &IniFile::fileName() const
{
    return _fileName;
}

bool IniFile::fileOpen(const std::string &fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in);
    if (!file.is_open())
        return false;

    _fileName = fileName;

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

bool IniFile::fileSave(const std::string &fileName)
{
    applyChangesToContent();

    if (!fileName.empty())
        _fileName = fileName;

    std::ofstream file(_fileName);

    if (!file.is_open())
        return false;

    for (const auto &line : _content)
        file << line << std::endl;

    file.close();
    return true;
}

void IniFile::clear()
{
    _sectionHash.clear();
    _sectionList.clear();
    _lines.clear();
}

StdStringList IniFile::content()
{
    applyChangesToContent();
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

bool IniFile::setValue(const std::string &sectionName,
                       const std::string &parameterName,
                       const std::string &value)
{
    if (parameterName.empty())
        return false;

    auto section = _sectionHash[sectionName];
    if (!section)
    {
        section = std::make_shared<IniLineSection>(CommentList());
        section->parse("[" + sectionName + "]");
        _sectionHash[sectionName] = section;
        _sectionList.push_back(section);
        _lines.push_back(section);
    }

    auto parameter = section->parameterByName(parameterName);
    if (!parameter)
    {
        parameter = std::make_shared<IniLineParameter>(CommentList(), section);
        parameter->parse(parameterName + "=");
        section->append(parameter);

        bool sectionFound = false;
        auto position = _lines.end();
        std::string currentSectionName;

        for (auto it = _lines.begin(); it != _lines.end(); it++)
        {
            auto line = *it;
            currentSectionName = (line->section()) ? line->section()->name() : std::string();

            if (!sectionFound)
            {
                if (currentSectionName == section->name())
                    sectionFound = true;
            }
            else
            {
                if (currentSectionName != section->name())
                {
                    position = it;
                    break;
                }
            }
        }

        if (position == _lines.end())
        {
            if (sectionFound)
                _lines.push_back(parameter);
            else
                return false;
        }
        else
            _lines.insert(position, parameter);
    }

    parameter->setValue(value);

    return true;
}

const IniLineVector IniFile::lines()
{
    IniLineVector result(_lines.size());
    std::copy(_lines.begin(), _lines.end(), result.begin());
    return result;
}

void IniFile::sectionParameterDelete(const std::string &sectionName,
                                     const std::string &parameterName)
{
    auto section = _sectionHash[sectionName];
    if (!section)
        return;

    auto parameter = section->parameterByName(parameterName);
    if (!parameter)
        return;

    for (const auto &comment : parameter->commentList())
        _lines.remove(comment);

    _lines.remove(parameter);
    section->parameterDelete(parameterName);
}

void IniFile::sectionDelete(const std::string &sectionName)
{
    if (sectionName.empty())
        return;

    auto section = _sectionHash[sectionName];
    if (!section)
        return;

    auto it = _lines.begin();
    while (it != _lines.end())
    {
        auto line = *it;
        const auto currentSectionName = (line->section()) ? line->section()->name() : "";
        if (currentSectionName == sectionName)
        {
            it = _lines.erase(it);
        }
        else
            it++;

    }
    _sectionHash.erase(sectionName);
    _sectionList.remove(section);
}

void IniFile::applyChangesToContent()
{
    _content.clear();
    for (const auto &line : _lines)
    {
        auto lineString = line->toString();
        _content.push_back(lineString);
    }
}

void IniFile::parseContent()
{
    clear();

    static const std::regex regExpComment("\\s*(\\;|\\#).*");
    static const std::regex regExpSection("^\\[[A-z\\d](\\w|\\.|\\d)*\\]\\s*$");
    static const std::regex regExpParameter("^[A-z\\d](\\w|\\.|\\d)*\\=.*$");

    IniLineSection::Ptr currentSection = nullptr;

    std::list<IniLineComment::Ptr> commentList;

    // for (int i = 0; i < _content.size(); i++)
    for (const auto &contentLine : _content)
    {
        IniLineBase::Ptr line = nullptr;

        std::cmatch match;
        if (std::regex_match(contentLine.c_str(), match, regExpComment))
        {
            auto comment = std::make_shared<IniLineComment>();
            line = comment;
            line->parse(contentLine);

            commentList.push_back(comment);
        }
        else if (std::regex_match(contentLine.c_str(), match, regExpSection))
        {
            currentSection = std::make_shared<IniLineSection>(commentList);

            line = currentSection;
            line->parse(contentLine);

            _sectionHash[currentSection->name()] = currentSection;
            _sectionList.push_back(currentSection);
        }
        else if (std::regex_match(contentLine.c_str(), match, regExpParameter))
        {
            if (!currentSection)
            {
                currentSection = std::make_shared<IniLineSection>(std::list<IniLineComment::Ptr>());
                _sectionHash[currentSection->name()] = currentSection;
                _sectionList.push_back(currentSection);
            }

            auto currenParameter = std::make_shared<IniLineParameter>(commentList,
                                                                      currentSection);

            line = currenParameter;
            line->parse(contentLine);

            if (currentSection)
                currentSection->append(currenParameter);
        }
        else
        {
            line = std::make_shared<IniLineBase>(std::list<IniLineComment::Ptr>());
            line->parse(contentLine);
        }

        if (line->lineType() != LineType::Comment)
            commentList.clear();

        _lines.push_back(line);
    }
}
