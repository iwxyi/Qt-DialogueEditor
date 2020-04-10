#include "dialoguebucket.h"

DialogueBucket::DialogueBucket(ChatType type, QString name, QPixmap avatar, QString said, QWidget *parent) : QWidget(parent)
{
    setObjectName("DialogueBucket");

    this->type = type;

    initView(name, avatar, said);
}

DialogueBucket::DialogueBucket(QString narr, QWidget *parent) : QWidget(parent)
{
    setObjectName("DialogueBucket");
    this->type = NarrChat;

    initView(narr);
}

void DialogueBucket::initView(QString name, QPixmap ava, QString said)
{
    QHBoxLayout* hlayout = new QHBoxLayout(this);
    QVBoxLayout* vlayout = new QVBoxLayout;
    QVBoxLayout* alayout = new QVBoxLayout;
    avatar = new DialogueAvatar(ava, this);
    figure = new DialogueFigure(name, this);
    bubble = new DialogueBubble(said, this);

    alayout->addWidget(avatar);
    vlayout->addWidget(figure);
    vlayout->addWidget(bubble);

    hlayout->setSpacing(12);
    vlayout->setSpacing(4);
    vlayout->setMargin(8);
    vlayout->setAlignment(Qt::AlignVCenter);
    alayout->setAlignment(Qt::AlignTop);

    if (isSelf()) // 自己说的话，靠右对齐
    {
        hlayout->addLayout(vlayout);
        hlayout->addLayout(alayout);
        hlayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        figure->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    else // 对方说的话，靠左对齐
    {
        hlayout->addLayout(alayout);
        hlayout->addLayout(vlayout);
        hlayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
}

void DialogueBucket::initView(QString narr)
{
    narrator = new DialogueNarrator(narr, this);
    QHBoxLayout* hlayout = new QHBoxLayout(this);
    hlayout->addWidget(narrator);

    hlayout->setAlignment(Qt::AlignCenter);
}

bool DialogueBucket::isSelf()
{
    return type == SelfChat;
}

bool DialogueBucket::isNarrator()
{
    return type == NarrChat;
}

void DialogueBucket::setNameVisible(bool visible)
{
    figure->setVisible(visible);
}
