#ifndef QSSHIGHLIGHTEDITOR_H
#define QSSHIGHLIGHTEDITOR_H

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class QSSHighlightEditor : public QPlainTextEdit
{
public:
    QSSHighlightEditor(QWidget* parent = nullptr);
};

class QSSHighlighter : public QSyntaxHighlighter
{
    struct QSSRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

public:
    QSSHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;
};

#endif // QSSHIGHLIGHTEDITOR_H
