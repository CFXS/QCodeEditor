// QCodeEditor
#include <QLDHighlighter>
#include <QLanguage>
#include <QSyntaxStyle>

// Qt
#include <QFile>

QLDHighlighter::QLDHighlighter(QTextDocument* document) : QStyleSyntaxHighlighter(document), m_HighlightRules() {
    Q_INIT_RESOURCE(qcodeeditor_resources);
    QFile fl(":/languages/ld.xml");

    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    QLanguage language(&fl);

    if (!language.isLoaded()) {
        return;
    }

    auto keys = language.keys();
    for (auto&& key : keys) {
        auto names = language.names(key);
        for (auto&& name : names) {
            m_HighlightRules.append({QRegularExpression(QString(R"(\b%1\b)").arg(name)), key});
        }
    }

    // Numbers
    m_HighlightRules.append(
        {QRegularExpression(
             R"((?<=\b|\s|^)(?i)(?:(?:(?:(?:(?:\d+(?:'\d+)*)?\.(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)\.(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)?\.(?:[0-9a-f]+(?:'[0-9a-f]+)*)(?:p[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)\.?(?:p[+-]?(?:\d+(?:'\d+)*))))[lf]?)|(?:(?:(?:[1-9]\d*(?:'\d+)*)|(?:0[0-7]*(?:'[0-7]+)*)|(?:0x[0-9a-f]+(?:'[0-9a-f]+)*)|(?:0b[01]+(?:'[01]+)*))(?:u?l{0,2}|l{0,2}u?)))(?=\b|\s|$))"),
         "Number"});

    // Escapes
    m_HighlightRules.append({QRegularExpression(R"((\\))"), "Escape"});

    // Single line
    m_HighlightRules.append({QRegularExpression(R"(//[^\n]*)"), "Comment"});

    // rwx
    m_HighlightRules.append({QRegularExpression(R"((?i)\(((?:r|w|x)*)\))"), "Modifier"});

    // noload
    m_HighlightRules.append({QRegularExpression(R"((?i)\((NOLOAD\)))"), "Preprocessor"});

    // attribute
    m_HighlightRules.append({QRegularExpression(R"((?i)\((ATTRIBUTE\)))"), "Preprocessor"});

    // Placement target
    m_HighlightRules.append({QRegularExpression(R"(>\s*([a-zA-Z\d_]*))"), "Access"});

    // Operators
    m_HighlightRules.append({QRegularExpression(R"((>))"), "Keyword"});
    m_HighlightRules.append({QRegularExpression(R"(([:+=\-]))"), "Variable"});

    // dot
    m_HighlightRules.append({QRegularExpression(R"(([.]))"), "Variable"});

    // *
    m_HighlightRules.append({QRegularExpression(R"(([*]))"), "Escape"});
}

void QLDHighlighter::highlightBlock(const QString& text) {
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
