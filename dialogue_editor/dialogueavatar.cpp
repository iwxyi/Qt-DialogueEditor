#include "dialogueavatar.h"

DialogueAvatar::DialogueAvatar(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueAvatar");
}

DialogueAvatar::DialogueAvatar(QPixmap pixmap, QWidget *parent) : DialogueAvatar(parent)
{
    setPixmap(pixmap);
}
