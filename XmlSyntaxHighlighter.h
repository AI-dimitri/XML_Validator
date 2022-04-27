#pragma once

#include <QtCore/QRegExp>
#include <QtGui/QSyntaxHighlighter>

class XmlSyntaxHighlighter : public QSyntaxHighlighter
{
    public:
        explicit XmlSyntaxHighlighter(QTextDocument *parent = nullptr);

    protected:
        virtual void highlightBlock(const QString &text);

    private:
        struct HighlightingRule
        {
            QRegExp pattern;
            QTextCharFormat format;
        };
        QVector<HighlightingRule> highlightingRules;

        QRegExp commentStartExpression;
        QRegExp commentEndExpression;

        QTextCharFormat tagFormat;
        QTextCharFormat attributeFormat;
        QTextCharFormat attributeContentFormat;
        QTextCharFormat commentFormat;
};
