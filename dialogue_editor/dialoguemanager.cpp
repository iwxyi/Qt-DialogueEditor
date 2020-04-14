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
