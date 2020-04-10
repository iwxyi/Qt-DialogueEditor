#include "dialogueavatar.h"

DialogueAvatar::DialogueAvatar(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueAvatar");

    setStyleSheet("border-radius: 100%; width: 64px; height: 64px;");
}

DialogueAvatar::DialogueAvatar(QPixmap pixmap, QWidget *parent) : DialogueAvatar(parent)
{
    setPixmap(pixmap);
}
