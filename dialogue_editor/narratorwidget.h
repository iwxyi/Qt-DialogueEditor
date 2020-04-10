/**
  * 剧本 旁白控件
  */

#ifndef NARRATORWIDGET_H
#define NARRATORWIDGET_H

#include <QObject>
#include <QWidget>

class NarratorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NarratorWidget(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // NARRATORWIDGET_H
