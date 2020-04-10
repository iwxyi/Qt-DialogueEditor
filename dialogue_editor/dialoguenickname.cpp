#include "dialoguenickname.h"

DialogueNickname::DialogueNickname(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueFigure");
}

DialogueNickname::DialogueNickname(QString name, QWidget *parent) : DialogueNickname(parent)
{
    setText(name);
}
