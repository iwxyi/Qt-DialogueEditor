#include "dialoguemanager.h"

DialogueManager::DialogueManager(QObject *parent) : QObject(parent)
{

}

void DialogueManager::setDataDir(QString dir)
{
    if (!dir.endsWith("/"))
        dir += "/";
    this->data_dir = dir;
}

/**
 * 从文件从读取角色模板和配置信息
 * data_dir/
 *   - figures/
 *     - figure_id/
 *       - config.ini
 *       - style_sheet.qss
 *       - avatar.png
 */
void DialogueManager::loadData()
{
    QDir dirs(data_dir+"figures"); // data_dir/figures/aaa/config
    auto infos = dirs.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (auto info, infos)
    {
        if (!info.isDir())
            continue;
        QDir dir(info.absoluteFilePath());
        if (!QFile(dir.absoluteFilePath("config.ini")).exists())
            continue;
        QSettings s(dir.absoluteFilePath("config.ini"), QSettings::IniFormat, this);
        createFigure((ChatType)(s.value("type").toInt()), s.value("nickname").toString(), QPixmap(dir.absoluteFilePath("avatar.png")), readTextFile(dir.absoluteFilePath("style_sheet.qss")), s.value("id").toString());
    }
}

/**
 * 保存角色模板与设置到文件
 */
void DialogueManager::saveData(DialogueFigure *figure)
{
    QString path = data_dir + "figures/" + figure->figure_id + "/";
    QDir dir(path);
    dir.mkpath(path);
    QSettings s(path + "config.ini", QSettings::IniFormat, this);
    s.setValue("id", figure->figure_id);
    s.setValue("type", (int)figure->type);
    s.setValue("nickname", figure->nickname);
    figure->avatar.save(dir.absoluteFilePath("avatar.png"));
    writeTextFile(dir.absoluteFilePath("style_sheet.qss"), figure->qss);
}

/**
 * 根据名字判断有没有对应的角色
 * 如果有则覆盖；如果没有在重新创建一个
 */
void DialogueManager::saveFigure(DialogueBucket *bucket)
{
    DialogueFigure* figure = bucket->figure_id.isEmpty() ? nullptr : getFigureById(bucket->figure_id);
    if (!figure) // 没有相同ID，尝试按名字查找
        figure = bucket->isNarrator() ? nullptr : getFigureByName(bucket->getName());
    if (!figure) // 如果没有对应的角色模板，则创建
        figure = bucket->isNarrator()
                ? createFigure(bucket->type, bucket->styleSheet())
                : createFigure(bucket->type, bucket->nickname->text(), *bucket->avatar->pixmap(), bucket->styleSheet());
    else // 更新模板
    {
        qDebug() << "saave";
        figure->type = bucket->type;
        if (figure->type != NarrChat)
        {
            figure->nickname = bucket->getName();
            figure->avatar = bucket->getAvatar();
        }
        figure->qss = bucket->styleSheet();
    }
    // 保存figure
    saveData(figure);
}

void DialogueManager::deleteFigure(DialogueFigure* figure)
{
    figures.removeOne(figure);
    QString id = figure->figure_id;
    QDir dir(data_dir + "figures/" + id);
    dir.removeRecursively(); // 强制删除文件夹及其内容
    delete figure;
}

QList<DialogueFigure *> &DialogueManager::getFigures()
{
    return figures;
}

DialogueFigure *DialogueManager::getFigureByName(QString name)
{
    foreach (auto figure, figures)
    {
        if (figure->nickname == name)
            return figure;
    }
    return nullptr;
}

DialogueFigure *DialogueManager::getFigureById(QString id)
{
    foreach (auto figure, figures)
    {
        if (figure->figure_id == id)
            return figure;
    }
    return nullptr;
}

DialogueFigure *DialogueManager::createFigure(ChatType t, QString n, QPixmap a, QString ss, QString id)
{
    auto figure = new DialogueFigure(this);
    figure->figure_id = id.isEmpty() ? createFigureID() : id;
    figure->type = t;
    figure->nickname = n;
    figure->avatar = a;
    figure->qss = ss;
    figures.append(figure);
    return figure;
}

DialogueFigure *DialogueManager::createFigure(ChatType t, QString ss, QString id)
{
    auto figure = new DialogueFigure(this);
    figure->figure_id = id.isEmpty() ? createFigureID() : id;
    figure->type = t;
    figure->nickname = "[旁白]";
    figure->qss = ss;
    figures.append(figure);
    return figure;
}

QString DialogueManager::createFigureID()
{
    QString id;
    QString chars = "qwertyuiopasdfghjklzxcvbnm1029384756";
    const int len = 8, l = chars.length();
    srand(time(0));
    do {
        id = "";
        for (int i = 0; i < len; i++)
            id += chars.at(rand() % l);
    } while (getFigureById(id) != nullptr);
    return id;
}

QString DialogueManager::readTextFile(QString path)
{
    QString text;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "";
    }
    if (!file.isReadable())
    {
        return "";
    }
    QTextStream text_stream(&file);
    text_stream.setCodec(QTextCodec::codecForName(QByteArray("utf-8")));
    while(!text_stream.atEnd())
    {
        text = text_stream.readAll();
    }
    file.close();
    return text;
}

bool DialogueManager::writeTextFile(QString path, QString text)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    }
    QTextStream text_stream(&file);
    text_stream.setCodec(QTextCodec::codecForName(QByteArray("utf-8")));
    text_stream << text;
    file.close();
    return true;
}
