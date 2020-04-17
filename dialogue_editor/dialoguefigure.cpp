#include "dialoguefigure.h"

DialogueFigure::DialogueFigure(QObject *parent) : QObject(parent)
{

}

bool DialogueFigure::isNarrator()
{
    return type == NarrChat;
}

void DialogueFigure::setNameReg(QString s)
{
    name_reg.setPattern(s);
}

void DialogueFigure::setLineReg(QString s)
{
    line_reg.setPattern(s);
}
