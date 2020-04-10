/**
  * 剧本 对话编辑器
  * 带 widgets、actions
  */

#ifndef DIALOGUEGROUP_H
#define DIALOGUEGROUP_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include "dialoguebucket.h"
#include "dialogueeditor.h"

class DialogueGroup : public QWidget
{
    Q_OBJECT
public:
    DialogueGroup(QWidget *parent = nullptr);

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
    DialogueEditor *editor;

    QList<DialogueBucket*> buckets;
};

#endif // DIALOGUEGROUP_H
