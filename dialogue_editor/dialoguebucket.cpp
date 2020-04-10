#include "dialoguebucket.h"

DialogueBucket::DialogueBucket(bool self, QString name, QPixmap avatar, QString said, QWidget *parent) : QWidget(parent)
{
    setObjectName("DialogueBucket");

    this->is_self = self;

    initView(name, avatar, said);
    initStyle();
}

void DialogueBucket::initView(QString name, QPixmap ava, QString said)
{
    QHBoxLayout* hlayout = new QHBoxLayout(this);
    QVBoxLayout* vlayout = new QVBoxLayout;
    avatar = new DialogueAvatar(ava, this);
    name_label = new QLabel(name, this);
    bubble = new DialogueBubble(said, this);

    vlayout->addWidget(name_label);
    vlayout->addWidget(bubble);

    if (is_self) // 自己说的话，靠右对齐
    {
        hlayout->addLayout(vlayout);
        hlayout->addWidget(avatar);
        hlayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    else // 对方说的话，靠左对齐
    {
        hlayout->addWidget(avatar);
        hlayout->addLayout(vlayout);
        hlayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
}

void DialogueBucket::initStyle()
{

}
