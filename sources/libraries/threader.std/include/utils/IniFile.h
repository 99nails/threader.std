#pragma once

#include "StdStringUtils.h"

#include <map>
#include <list>
#include <string>

using namespace Threader::Utils;

enum LineType {
    UnknownLineType,
    Comment,
    Parameter,
    Section
};

class IniLineComment;

using CommentList = std::list<IniLineComment*>;

class IniLineBase
{
public:
    explicit IniLineBase(const CommentList &commentList,
                         const LineType lineType = LineType::UnknownLineType);
    virtual ~IniLineBase() = default;

    int position() const;

    const std::string &content() const;

    LineType lineType() const;

    virtual bool parse(const std::string &line);

    virtual std::string toString();

    std::string commentString();

    CommentList commentList() const;

private:
    std::string _content;
    LineType _lineType;
    CommentList _commentList;
};

using IniLineList = std::list<IniLineBase*>;


class IniLineComment: public IniLineBase
{
public:
    explicit IniLineComment();

    bool parse(const std::string &line = std::string()) override;

    std::string comment() const;

    std::string toString() override;

private:
    std::string _comment;

};

class IniLineParameter: public IniLineBase
{
public:
    explicit IniLineParameter(const CommentList &commentList);

    bool parse(const std::string &line = std::string()) override;

    std::string name() const;

    std::string value() const;

    void setValue(const std::string &value);

    std::string toString() override;

private:
    std::string _name;
    std::string _value;
};

using IniLineParameterHash = std::map<std::string, IniLineParameter*>;

using IniLineParameterList = std::list<IniLineParameter*>;

class IniLineSection: public IniLineBase
{
public:
    explicit IniLineSection(const CommentList &commentList);

    void clear();

    bool parse(const std::string &line = std::string()) override;

    std::string name() const;

    std::string toString() override;

    void append(IniLineParameter* parameter);

    IniLineParameter* parameterByName(const std::string &parameterName);

    int parameterCount() const;

    IniLineParameter* parameterByIndex(const int index) const;

private:
    std::string _name;

    IniLineParameterHash _parameterHash;
    IniLineParameterList _parameterList;
};

using IniLineSectionHash = std::map<std::string, IniLineSection*>;

using IniLineSectionList = std::list<IniLineSection*>;

class IniFile
{
public:
    explicit IniFile();
    virtual ~IniFile();

    bool fileOpen(const std::string &fileName);

    bool fileSave(const std::string &fileName = std::string());

    void clear();

    const std::string &fileName() const;

    StdStringList content();
    void setContent(const StdStringList &content);

    int sectionCount();

    std::string sectionNameByIndex(const int index) const;

    int sectionParameterCountByName(const std::string &sectionName) const;

    std::string sectionParameterNameByIndex(const std::string &sectionName,
                                            const int index) const;

    template <typename ValueType>
    ValueType value(const std::string &sectionName,
                    const std::string &parameterName,
                    const ValueType &defaultValue)
    {
        auto valueString = value(sectionName, parameterName, std::string());
        if (valueString.empty())
            return defaultValue;

        ValueType result = defaultValue;
        if (vtonum<ValueType>(valueString, result))
            return result;

        return defaultValue;
    }

    std::string value(const std::string &sectionName,
                      const std::string &parameterName,
                      const std::string &defaultValue);

    StdStringList comments(const std::string &sectionName,
                           const std::string &parameterName);

    void setValue(const std::string &sectionName,
                  const std::string &parameterName,
                  const std::string &value);

private:
    void applyChangesToContent();

    void parseContent();

private:
    std::string _fileName;
    StdStringList _content;
    IniLineList _lines;
    IniLineSectionHash _sectionHash;
    IniLineSectionList _sectionList;
};
