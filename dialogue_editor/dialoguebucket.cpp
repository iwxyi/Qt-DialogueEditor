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
    {
    }

    hlayout->setSpacing(12);
    vlayout->setSpacing(4);
    vlayout->setMargin(8);
    vlayout->setAlignment(Qt::AlignVCenter);
    alayout->setAlignment(Qt::AlignTop);

    QHBoxLayout* flayout = new QHBoxLayout;
    QHBoxLayout* blayout = new QHBoxLayout;
    if (isSelf()) // 自己说的话，靠右对齐
    {
        hlayout->addSpacing(64);
        hlayout->addLayout(vlayout);
        hlayout->addLayout(alayout);

        flayout->addStretch(1);
        flayout->addWidget(figure);
        blayout->addStretch(1);
        blayout->addWidget(bubble);

        hlayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        figure->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    else // 对方说的话，靠左对齐
    {
        hlayout->addLayout(alayout);
        hlayout->addLayout(vlayout);
        hlayout->addSpacing(64);

        flayout->addWidget(figure);
        flayout->addStretch(1);
        blayout->addWidget(bubble);
        blayout->addStretch(1);

        hlayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
    vlayout->addLayout(flayout);
    vlayout->addLayout(blayout);
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

QString DialogueBucket::getDefaultChatStyleSheet()
{
    QFile file(":/styles/chat");
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    return in.readAll();
}

QString DialogueBucket::getDefaultNarratorStyleSheet()
{
    QFile file(":/styles/narrator");
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    return in.readAll();
}
