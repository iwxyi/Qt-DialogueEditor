#include "dialogueavatar.h"

DialogueAvatar::DialogueAvatar(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueAvatar");

    setStyleSheet("#DialogueAvatar { border-radius: 100%; width: 32px; height: 32px; }");
}

DialogueAvatar::DialogueAvatar(QPixmap pixmap, QWidget *parent) : DialogueAvatar(parent)
{
    setPixmap(pixmap);
}
