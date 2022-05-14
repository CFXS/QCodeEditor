#pragma once

// QCodeEditor
#include <QHighlightRule>
#include <QStyleSyntaxHighlighter> // Required for inheritance

// Qt
#include <QRegularExpression>
#include <QVector>

class QSyntaxStyle;

/**
 * @brief Class, that describes C++ code
 * highlighter.
 */
class QCMakeHighlighter : public QStyleSyntaxHighlighter {
    //Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param document Pointer to document.
     */
    explicit QCMakeHighlighter(QTextDocument* document = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<QHighlightRule> m_HighlightRules;

    const QString m_IncludeRegex  = QStringLiteral(R"(^\s*#\s*include\s*([<"][^:?"<>\|]+[">]))");
    const QString m_FunctionRegex = QStringLiteral(
        R"(\b([_a-zA-Z][_a-zA-Z0-9]*\s+)?((?:[_a-zA-Z][_a-zA-Z0-9]*\s*::\s*)*([_a-zA-Z][_a-zA-Z0-9]*(?:<(.*?)>)?))(?=\s*\())");
    const QString m_DefTypeRegex = QStringLiteral(R"(\b([_a-zA-Z][_a-zA-Z0-9]*)\s+[_a-zA-Z][_a-zA-Z0-9]*\s*[;=])");
    const QString m_ClassRegex   = QStringLiteral(R"(\b([_a-zA-Z][_a-zA-Z0-9]*\s+)(?:class |struct )?([_a-zA-Z][_a-zA-Z0-9]*)(?=\s*{))");
    const QString m_CallDefRegex = QStringLiteral(R"(\b(?:\((.*)\)))");

    QRegularExpression m_IncludePattern  = QRegularExpression(m_IncludeRegex);
    QRegularExpression m_FunctionPattern = QRegularExpression(m_FunctionRegex);
    QRegularExpression m_DefTypePattern  = QRegularExpression(m_DefTypeRegex);
    QRegularExpression m_ClassPattern    = QRegularExpression(m_ClassRegex);
    QRegularExpression m_CallDefPattern  = QRegularExpression(m_CallDefRegex);
};