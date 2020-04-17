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

DialogueBucket::DialogueBucket(const DialogueBucket *b)
{
    setObjectName("DialogueBucket");

    figure_id = b->figure_id;
    type = b->type;

    if (b->isNarrator())
    {
        initView(b->narrator->text());
    }
    else
    {
        initView(b->nickname->text(), *b->avatar->pixmap(), b->bubble->text());
    }
}

void DialogueBucket::initView(QString name, QPixmap ava, QString said)
{
    QHBoxLayout* hlayout = new QHBoxLayout(this);
    QVBoxLayout* vlayout = new QVBoxLayout;
    QVBoxLayout* alayout = new QVBoxLayout;
    avatar = new DialogueAvatar(ava, this);
    nickname = new DialogueNickname(name, this);
    bubble = new DialogueBubble(said, this);

    alayout->addWidget(avatar);
    {
    }

    hlayout->setSpacing(8);
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
        flayout->addWidget(nickname);
        blayout->addStretch(1);
        blayout->addWidget(bubble);

        hlayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        nickname->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    else // 对方说的话，靠左对齐
    {
        hlayout->addLayout(alayout);
        hlayout->addLayout(vlayout);
        hlayout->addSpacing(64);

        flayout->addWidget(nickname);
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

bool DialogueBucket::isNarrator() const
{
    return type == NarrChat;
}

void DialogueBucket::setNameVisible(bool visible)
{
    if (isNarrator())
        return ;
    nickname->setVisible(visible);
}

QString DialogueBucket::readTextFile(QString path)
{
    QString text;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "";
    }
    if (!file.isReadable())
    {
        return "";
    }
    QTextStream text_stream(&file);
    text_stream.setCodec(QTextCodec::codecForName(QByteArray("utf-8")));
    while(!text_stream.atEnd())
    {
        text = text_stream.readAll();
    }
    file.close();
    return text;
}

QString DialogueBucket::getDefaultChatStyleSheet()
{
    return readTextFile(":/styles/chat");
}

QString DialogueBucket::getDefaultNarratorStyleSheet()
{
    return readTextFile(":/styles/narrator");
}

QString DialogueBucket::getName()
{
    if (type == ChatType::NarrChat)
        return "";
    else
        return nickname->text();
}

QPixmap DialogueBucket::getAvatar()
{
    return *avatar->pixmap();
}

QString DialogueBucket::getSaid()
{
    if (isNarrator())
        return narrator->text();
    else
        return bubble->text();
}

void DialogueBucket::setName(QString name)
{
    nickname->setText(name);
}

void DialogueBucket::setAvatar(QPixmap pixmap)
{
    avatar->setPixmap(pixmap);
}

void DialogueBucket::setNarrator(QString narrator)
{
    this->narrator->setText(narrator);
}

void DialogueBucket::setAvatarSize(int x)
{
    avatar->setFixedSize(x, x);
}

void DialogueBucket::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        press_pos = event->pos();
    }
    return QWidget::mousePressEvent(event);
}

void DialogueBucket::mouseMoveEvent(QMouseEvent *event)
{
    // 纵向拖拽是多选，此处不处理
    // 横向拖拽是导出
    if ((event->buttons() & Qt::LeftButton) && (event->pos().x() < 0-QApplication::startDragDistance() || event->pos().x() > width() + QApplication::startDragDistance()))
    {
        if (event->pos().x() < 0)
            press_pos.setX(0);
        else if (event->pos().x() > width())
            press_pos.setX(width());
        emit signalDragOutEvent(mapToGlobal(press_pos));
    }

    return QWidget::mouseMoveEvent(event);
}
