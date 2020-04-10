#include "dialoguefigure.h"

DialogueFigure::DialogueFigure(QWidget *parent) : QLabel(parent)
{
    setObjectName("DialogueFigure");

    setStyleSheet("#DialogueFigure { color: gray; }");
}

DialogueFigure::DialogueFigure(QString name, QWidget *parent) : DialogueFigure(parent)
{
    setText(name);
}
