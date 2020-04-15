#include "dialogueavatar.h"

int DialogueAvatar::avatar_size = 50;

DialogueAvatar::DialogueAvatar(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueAvatar");
}

DialogueAvatar::DialogueAvatar(QPixmap pixmap, QWidget *parent) : DialogueAvatar(parent)
{
    setPixmap(pixmap);

    setScaledContents(true);
    setFixedSize(avatar_size, avatar_size);
}
