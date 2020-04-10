/**
  * 剧本 人名控件
  */

#ifndef DIALOGUEFIGURE_H
#define DIALOGUEFIGURE_H

#include <QObject>
#include <QLabel>

class DialogueFigure : public QLabel
{
public:
    DialogueFigure(QWidget *parent = nullptr);
    DialogueFigure(QString name, QWidget *parent = nullptr);
};

#endif // DIALOGUEFIGURE_H
