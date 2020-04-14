#ifndef DIALOGUEFIGURE_H
#define DIALOGUEFIGURE_H

#include <QObject>
#include <QPixmap>
#include <QStringList>
#include "dialoguechattype.h"

class DialogueFigure : public QObject
{
    Q_OBJECT
public:
    DialogueFigure(QObject *parent = nullptr);

signals:

public slots:

public:
    bool is_self = false; // 是否在右边
    QString figure_id;
    ChatType type;
    QString nickname;
    QString said;
    QPixmap avatar;
    QString qss; // 各种样式表
};

#endif // DIALOGUEFIGURE_H
