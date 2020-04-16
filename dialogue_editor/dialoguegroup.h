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
#include <QInputDialog>
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
    void insertBucketAndSetQSS(QListWidgetItem *item, DialogueBucket* bucket, QString qss = "", bool next = false);
    void insertBucketAndSetQSS(int row, DialogueBucket* bucket, QString qss = "", bool next = false);
    QListWidgetItem *addChat(DialogueBucket* bucket, int row = -1);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:

public slots:
    void slotAddLeftChat();  // 最下面添加左边对话 ctrl+alt+q
    void slotAddNarrator();  // 最下面添加中间对话 ctrl+alt+w
    void slotAddRightChat(); // 最下面添加右边对话 ctrl+alt+e
    void slotDialogueMenuShowed(QPoint pos);
    void slotFigureMenuShowed(QPoint pos);
    void slotSaveFigure(DialogueBucket* bucket);
    void slotInsertFromFigure(DialogueFigure* figure);
    void refreshFigures();

    // ==== 对话菜单 ====
    void actionInsertLeftChat(bool next = false);  // 插入左边对话 ctrl[+shift]+q
    void actionInsertNarrator(bool next = false);  // 插入中间对话 ctrl[+shift]+w
    void actionInsertRightChat(bool next = false); // 插入右边对话 ctrl[+shift]+e
    void actionRenameChatNickname(); // 重命名对话 ctrl+r
    void actionCopyChat();  // 复制 ctrl+c
    void actionPasteChat(); // 粘贴 ctrl+v
    void actionChatMoveUp();  // 上移 ctrl+←
    void actionChatMoveDown(); // 下移 ctrl+→
    void actionChatDelete(); // 删除 delete

    // ==== 角色模板菜单 ====
    void actionInsertFigureDialogue(); // 插入到对话 空格 / ins
    void actionSelectFigureDialogue(); // 选中 shift+空格
    void actionUpdateFigureDialogues(); // 更新样式 ctrl+u
    void actionUpdateSelectedDialogues(); // 设为模板 ctrl+t
    void actionRenameFigureAndDialogues(); // 重命名 ctrl+r
    void actionFigureMoveUp(); // 上移 ctrl+←
    void actionFigureMoveDown(); // 下移 ctrl+→
    void actionFigureDelete(); // 删除 delete

    void slotSaveToFile(); // 导出 ctrl+o
    void slotLoadFromFile(); // 导入 ctrl+i

private:
    QString data_dir;

    QListWidget *dialogues_list_widget, *figure_list_widget;
    QPushButton *left_button, *mid_button, *right_button;
    DialogueEditor *editor;

    DialogueManager* manager;
    QList<DialogueBucket*> buckets;
};

#endif // DIALOGUEGROUP_H
