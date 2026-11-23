#include <gtest/gtest.h>
#include "IniFile.h"

using namespace Threader::Utils;

TEST(IniLineParameter, ParsesNameAndValue)
{
    IniLineParameter::Ptr param = std::make_shared<IniLineParameter>(CommentList(), nullptr);
    IniLineSection::Ptr section = std::make_shared<IniLineSection>(CommentList());
    param->setSection(section.get());

    EXPECT_TRUE(param->parse("port=1234"));
    EXPECT_EQ(param->name(), "port");
    EXPECT_EQ(param->value(), "1234");
    EXPECT_EQ(param->toString(), "port=1234");
}

TEST(IniLineParameter, LineWithoutEqualsFails)
{
    IniLineParameter::Ptr param = std::make_shared<IniLineParameter>(CommentList(), nullptr);
    IniLineSection::Ptr section = std::make_shared<IniLineSection>(CommentList());
    param->setSection(section.get());

    EXPECT_FALSE(param->parse("just-a-string"));
}

TEST(IniLineComment, ParsesCommentAndRoundTrip)
{
    IniLineComment comment;
    EXPECT_TRUE(comment.parse("; this is a comment"));
    EXPECT_EQ(comment.comment(), " this is a comment");
    EXPECT_EQ(comment.toString(), "; this is a comment");
}

TEST(IniFile, ParsesContentWithSectionAndParameter)
{
    IniFile ini;
    StdStringList content;
    content.push_back("[section1]");
    content.push_back("key=value");
    ini.setContent(content);

    EXPECT_EQ(ini.sectionCount(), 1);
    EXPECT_EQ(ini.sectionParameterCountByName("section1"), 1);
    EXPECT_EQ(ini.sectionParameterNameByIndex("section1", 0), "key");
}

// BUG-5: sectionParameterDelete() читает _sectionHash через operator[]
// до проверки существования секции — для несуществующего имени в хэш
// вставляется нулевая запись. После этого count("имя") == 1, и
// sectionParameterCountByName() разыменует нулевой указатель: сегфолт.
// Фикс: заменить operator[] на find().
TEST(DISABLED_IniFileHash, DeleteUnknownSectionPoisonsLookup)
{
    IniFile ini;
    ASSERT_TRUE(ini.setValue("real", "k", "v"));

    // noop-удаление по несуществующей секции не должно менять состояние
    ini.sectionParameterDelete("ghost", "k");

    EXPECT_EQ(ini.sectionCount(), 1);
    EXPECT_EQ(ini.value("ghost", "k", "fallback"), "fallback");

    // До исправления: null-запись в хэше, count("ghost") == 1,
    // sectionParameterCountByName разыменует nullptr — сегфолт.
    // После исправления: должен вернуть -1 (секции нет).
    EXPECT_EQ(ini.sectionParameterCountByName("ghost"), -1);
}

// Тот же механизм через пустое имя: sectionDelete() пустое имя отсекает,
// а sectionParameterDelete() — нет.
TEST(DISABLED_IniFileHash, DeleteWithEmptySectionNamePoisonsEmptyKey)
{
    IniFile ini;
    ASSERT_TRUE(ini.setValue("real", "k", "v"));

    ini.sectionParameterDelete("", "k");

    EXPECT_EQ(ini.sectionCount(), 1);
    // до исправления — сегфолт, после — -1
    EXPECT_EQ(ini.sectionParameterCountByName(""), -1);
}

// Регрессионная защита для comments(): этот метод проверки проходит корректно
// и загрязнения не делает. Не DISABLED — должен проходить уже сейчас.
TEST(IniFile, CommentsUnknownSectionDoesNotPolluteHash)
{
    IniFile ini;
    ASSERT_TRUE(ini.setValue("real", "k", "v"));

    EXPECT_TRUE(ini.comments("ghost", "").empty());
    // если это вдруг сломается — значит, кто-то убрал проверку count()
    EXPECT_EQ(ini.sectionParameterCountByName("ghost"), -1);
}


/**************************************************************************************************/

namespace
{

IniFile makeIni(const StdStringList &lines)
{
    IniFile ini;
    ini.setContent(lines);
    return ini;
}

} // namespace

// ---------- Добавление ----------

TEST(IniFile, SetValueCreatesSectionAndParameter)
{
    IniFile ini;
    EXPECT_TRUE(ini.setValue("network", "port", "1234"));

    EXPECT_EQ(ini.sectionCount(), 1);
    EXPECT_EQ(ini.sectionNameByIndex(0), "network");
    EXPECT_EQ(ini.sectionParameterCountByName("network"), 1);
    EXPECT_EQ(ini.sectionParameterNameByIndex("network", 0), "port");
    EXPECT_EQ(ini.value("network", "port", ""), "1234");
}

TEST(IniFile, SetValueOnExistingSectionAddsParameter)
{
    IniFile ini;
    ASSERT_TRUE(ini.setValue("network", "port", "1234"));
    ASSERT_TRUE(ini.setValue("network", "host", "localhost"));

    EXPECT_EQ(ini.sectionCount(), 1);
    EXPECT_EQ(ini.sectionParameterCountByName("network"), 2);
    EXPECT_EQ(ini.value("network", "host", ""), "localhost");
}

TEST(IniFile, SetValueUpdatesExistingParameterWithoutDuplicate)
{
    IniFile ini;
    ASSERT_TRUE(ini.setValue("network", "port", "1234"));
    ASSERT_TRUE(ini.setValue("network", "port", "4321"));

    EXPECT_EQ(ini.sectionParameterCountByName("network"), 1);
    EXPECT_EQ(ini.value("network", "port", ""), "4321");
}

TEST(IniFile, SetValueWithEmptyParameterNameFails)
{
    IniFile ini;
    EXPECT_FALSE(ini.setValue("network", "", "1234"));
    EXPECT_EQ(ini.sectionCount(), 0);
}

TEST(IniFile, ValueReturnsDefaultForMissingSectionOrParameter)
{
    IniFile ini;
    ASSERT_TRUE(ini.setValue("network", "port", "1234"));

    EXPECT_EQ(ini.value("missing", "port", "fallback"), "fallback");
    EXPECT_EQ(ini.value("network", "missing", "fallback"), "fallback");
}

TEST(IniFile, SetValueInsertsParameterBeforeNextSection)
{
    auto ini = makeIni({
        "[s1]",
        "k1=v1",
        "[s2]",
        "k2=v2",
    });

    // новый параметр s1 должен попасть в секцию s1, а не в конец файла
    ASSERT_TRUE(ini.setValue("s1", "k1b", "vb"));

    const auto c = ini.content();
    const std::vector<std::string> content(c.begin(), c.end());

    ASSERT_EQ(content.size(), 5u);
    EXPECT_EQ(content[0], "[s1]");
    EXPECT_EQ(content[1], "k1=v1");
    EXPECT_EQ(content[2], "k1b=vb");
    EXPECT_EQ(content[3], "[s2]");
    EXPECT_EQ(content[4], "k2=v2");
}

// ---------- Удаление ----------

TEST(IniFile, SectionDeleteRemovesSectionWithAllParameters)
{
    auto ini = makeIni({
        "[s1]",
        "k1=v1",
        "k2=v2",
        "[s2]",
        "k3=v3",
    });

    ini.sectionDelete("s1");

    EXPECT_EQ(ini.sectionCount(), 1);
    EXPECT_EQ(ini.sectionNameByIndex(0), "s2");
    EXPECT_EQ(ini.sectionParameterCountByName("s1"), -1); // секции больше нет
    EXPECT_EQ(ini.value("s2", "k3", ""), "v3");

    const auto c = ini.content();
    const std::vector<std::string> content(c.begin(), c.end());
    ASSERT_EQ(content.size(), 2u);
    EXPECT_EQ(content[0], "[s2]");
    EXPECT_EQ(content[1], "k3=v3");
}

TEST(IniFile, SectionDeleteUnknownSectionIsNoop)
{
    auto ini = makeIni({"[s1]", "k1=v1"});

    ini.sectionDelete("missing");
    ini.sectionDelete(""); // пустое имя — тоже noop

    EXPECT_EQ(ini.sectionCount(), 1);
    EXPECT_EQ(ini.value("s1", "k1", ""), "v1");
}

TEST(IniFile, SectionParameterDeleteRemovesOnlyParameter)
{
    auto ini = makeIni({
        "[s1]",
        "k1=v1",
        "k2=v2",
        "[s2]",
        "k3=v3",
    });

    ini.sectionParameterDelete("s1", "k2");

    EXPECT_EQ(ini.sectionParameterCountByName("s1"), 1);
    EXPECT_EQ(ini.sectionParameterNameByIndex("s1", 0), "k1");
    EXPECT_EQ(ini.value("s2", "k3", ""), "v3");

    const auto c = ini.content();
    const std::vector<std::string> content(c.begin(), c.end());

    ASSERT_EQ(content.size(), 4u);
    EXPECT_EQ(content[1], "k1=v1");
}

TEST(IniFile, SectionParameterDeleteUnknownIsNoop)
{
    auto ini = makeIni({"[s1]", "k1=v1"});

    ini.sectionParameterDelete("s1", "missing");
    ini.sectionParameterDelete("missing", "k1");

    EXPECT_EQ(ini.sectionParameterCountByName("s1"), 1);
}

TEST(IniFile, RepeatedAddDeleteRestoreCycle)
{
    IniFile ini;
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_TRUE(ini.setValue("s", "k", std::to_string(i)));
        EXPECT_EQ(ini.value("s", "k", ""), std::to_string(i));
        ini.sectionParameterDelete("s", "k");
        EXPECT_EQ(ini.sectionParameterCountByName("s"), 0);
    }
    // секция осталась, параметров нет — после удаления можно добавлять снова
    ASSERT_TRUE(ini.setValue("s", "k", "final"));
    EXPECT_EQ(ini.value("s", "k", ""), "final");
}

// ---------- Комментарии и разбор ----------

TEST(IniFile, CommentsAttachedToSectionAndParameter)
{
    auto ini = makeIni({
        "; section comment",
        "[s1]",
        "; parameter comment",
        "k1=v1",
    });

    const auto sectionComments = ini.comments("s1", "");
    ASSERT_EQ(sectionComments.size(), 1u);
    EXPECT_EQ(sectionComments.front(), " section comment");

    const auto parameterComments = ini.comments("s1", "k1");
    ASSERT_EQ(parameterComments.size(), 1u);
    EXPECT_EQ(parameterComments.front(), " parameter comment");
}

TEST(IniFile, InvalidLinesDoNotCreateSections)
{
    auto ini = makeIni({
        "not a section",
        "=novalue",   // пустое имя параметра не проходит регексп
        "===",
    });

    EXPECT_EQ(ini.sectionCount(), 0);
}

TEST(IniFile, RoundtripThroughFileSaveAndOpen)
{
    const std::string path = "/tmp/threader_ini_test_" +
                             std::to_string(::getpid()) + ".ini";

    {
        IniFile ini;
        ASSERT_TRUE(ini.setValue("network", "port", "1234"));
        ASSERT_TRUE(ini.setValue("network", "host", "localhost"));
        ASSERT_TRUE(ini.setValue("logging", "level", "debug"));

        ASSERT_TRUE(ini.fileSave(path));
    }

    IniFile ini;
    ASSERT_TRUE(ini.fileOpen(path));
    EXPECT_EQ(ini.sectionCount(), 2);
    EXPECT_EQ(ini.value("network", "port", ""), "1234");
    EXPECT_EQ(ini.value("network", "host", ""), "localhost");
    EXPECT_EQ(ini.value("logging", "level", ""), "debug");

    ::remove(path.c_str());
}
