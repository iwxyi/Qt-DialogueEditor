#include "dialoguebubble.h"

DialogueBubble::DialogueBubble(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueBubble");

    setStyleSheet("#DialogueBubble { border: none; border-radius: 5px; background-color: #eeeeee; padding: 10px; }");
}

DialogueBubble::DialogueBubble(QString text, QWidget *parent) : DialogueBubble(parent)
{
    setText(text);
}
