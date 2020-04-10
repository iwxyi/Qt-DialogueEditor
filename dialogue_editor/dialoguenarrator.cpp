#include "dialoguenarrator.h"

DialogueNarrator::DialogueNarrator(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueNarrator");
}

DialogueNarrator::DialogueNarrator(QString narr, QWidget *parent) : DialogueNarrator(parent)
{
    setText(narr);
}
