/**
  * 剧本 对话编辑器
  * 带 widgets、actions
  */

#ifndef DIALOGUEGROUP_H
#define DIALOGUEGROUP_H

#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QClipboard>
#include "dialoguebucket.h"
#include "dialogueeditor.h"
#include "dialoguemanager.h"

class DialogueGroup : public QWidget
{
    Q_OBJECT
public:
    DialogueGroup(QWidget *parent = nullptr);
    DialogueGroup(QString dir, QWidget *parent = nullptr);

    void initView();
    void initStyle();
    void initData();

    void setDataDirAndLoad(QString dir);
    void fromText(QString full);
    QString toText(QList<DialogueBucket *> buckets, QString indent_blank = "　　", QString indent_line = "\n\n");
    QString toText(QString indent_blank = "　　", QString indent_line = "\n\n");
    void fromJson(QJsonObject);
    QJsonObject toJson();
    QPixmap toPixmap();

    void setAvatarSize(int x);

private:
    void insertBucketAndSetQSS(QListWidgetItem *item, DialogueBucket* bucket, QString qss = "", bool above = false);
    void insertBucketAndSetQSS(int row, DialogueBucket* bucket, QString qss = "", bool above = false);
    QListWidgetItem *addChat(DialogueBucket* bucket, int row = -1);

protected:
    void resizeEvent(QResizeEvent* event);

signals:

public slots:
    void slotAddLeftChat();
    void slotAddNarrator();
    void slotAddRightChat();
    void slotDialogueMenuShowed(QPoint pos);
    void slotFigureMenuShowed(QPoint pos);
    void slotSaveFigure(DialogueBucket* bucket);
    void slotInsertFromFigure(DialogueFigure* figure);
    void refreshFigures();

    void actionInsertLeftChat();
    void actionInsertNarrator();
    void actionInsertRightChat();
    void actionCopyChat();
    void actionPasteChat();
    void actionChatMoveUp();
    void actionChatMoveDown();
    void actionChatDelete();

    void actionInsertFigureDialogue();
    void actionSelectFigureDialogue();
    void actionUpdateFigureDialogues();
    void actionUpdateSelectedDialogues();
    void actionFigureMoveUp();
    void actionFigureMoveDown();
    void actionFigureDelete();

    void slotSaveToFile();
    void slotLoadFromFile();

private:
    QListWidget *dialogues_list_widget, *figure_list_widget;
    QPushButton *left_button, *mid_button, *right_button;
    DialogueEditor *editor;

    DialogueManager* manager;
    QList<DialogueBucket*> buckets;
};

#endif // DIALOGUEGROUP_H
