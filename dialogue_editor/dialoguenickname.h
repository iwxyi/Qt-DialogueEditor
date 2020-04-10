/**
  * 剧本 人名控件
  */

#ifndef DIALOGUENICKNAME_H
#define DIALOGUENICKNAME_H

#include <QObject>
#include <QLabel>

class DialogueNickname : public QLabel
{
public:
    DialogueNickname(QWidget *parent = nullptr);
    DialogueNickname(QString name, QWidget *parent = nullptr);
};

#endif // DIALOGUENICKNAME_H
