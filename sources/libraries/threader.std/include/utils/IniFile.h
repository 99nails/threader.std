#pragma once

#include "StdStringUtils.h"

#include <map>
#include <memory>
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
using IniLineCommentPtr = std::shared_ptr<IniLineComment>;
using CommentList = std::list<IniLineCommentPtr>;

class IniLineSection;
using IniLineSectionPtr = std::shared_ptr<IniLineSection>;


class IniLineBase
{
public:
    using Ptr = std::shared_ptr<IniLineBase>;

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

    IniLineSection *section() const;
    virtual void setSection(IniLineSection *section);

private:
    std::string _content;
    LineType _lineType;
    CommentList _commentList;
    IniLineSection *_section;
};

using IniLineList = std::list<IniLineBase::Ptr>;

using IniLineVector = std::vector<IniLineBase::Ptr>;


class IniLineComment: public IniLineBase
{
public:
    using Ptr = std::shared_ptr<IniLineComment>;

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
    using Ptr = std::shared_ptr<IniLineParameter>;

public:
    explicit IniLineParameter(const CommentList &commentList,
                              const IniLineSectionPtr &section);

    bool parse(const std::string &line = std::string()) override;

    std::string name() const;

    std::string value() const;

    void setValue(const std::string &value);

    std::string toString() override;

    void setSection(IniLineSection *section) override;
private:
    std::string _name;
    std::string _value;
};

using IniLineParameterHash = std::map<std::string, IniLineParameter::Ptr>;

using IniLineParameterList = std::list<IniLineParameter::Ptr>;

class IniLineSection: public IniLineBase
{
public:
    using Ptr = std::shared_ptr<IniLineSection>;

public:
    explicit IniLineSection(const CommentList &commentList);

    void clear();

    bool parse(const std::string &line = std::string()) override;

    std::string name() const;

    std::string toString() override;

    void append(IniLineParameter::Ptr parameter);

    IniLineParameter::Ptr parameterByName(const std::string &parameterName);

    int parameterCount() const;

    IniLineParameter::Ptr parameterByIndex(const int index) const;

    void parameterDelete(const std::string &parameterName);

private:
    std::string _name;

    IniLineParameterHash _parameterHash;
    IniLineParameterList _parameterList;
};

using IniLineSectionHash = std::map<std::string, IniLineSection::Ptr>;

using IniLineSectionList = std::list<IniLineSection::Ptr>;

class IniFile
{
public:
    using Ptr = std::shared_ptr<IniFile>;

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

    bool setValue(const std::string &sectionName,
                  const std::string &parameterName,
                  const std::string &value);

    const IniLineVector lines();

    void sectionParameterDelete(const std::string &sectionName,
                                const std::string &parameterName);

    void sectionDelete(const std::string &sectionName);

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
