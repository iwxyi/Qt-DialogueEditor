#include "dialoguenickname.h"

DialogueNickname::DialogueNickname(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueNickname");
}

DialogueNickname::DialogueNickname(QString name, QWidget *parent) : DialogueNickname(parent)
{
    setText(name);
}
