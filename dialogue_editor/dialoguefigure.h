#ifndef DIALOGUEFIGURE_H
#define DIALOGUEFIGURE_H

#include <QObject>
#include <QPixmap>
#include <QStringList>

class DialogueFigure : public QObject
{
    Q_OBJECT
public:
    explicit DialogueFigure(QObject *parent = nullptr);

signals:

public slots:

private:
    bool is_self = false; // 是否在右边
    QString nickname;
    QPixmap avatar;
    QString avatar_qss, chat_qss; // 样式表
};

#endif // DIALOGUEFIGURE_H
