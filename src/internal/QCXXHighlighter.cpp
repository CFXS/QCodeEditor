// QCodeEditor
#include <QCXXHighlighter>
#include <QLanguage>
#include <QSyntaxStyle>

// Qt
#include <QFile>

QCXXHighlighter::QCXXHighlighter(QTextDocument* document) : QStyleSyntaxHighlighter(document), m_HighlightRules() {
    Q_INIT_RESOURCE(qcodeeditor_resources);
    QFile fl(":/languages/cpp.xml");

    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    QLanguage language(&fl);

    if (!language.isLoaded()) {
        return;
    }

    // func ptr
    m_HighlightRules.append({QRegularExpression(R"((?:\()(\*.*(?:\)))(?:\(.*\)))"), "Variable"});

    // control content
    //m_HighlightRules.append({QRegularExpression(R"(for\s*?\(.*\))"), "Variable"});
    //m_HighlightRules.append({QRegularExpression(R"(if\s*?\(.*\))"), "Variable"});
    //m_HighlightRules.append({QRegularExpression(R"(while\s*?\(.*\))"), "Variable"});
    //m_HighlightRules.append({QRegularExpression(R"(switch\s*?\(.*\))"), "Variable"});

    // std
    m_HighlightRules.append({QRegularExpression(R"((std::.*?[<\(\s]))"), "Type"});

    // Color reset
    m_HighlightRules.append({QRegularExpression(R"([&;><\+\-\/\%:=\*\[\]\{\},])"), "Text"});

    auto keys = language.keys();
    for (auto&& key : keys) {
        auto names = language.names(key);
        for (auto&& name : names) {
            m_HighlightRules.append({QRegularExpression(QString(R"(\b%1\b)").arg(name)), key});
        }
    }

    // Include
    m_HighlightRules.append({QRegularExpression(R"(#include\s*?[<"].*?[>"])"), "String"});

    // Define
    m_HighlightRules.append({QRegularExpression(R"(#[a-zA-Z_]+)"), "Preprocessor"});

    // Numbers
    m_HighlightRules.append(
        {QRegularExpression(
             R"((?<=\b|\s|^)(?i)(?:(?:(?:(?:(?:\d+(?:'\d+)*)?\.(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)\.(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)?\.(?:[0-9a-f]+(?:'[0-9a-f]+)*)(?:p[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)\.?(?:p[+-]?(?:\d+(?:'\d+)*))))[lf]?)|(?:(?:(?:[1-9]\d*(?:'\d+)*)|(?:0[0-7]*(?:'[0-7]+)*)|(?:0x[0-9a-f]+(?:'[0-9a-f]+)*)|(?:0b[01]+(?:'[01]+)*))(?:u?l{0,2}|l{0,2}u?)))(?=\b|\s|$))"),
         "Number"});

    // Chars
    m_HighlightRules.append({QRegularExpression(R"(('(?:[^'\n].*?)?(?:.)?'))"), "String"});

    // Strings
    m_HighlightRules.append({QRegularExpression(R"(\"(\\.|[^"\\])*\")"), "String"});

    // printf
    m_HighlightRules.append({QRegularExpression(R"((%(?:[\*\-\+\d\x]*(?:\.[\d\x]+)?)?(?:d|f|s|x|l|g|X|llu|lu|u)))"), "Format"});

    // Escapes
    m_HighlightRules.append({QRegularExpression(R"((\\))"), "Escape"});

    // Single line
    m_HighlightRules.append({QRegularExpression(R"(//[^\n]*)"), "Comment"});
}

void QCXXHighlighter::highlightBlock(const QString& text) {
    // Check for func def or call
    {
        auto matchIterator = m_CallDefPattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(1), match.capturedLength(1), syntaxStyle()->getFormat("Variable"));

            int i        = 0;
            QString line = text.mid(match.capturedStart(1), match.capturedLength(1));
            while ((i = line.indexOf(",", i)) != -1) {
                setFormat(match.capturedStart(4) + i, 1, syntaxStyle()->getFormat("Text"));
                ++i;
            }
        }
    }

    // Checking for function
    {
        auto matchIterator = m_FunctionPattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            setFormat(match.capturedStart(0), match.capturedLength(0), syntaxStyle()->getFormat("Type"));
            setFormat(match.capturedStart(3), match.capturedLength(3), syntaxStyle()->getFormat("Function"));
            setFormat(match.capturedStart(4), match.capturedLength(4), syntaxStyle()->getFormat("Type"));
        }
    }
    // Checking for class
    {
        auto matchIterator = m_ClassPattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            setFormat(match.capturedStart(), match.capturedLength(), syntaxStyle()->getFormat("Keyword"));

            setFormat(match.capturedStart(2), match.capturedLength(2), syntaxStyle()->getFormat("Type"));
        }
    }
    {
        auto matchIterator = m_DefTypePattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            setFormat(match.capturedStart(1), match.capturedLength(1), syntaxStyle()->getFormat("Type"));
        }
    }

    for (auto& rule : m_HighlightRules) {
        auto matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            setFormat(match.capturedStart(), match.capturedLength(), syntaxStyle()->getFormat(rule.formatName));
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(m_CommentStartPattern);
    }

    while (startIndex >= 0) {
        auto match = m_CommentEndPattern.match(text, startIndex);

        int endIndex      = match.capturedStart();
        int commentLength = 0;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, syntaxStyle()->getFormat("Comment"));
        startIndex = text.indexOf(m_CommentStartPattern, startIndex + commentLength);
    }
}
