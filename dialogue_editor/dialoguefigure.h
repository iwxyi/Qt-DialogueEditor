#ifndef DIALOGUEFIGURE_H
#define DIALOGUEFIGURE_H

#include <QObject>
#include <QPixmap>
#include <QStringList>
#include <QRegularExpression>
#include "dialoguechattype.h"

class DialogueFigure : public QObject
{
public:
    DialogueFigure(QObject *parent = nullptr);

    bool isNarrator();
    void setNameReg(QString s);
    void setLineReg(QString s);

public:
    bool is_self = false; // 是否在右边
    QString figure_id;
    ChatType type;
    QString nickname;
    QString said;
    QPixmap avatar;
    QString qss; // 各种样式表
    QString line_pattern; // 行匹配（下一行是他说的话）
    QString name_pattern; // 名字匹配（当前行是他说的话）
    QRegularExpression line_reg;
    QRegularExpression name_reg;
};

#endif // DIALOGUEFIGURE_H
