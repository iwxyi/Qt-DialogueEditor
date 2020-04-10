/**
  * 剧本 旁白控件
  */

#ifndef NARRATORWIDGET_H
#define NARRATORWIDGET_H

#include <QObject>
#include <QLabel>

class DialogueNarrator : public QLabel
{
    Q_OBJECT
public:
    DialogueNarrator(QWidget *parent = nullptr);
    DialogueNarrator(QString narr, QWidget* parent = nullptr);

signals:

public slots:

private:

};

#endif // NARRATORWIDGET_H
