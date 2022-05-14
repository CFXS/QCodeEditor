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
class QLDHighlighter : public QStyleSyntaxHighlighter {
    //Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param document Pointer to document.
     */
    explicit QLDHighlighter(QTextDocument* document = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<QHighlightRule> m_HighlightRules;

    const QString m_CommentStartRegex        = QStringLiteral(R"(/\*)");
    const QString m_CommentEndRegex          = QStringLiteral(R"(\*/)");
    QRegularExpression m_CommentStartPattern = QRegularExpression(m_CommentStartRegex);
    QRegularExpression m_CommentEndPattern   = QRegularExpression(m_CommentEndRegex);
};
