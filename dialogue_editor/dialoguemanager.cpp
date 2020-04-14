#include "dialoguemanager.h"

DialogueManager::DialogueManager(QObject *parent) : QObject(parent)
{

}

void DialogueManager::setDataDir(QString dir)
{
    this->data_dir = dir;
}

/**
 * 从文件从读取角色模板和配置信息
 */
void DialogueManager::loadData()
{

}

/**
 * 保存角色模板与设置到文件
 */
void DialogueManager::saveData()
{

}

/**
 * 根据名字判断有没有对应的角色
 * 如果有则覆盖；如果没有在重新创建一个
 */
void DialogueManager::saveFigure(DialogueBucket *bucket)
{
    DialogueFigure* figure = bucket->figure_id.isEmpty() ? nullptr : getFigure(bucket->figure_id);
    if (!figure) // 如果没有对应的角色模板，则创建
        figure = createFigure(bucket->type, bucket->nickname->text(), *bucket->avatar->pixmap(), bucket->styleSheet());

}

void DialogueManager::deleteFigure(QString name)
{

}

QList<DialogueFigure *> &DialogueManager::getFigures()
{
    return figures;
}

DialogueFigure *DialogueManager::getFigure(QString id)
{
    foreach (auto figure, figures)
    {
        if (figure->figure_id == id)
            return figure;
    }
    return nullptr;
}

DialogueFigure *DialogueManager::createFigure(ChatType t, QString n, QPixmap a, QString ss)
{
    auto figure = new DialogueFigure(this);
    figure->type = t;
    figure->nickname = n;
    figure->avatar = a;
    figure->qss = ss;
    figures.append(figure);
}
