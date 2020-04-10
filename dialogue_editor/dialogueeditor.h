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
#include <QVBoxLayout>
#include <QPushButton>
#include "dialoguebucket.h"

class DialogueEditor : public QWidget
{
    Q_OBJECT
public:
    DialogueEditor(QWidget *parent = nullptr);

    void initView();
    void initStyle();
    void initData();

signals:

public slots:
    void slotAddLeftChat();
    void slotAddNarrator();
    void slotAddRightChat();

private:
    void addChat(DialogueBucket* bucket, int row = -1);

private:
    QListWidget *dialogues_list_widget;
    QPushButton *left_button, *mid_button, *right_button;

    QList<DialogueBucket*> buckets;
};

#endif // DIALOGUEDITOR_H
