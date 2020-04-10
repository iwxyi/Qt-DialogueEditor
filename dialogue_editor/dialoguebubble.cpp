#include "dialoguebubble.h"

DialogueBubble::DialogueBubble(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueBubble");
}

DialogueBubble::DialogueBubble(QString text, QWidget *parent) : DialogueBubble(parent)
{
    setText(text);
}
