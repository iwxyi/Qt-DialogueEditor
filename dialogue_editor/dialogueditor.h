/**
  * 剧本 对话编辑器
  * 带 widgets、actions
  */

#ifndef DIALOGUEDITOR_H
#define DIALOGUEDITOR_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>

class DialoguEditor : public QWidget
{
    Q_OBJECT
public:
    explicit DialoguEditor(QWidget *parent = nullptr);

signals:

public slots:

private:
    QListWidget* dialogues_widget;

};

#endif // DIALOGUEDITOR_H
