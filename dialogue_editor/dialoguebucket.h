/**
  * 剧本 对话控件组
  */

#ifndef DIALOGUEBUCKET_H
#define DIALOGUEBUCKET_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include "dialogueavatar.h"
#include "dialoguenickname.h"
#include "dialoguebubble.h"
#include "dialoguenarrator.h"

enum ChatType {
    SelfChat, // 自己说的（右边）
    OppoChat, // 对方说的（左边）
    NarrChat, // 旁白文字（中间）
};

class DialogueBucket : public QWidget
{
    Q_OBJECT
    friend class DialogueEditor;
    friend class DialogueManager;
public:
    DialogueBucket(ChatType type, QString name, QPixmap avatar, QString said, QWidget *parent = nullptr);
    DialogueBucket(QString narr, QWidget *parent = nullptr);
    DialogueBucket(const DialogueBucket* b);

    void initView(QString name, QPixmap ava, QString said);
    void initView(QString narr);

    bool isSelf();
    bool isNarrator() const;

    void setNameVisible(bool visible);

    static QString getDefaultChatStyleSheet();
    static QString getDefaultNarratorStyleSheet();

    QString getName();

signals:
    void signalBubbleChanged();

public slots:

protected:
    ChatType type = SelfChat;
    QString figure_id; // 角色的id
    DialogueNickname *nickname = nullptr; // 姓名
    DialogueAvatar *avatar = nullptr; // 头像
    DialogueBubble *bubble = nullptr; // 气泡
    DialogueNarrator *narrator = nullptr; // 旁白
};

#endif // DIALOGUEBUCKET_H
