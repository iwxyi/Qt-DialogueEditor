#include "dialoguebubble.h"

DialogueBubble::DialogueBubble(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueBubble");

    setStyleSheet("border: none; border-radius: 5px; background: #eeeeee");
}

DialogueBubble::DialogueBubble(QString text, QWidget *parent) : QLabel(text, parent)
{

}
