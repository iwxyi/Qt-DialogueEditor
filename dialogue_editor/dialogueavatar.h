/**
  * 剧本 （圆形）头像控件
  */

#ifndef DIALOGUEAVATAR_H
#define DIALOGUEAVATAR_H

#include <QObject>
#include <QLabel>

class DialogueAvatar : public QLabel
{
public:
    DialogueAvatar(QWidget* parent = nullptr);
    DialogueAvatar(QPixmap pixmap, QWidget* parent = nullptr);
};

#endif // DIALOGUEAVATAR_H
