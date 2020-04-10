/**
  * 剧本 对话控件组
  */

#ifndef DIALOGUEBUCKET_H
#define DIALOGUEBUCKET_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "dialogueavatar.h"
#include "dialoguebubble.h"

class DialogueBucket : public QWidget
{
    Q_OBJECT
public:
    DialogueBucket(bool self, QString name, QPixmap avatar, QString said, QWidget *parent = nullptr);

    void initView(QString name, QPixmap ava, QString said);
    void initStyle();

signals:

public slots:

private:
    bool is_self = false; // 是否是自己说的话（即在右边）

    QLabel *name_label;
    DialogueAvatar *avatar; // 头像
    DialogueBubble *bubble; // 气泡
};

#endif // DIALOGUEBUCKET_H
