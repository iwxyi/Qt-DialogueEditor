#include "dialoguenarrator.h"

DialogueNarrator::DialogueNarrator(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueNarrator");

    setStyleSheet("border: none; border-radius: 5px; background: #eeeeee; padding: 10px; ");
}

DialogueNarrator::DialogueNarrator(QString narr, QWidget *parent) : DialogueNarrator(parent)
{
    setText(narr);
}
