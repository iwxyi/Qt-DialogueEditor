#include "dialoguegroup.h"

DialogueGroup::DialogueGroup(QWidget *parent) : QWidget(parent)
{
    initView();
    initStyle();
    initData();
}

DialogueGroup::DialogueGroup(QString dir, QWidget *parent) : DialogueGroup(parent)
{
    setDataDirAndLoad(dir);
}

void DialogueGroup::initView()
{
    dialogues_list_widget = new QListWidget(this);
    figure_list_widget = new QListWidget(this);
    left_button = new QPushButton("+左边", this);
    mid_button = new QPushButton("+旁白", this);
    right_button = new QPushButton("+右边", this);

    QVBoxLayout* list_vlayout = new QVBoxLayout;
    list_vlayout->addWidget(dialogues_list_widget);
    QHBoxLayout* button_hlayout = new QHBoxLayout;
    button_hlayout->addWidget(left_button);
    button_hlayout->addWidget(mid_button);
    button_hlayout->addWidget(right_button);
    list_vlayout->addLayout(button_hlayout);

    editor = new DialogueEditor(this);
    QHBoxLayout* main_hlayout = new QHBoxLayout(this);
    main_hlayout->addWidget(figure_list_widget);
    main_hlayout->addLayout(list_vlayout);
    main_hlayout->addWidget(editor);

    main_hlayout->setStretch(0, 1);
    main_hlayout->setStretch(1, 3);
    main_hlayout->setStretch(2, 2);

    dialogues_list_widget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    dialogues_list_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    dialogues_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    dialogues_list_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    figure_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    figure_list_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    figure_list_widget->setToolTip("角色模板，双击插入该角色至对话框\n通过最右边“保存角色”创建");

    connect(left_button, SIGNAL(clicked()), this, SLOT(slotAddLeftChat()));
    connect(mid_button, SIGNAL(clicked()), this, SLOT(slotAddNarrator()));
    connect(right_button, SIGNAL(clicked()), this, SLOT(slotAddRightChat()));

    connect(dialogues_list_widget, &QListWidget::currentRowChanged, this, [=](int row){
        if (row == -1 || row > dialogues_list_widget->count())
        {
            editor->setBucket(nullptr);
            return ;
        }
        auto bucket = buckets[row];
        editor->setBucket(bucket);
    });
    connect(dialogues_list_widget, &QListWidget::doubleClicked, this, [=](const QModelIndex& index) {
        int row = index.row();
        editor->focusSaid();
    });
    connect(dialogues_list_widget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDialogueMenuShowed(QPoint)));

    connect(editor, SIGNAL(signalSaveFigure(DialogueBucket*)), this, SLOT(slotSaveFigure(DialogueBucket*)));
    connect(editor, SIGNAL(signalSaveToFile()), this, SLOT(slotSaveToFile()));

    connect(figure_list_widget, &QListWidget::doubleClicked, this, [=](const QModelIndex& index) {
        int row = index.row();
        auto figure = manager->getFigures().at(row);
        slotInsertFromFigure(figure);
        dialogues_list_widget->setFocus();
        if (dialogues_list_widget->selectedItems().size() == 1)
            editor->focusSaid();
    });
    connect(figure_list_widget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotFigureMenuShowed(QPoint)));
}

void DialogueGroup::initStyle()
{

}

void DialogueGroup::initData()
{
    manager = new DialogueManager(this);
}

void DialogueGroup::setDataDirAndLoad(QString dir)
{
    data_dir = dir;

    manager->setDataDir(dir);
    manager->loadData();
    refreshFigures();

    editor->setDataDir(dir);
}

void DialogueGroup::fromText(QString full)
{
    QStringList paras = full.split("\n", QString::SkipEmptyParts);
    foreach (auto para, paras)
    {
        para = para.trimmed();
        if (para.isEmpty())
            continue;
        if (para.indexOf("“") == -1 && para.indexOf(":") == -1 && para.indexOf("：") == -1) // 是旁白
        {
            addChat(new DialogueBucket(para, this));
        }
        else // 是语言
        {
            QString name, said;
            if (para.indexOf("“") != -1)
            {
                int l_pos = para.indexOf("“");
                int r_pos = para.indexOf("”", l_pos+1);
                if (r_pos == -1)
                    r_pos = para.length();
                name = para.mid(0, l_pos).trimmed();
                said = para.mid(l_pos+1, r_pos-l_pos-1).trimmed();
                if (name.endsWith("：") || name.endsWith(":"))
                    name = name.left(name.length()-1);
            }
            else
            {
                int c_pos= para.indexOf("：");
                if (c_pos == -1)
                    c_pos = para.indexOf(":");
                name = para.left(c_pos);
                said = para.right(para.length() - c_pos - 1);
            }

            ChatType type = (name=="我") ? ChatType::SelfChat : ChatType::OppoChat;
            QPixmap avatar = QPixmap(":/avatars/girl_1"); // TODO: 确保默认头像存在
            QString style = DialogueBucket::getDefaultChatStyleSheet();

            auto figure = manager->getFigureByName(name);
            if (figure == nullptr) // 没有这个人，针对名字智能获取
            {
                QString temp = name;
                const QStringList chopped = QString("道 说 问 讲").split(" ");
                for (int i = 0; i < chopped.size(); i++)
                {
                    auto chop = chopped.at(i);
                    if (name.endsWith(chop))
                    {
                        name = name.left(name.length() - chop.length());
                        if ((figure = manager->getFigureByName(name)) != nullptr) // 有这个人名了
                            break;
                        i = 0; // 从头开始
                    }
                }
            }
            if (figure != nullptr)
            {
                type = figure->type;
                avatar = figure->avatar;
                style = figure->qss;
            }
            auto bucket = new DialogueBucket(type, name, avatar, said, this);
            bucket->setStyleSheet(style);
            addChat(bucket);
        }
    }
}

QString DialogueGroup::toText(QList<DialogueBucket*> buckets, QString indent_blank, QString indent_line)
{
    QString full;
    foreach (auto bucket, buckets)
    {
        if (bucket->isNarrator())
        {
            full += indent_blank + bucket->getSaid();
        }
        else
        {
            // 注意：可能说的话没有名字，这个直接没名字就行了
            full += indent_blank + bucket->getName() + "：“" + bucket->getSaid() + "”";
        }
        full += indent_line;
    }
    return full;
}

QString DialogueGroup::toText(QString indent_blank, QString indent_line)
{
    return toText(buckets, indent_blank, indent_line);
}

void DialogueGroup::fromJson(QJsonObject)
{

}

QJsonObject DialogueGroup::toJson()
{

}

QPixmap DialogueGroup::toPixmap()
{
    dialogues_list_widget->clearSelection();
    auto scrollBar = dialogues_list_widget->verticalScrollBar();
    QSize size(dialogues_list_widget->contentsRect().width() - scrollBar->width(), scrollBar->maximum() + scrollBar->pageStep());
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    auto focus = dialogues_list_widget->focusPolicy();
    dialogues_list_widget->setFocusPolicy(Qt::NoFocus); // 避免鼠标悬浮

    // 分页绘制
    int page = 0; // 第几页
    while (page * scrollBar->pageStep() <= size.height())
    {
        QRect rd_rect = dialogues_list_widget->contentsRect();
        const int start_y = page * scrollBar->pageStep(); // 页顶
        int end_y = (page+1) * scrollBar->pageStep();
        if (end_y > size.height()) // 最后一页，不满一页
        {
            int t = rd_rect.top() + (end_y - size.height());
            rd_rect.setTop(t);
            end_y = size.height();
        }
        scrollBar->setSliderPosition(start_y);
        dialogues_list_widget->render(&pixmap, QPoint(0, start_y), rd_rect);
        page++;
    }
    dialogues_list_widget->setFocusPolicy(focus);

    return pixmap;
}

void DialogueGroup::setAvatarSize(int x)
{
    DialogueAvatar::avatar_size = x;
    foreach (auto bucket, buckets)
    {
        bucket->setAvatarSize(x);
    }
}

void DialogueGroup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (dialogues_list_widget)
    {
        int w = dialogues_list_widget->contentsRect().width() - dialogues_list_widget->verticalScrollBar()->width();
        foreach (auto bucket, buckets) {
            bucket->setMaximumWidth(w);
        }
        for (int i = 0; i < dialogues_list_widget->count(); i++)
        {
            dialogues_list_widget->item(i)->setSizeHint(QSize(w, dialogues_list_widget->item(i)->sizeHint().height()));
        }
    }
}

void DialogueGroup::slotAddLeftChat()
{
    auto bucket = new DialogueBucket(OppoChat, "名字", QPixmap(":/avatars/girl_1"), "说的话", this);
    bucket->setStyleSheet(DialogueBucket::getDefaultChatStyleSheet());
    auto item = addChat(bucket);
    dialogues_list_widget->clearSelection();
    dialogues_list_widget->setCurrentItem(item);
    dialogues_list_widget->scrollToItem(item);
    dialogues_list_widget->setFocus();
}

void DialogueGroup::slotAddNarrator()
{
    auto bucket = new DialogueBucket("旁白", this);
    bucket->setStyleSheet(DialogueBucket::getDefaultNarratorStyleSheet());
    auto item = addChat(bucket);
    dialogues_list_widget->clearSelection();
    dialogues_list_widget->setCurrentItem(item);
    dialogues_list_widget->scrollToItem(item);
    dialogues_list_widget->setFocus();
}

void DialogueGroup::slotAddRightChat()
{
    auto bucket = new DialogueBucket(SelfChat, "我", QPixmap(":/avatars/boy_1"), "说的话", this);
    bucket->setStyleSheet(DialogueBucket::getDefaultChatStyleSheet());
    bucket->setNameVisible(false);
    auto item = addChat(bucket);
    dialogues_list_widget->clearSelection();
    dialogues_list_widget->setCurrentItem(item);
    dialogues_list_widget->scrollToItem(item);
    dialogues_list_widget->setFocus();
}

void DialogueGroup::slotDialogueMenuShowed(QPoint)
{
    QMenu* menu = new QMenu("菜单");
    QAction* insert_left_action = new QAction("插入左边", this);
    QAction* insert_narr_action = new QAction("插入旁白", this);
    QAction* insert_right_action = new QAction("插入右边", this);
    QAction* copy_chat_action = new QAction("复制", this);
    QAction* paste_chat_action = new QAction("粘贴", this);
    QAction* move_up_action = new QAction("上移", this);
    QAction* move_down_action = new QAction("下移", this);
    QAction* delete_action = new QAction("删除", this);

    menu->addAction(insert_left_action);
    menu->addAction(insert_narr_action);
    menu->addAction(insert_right_action);
    menu->addSeparator();
    menu->addAction(copy_chat_action);
    menu->addAction(paste_chat_action);
    menu->addSeparator();
    menu->addAction(move_up_action);
    menu->addAction(move_down_action);
    menu->addSeparator();
    menu->addAction(delete_action);

    connect(insert_left_action, SIGNAL(triggered()), this, SLOT(actionInsertLeftChat()));
    connect(insert_narr_action, SIGNAL(triggered()), this, SLOT(actionInsertNarrator()));
    connect(insert_right_action, SIGNAL(triggered()), this, SLOT(actionInsertRightChat()));
    connect(copy_chat_action, SIGNAL(triggered()), this, SLOT(actionCopyChat()));
    connect(paste_chat_action, SIGNAL(triggered()), this, SLOT(actionPasteChat()));
    connect(move_up_action, SIGNAL(triggered()), this, SLOT(actionChatMoveUp()));
    connect(move_down_action, SIGNAL(triggered()), this, SLOT(actionChatMoveDown()));
    connect(delete_action, SIGNAL(triggered()), this, SLOT(actionChatDelete()));

    if (dialogues_list_widget->selectedItems().size() == 0)
    {
        insert_left_action->setEnabled(false);
        insert_narr_action->setEnabled(false);
        insert_right_action->setEnabled(false);
        copy_chat_action->setEnabled(false);
        move_up_action->setEnabled(false);
        move_down_action->setEnabled(false);
        delete_action->setEnabled(false);
    }
    if (QApplication::clipboard()->text().isEmpty())
    {
        paste_chat_action->setEnabled(false);
    }

    menu->exec(QCursor::pos());

    menu->deleteLater();
}

void DialogueGroup::slotFigureMenuShowed(QPoint pos)
{
    QMenu* menu = new QMenu("菜单");
    QAction* insert_dialogue_action = new QAction("添加该角色对话", this);
    QAction* select_all_dialogue_action = new QAction("选中该角色所有对话", this);
    QAction* figure_update_all_action = new QAction("套用样式至该角色所有对话", this);
    QAction* figure_update_select_action = new QAction("套用样式至选中对话", this);
    QAction* move_up_action = new QAction("上移", this);
    QAction* move_down_action = new QAction("下移", this);
    QAction* delete_action = new QAction("删除", this);

    menu->addAction(insert_dialogue_action);
    menu->addAction(select_all_dialogue_action);
    menu->addSeparator();
    menu->addAction(figure_update_all_action);
    menu->addAction(figure_update_select_action);
    menu->addSeparator();
    menu->addAction(move_up_action);
    menu->addAction(move_down_action);
    menu->addSeparator();
    menu->addAction(delete_action);

    connect(insert_dialogue_action, SIGNAL(triggered()), this, SLOT(actionInsertFigureDialogue()));
    connect(select_all_dialogue_action, SIGNAL(triggered()), this, SLOT(actionSelectFigureDialogue()));
    connect(figure_update_all_action, SIGNAL(triggered()), this, SLOT(actionUpdateFigureDialogues()));
    connect(figure_update_select_action, SIGNAL(triggered()), this, SLOT(actionUpdateSelectedDialogues()));
    connect(move_up_action, SIGNAL(triggered()), this, SLOT(actionFigureMoveUp()));
    connect(move_down_action, SIGNAL(triggered()), this, SLOT(actionFigureMoveDown()));
    connect(delete_action, SIGNAL(triggered()), this, SLOT(actionFigureDelete()));

    if (figure_list_widget->selectedItems().size() == 0) // 没有选中
    {
        insert_dialogue_action->setEnabled(false);
        select_all_dialogue_action->setEnabled(false);
        figure_update_all_action->setEnabled(false);
        figure_update_select_action->setEnabled(false);
        move_up_action->setEnabled(false);
        move_down_action->setEnabled(false);
        delete_action->setEnabled(false);
    }
    if (figure_list_widget->selectedItems().size() > 1)
    {
        insert_dialogue_action->setEnabled(false);
        figure_update_select_action->setEnabled(false);
    }

    menu->exec(QCursor::pos());

    menu->deleteLater();
}

void DialogueGroup::slotSaveFigure(DialogueBucket *bucket)
{
    if (bucket == nullptr)
        return ;
    manager->saveFigure(bucket);
    manager->saveOrder();
    refreshFigures();
}

void DialogueGroup::slotInsertFromFigure(DialogueFigure *figure)
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    if (figure->type == ChatType::NarrChat)
    {
        if (items.size() == 0) // 没有选中
        {
            insertBucketAndSetQSS(
                -1,
                new DialogueBucket("旁白", this),
                figure->qss
            );
        }
        else
        {
            foreach (auto item, items)
            {
                insertBucketAndSetQSS(
                    item,
                    new DialogueBucket("旁白", this),
                    figure->qss,
                    true
                );
            }
        }
    }
    else
    {
        if (items.size() == 0) // 没有选中
        {
            insertBucketAndSetQSS(
                -1,
                new DialogueBucket(figure->type, figure->nickname, figure->avatar, "说的话", this),
                figure->qss
            );
        }
        else
        {
            foreach (auto item, items)
            {
                insertBucketAndSetQSS(
                    item,
                    new DialogueBucket(figure->type, figure->nickname, figure->avatar, "说的话", this),
                    figure->qss,
                    true
                );
            }
        }
    }
}

void DialogueGroup::insertBucketAndSetQSS(QListWidgetItem* item, DialogueBucket *bucket, QString qss, bool above)
{
    int row = (item == nullptr ? -1 : dialogues_list_widget->row(item));
    insertBucketAndSetQSS(row, bucket, qss, above);
}

void DialogueGroup::insertBucketAndSetQSS(int row, DialogueBucket *bucket, QString qss, bool above)
{
    if (!qss.isEmpty())
        bucket->setStyleSheet(qss);
    if (above)
    {
        if (row == -1)
            row = -2; // 保持插入在最后面
        else // 插入到下一项
            row = row + 1;
    }
    dialogues_list_widget->setCurrentItem(addChat(bucket, row));
}

void DialogueGroup::refreshFigures()
{
    figure_list_widget->clear();
    auto figures = manager->getFigures();
    for (int i = 0; i < figures.size(); i++)
    {
        auto figure = figures.at(i);
        QListWidgetItem* item = new QListWidgetItem(figure->avatar, figure->nickname);
        figure_list_widget->addItem(item);
    }
}

void DialogueGroup::actionInsertLeftChat()
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        insertBucketAndSetQSS(item,
                              new DialogueBucket(OppoChat, "名字", QPixmap(":/avatars/girl_1"), "说的话", this),
                              DialogueBucket::getDefaultChatStyleSheet());
    }
}

void DialogueGroup::actionInsertNarrator()
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        insertBucketAndSetQSS(item, new DialogueBucket("旁白", this),
                              DialogueBucket::getDefaultNarratorStyleSheet());
    }
}

void DialogueGroup::actionInsertRightChat()
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        auto bucket = new DialogueBucket(SelfChat, "我", QPixmap(":/avatars/boy_1"), "说的话", this);
        bucket->setNameVisible(false);
        insertBucketAndSetQSS(item, bucket,
                              DialogueBucket::getDefaultChatStyleSheet());
    }
}

void DialogueGroup::actionCopyChat()
{
    auto items = dialogues_list_widget->selectedItems();
    QList<DialogueBucket*>widgets;
    for (int i = 0; i < items.size(); i++)
    {
        int row = dialogues_list_widget->row(items.at(i));
        if (row < 0)
            continue;
        auto widget = buckets.at(row);
        widgets.append(widget);
    }
    QApplication::clipboard()->setText(toText(widgets));
}

void DialogueGroup::actionPasteChat()
{
    QString s = QApplication::clipboard()->text();
    if (s.isEmpty())
        return ;
    fromText(s);
}

void DialogueGroup::actionChatMoveUp()
{
    auto items = dialogues_list_widget->selectedItems();
    for (int i = 0; i < items.size(); i++)
    {
        int row = dialogues_list_widget->row(items.at(i));
        if (row <= 0)
            continue;
        auto item = dialogues_list_widget->takeItem(row);
        auto widget = buckets.takeAt(row);
        insertBucketAndSetQSS(row-1, new DialogueBucket(widget), widget->styleSheet());
        delete item;
        delete widget;
    }
}

void DialogueGroup::actionChatMoveDown()
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    for (int i = items.size()-1; i >= 0; i--) // 倒着来
    {
        int row = dialogues_list_widget->row(items.at(i));
        if (row >= dialogues_list_widget->count()-1)
            continue;
        auto item = dialogues_list_widget->takeItem(row);
        auto widget = buckets.takeAt(row);
        insertBucketAndSetQSS(row+1, new DialogueBucket(widget), widget->styleSheet());
        delete item;
        delete widget;
    }
}

void DialogueGroup::actionChatDelete()
{
    auto items = dialogues_list_widget->selectedItems();
    for (int i = 0; i < items.size(); i++)
    {
        int row = dialogues_list_widget->row(items.at(i));
        if (row < 0)
            continue;
        auto item = dialogues_list_widget->takeItem(row);
        auto widget = static_cast<DialogueBucket*>(dialogues_list_widget->itemWidget(item));
        buckets.takeAt(row);
        delete item;
        delete widget;
    }
}

void DialogueGroup::actionInsertFigureDialogue()
{
    auto items = figure_list_widget->selectedItems();
    auto figures = manager->getFigures();
    for (int i = 0; i < items.size(); i++)
    {
        int row = figure_list_widget->row(items.at(i));
        auto figure = figures.at(row);
        // 遍历插入选中项
        slotInsertFromFigure(figure);
    }
}

/**
 * 选中该角色所有对话
 */
void DialogueGroup::actionSelectFigureDialogue()
{
    auto items = figure_list_widget->selectedItems();
    auto figures = manager->getFigures();
    dialogues_list_widget->clearSelection();
    for (int i = 0; i < items.size(); i++)
    {
        int row = figure_list_widget->row(items.at(i));
        auto figure = figures.at(row); // 选中 角色
        // 遍历插入选中项
        if (figure->type == NarrChat)
        {
            // 选中所有旁白
            for (int k = 0; k < buckets.size(); k++)
            {
                auto bucket = buckets.at(k);
                if (bucket->isNarrator())
                {
                    dialogues_list_widget->setCurrentRow(k, QItemSelectionModel::Select);
                }
            }
        }
        else
        {
            // 根据名字选择
            QString name = figure->nickname;
            if (name.isEmpty())
                continue;
            for (int k = 0; k < buckets.size(); k++)
            {
                auto bucket = buckets.at(k);
                if (!bucket->isNarrator() && bucket->getName() == name)
                {
                    dialogues_list_widget->setCurrentRow(k, QItemSelectionModel::Select);
                }
            }
        }
    }
}

/**
 * 更新该角色对话样式为模板样式
 * 可多选角色模板
 */
void DialogueGroup::actionUpdateFigureDialogues()
{
    auto items = figure_list_widget->selectedItems();
    auto figures = manager->getFigures();
    dialogues_list_widget->clearSelection();
    for (int i = 0; i < items.size(); i++)
    {
        int row = figure_list_widget->row(items.at(i));
        auto figure = figures.at(row); // 选中 角色
        // 遍历插入选中项
        if (figure->type == NarrChat)
        {
            // 选中所有旁白
            for (int k = 0; k < buckets.size(); k++)
            {
                auto bucket = buckets.at(k);
                if (bucket->isNarrator())
                {
                    bucket->setNarrator(figure->said);
                }
            }
        }
        else
        {
            // 根据名字选择
            QString name = figure->nickname;
            if (name.isEmpty())
                continue;
            for (int k = 0; k < buckets.size(); k++)
            {
                auto bucket = buckets.at(k);
                if (!bucket->isNarrator() && bucket->getName() == name)
                {
                    bucket->setAvatar(figure->avatar);
                    bucket->setStyleSheet(figure->qss);
                }
            }
        }
    }
}

/**
 * 设置选中对话为该角色
 * （对话、旁白 分开）
 */
void DialogueGroup::actionUpdateSelectedDialogues()
{
    auto figure = manager->getFigures().at(figure_list_widget->currentRow());
    auto items = dialogues_list_widget->selectedItems();
    foreach (auto item, items)
    {
        int row = dialogues_list_widget->row(item);
        auto bucket = buckets.at(row);
        bucket->setName(figure->nickname);
        bucket->setAvatar(figure->avatar);
        bucket->setStyleSheet(figure->qss);
    }
}

void DialogueGroup::actionFigureMoveUp()
{
    auto items = figure_list_widget->selectedItems();
    auto& figures = manager->getFigures();
    for (int i = 0; i < items.size(); i++)
    {
        int row = figure_list_widget->row(items.at(i));
        if (row <= 0)
            continue;
        auto figure = figures.takeAt(row);
        figures.insert(row-1, figure);
    }
    manager->saveOrder();
    refreshFigures();
}

void DialogueGroup::actionFigureMoveDown()
{
    auto items = figure_list_widget->selectedItems();
    auto& figures = manager->getFigures();
    for (int i = items.size()-1; i >= 0; i--) // 倒着来
    {
        int row = figure_list_widget->row(items.at(i));
        if (row >= figures.size()-1)
            continue;
        auto figure = figures.takeAt(row);
        figures.insert(row+1, figure);
    }
    manager->saveOrder();
    refreshFigures();
}

void DialogueGroup::actionFigureDelete()
{
    auto items = figure_list_widget->selectedItems();
    auto& figures = manager->getFigures();
    for (int i = 0; i < items.size(); i++)
    {
        int row = figure_list_widget->row(items.at(i));
        if (row < 0)
            continue;
        manager->deleteFigure(figures.at(row));
    }
    manager->saveOrder();
    refreshFigures();
}

void DialogueGroup::slotSaveToFile()
{
    QSettings st(QDir(data_dir).absoluteFilePath("settings.ini"), QSettings::IniFormat, this);
    QString r = st.value("recent/export_path", "").toString();

    QString path = QFileDialog::getSaveFileName(this, "保存对话", "对话",
                                                "长图片 (*.png *.jpg);;纯文本文件 (*.txt);;JSON文件（带样式表） (*.json)");
    if (path.isEmpty())
        return ;
    st.setValue("recent/export_path", path);

    if (path.endsWith(".png") || path.endsWith(".jpg")) // 导出图片
    {
        toPixmap().save(path);
    }
    else if (path.endsWith(".txt")) // 导出纯文本
    {
        DialogueManager::writeTextFile(path, toText());
    }
    else if (path.endsWith(".json")) // 导出JSON
    {

    }
}

void DialogueGroup::slotLoadFromFile()
{
    QString path = QFileDialog::getOpenFileName(this, "打开对话文件", "",
                                                tr("纯文本文件 (*.txt);;JSON文件（带样式表） (*.json)"));
    if (path.isEmpty())
        return ;

    // 先清理掉原来的（从头导入）
    dialogues_list_widget->clear();
    foreach (auto bucket, buckets)
        bucket->deleteLater();
    buckets.clear();

    if (path.endsWith(".txt")) // 纯文本
    {
        fromText(DialogueManager::readTextFile(path));
    }
    else
    {
//        fromJson();
    }
}

QListWidgetItem* DialogueGroup::addChat(DialogueBucket *bucket, int row)
{
    QListWidgetItem* item = new QListWidgetItem;
    if (row > -1)
    {
        buckets.insert(row, bucket);
        dialogues_list_widget->insertItem(row, item);
    }
    else
    {
        buckets.append(bucket);
        dialogues_list_widget->addItem(item);
    }
    dialogues_list_widget->setItemWidget(item, bucket);
    bucket->setMaximumWidth(dialogues_list_widget->width() - dialogues_list_widget->verticalScrollBar()->width());
    bucket->show();
    item->setSizeHint(bucket->size());
    connect(bucket, &DialogueBucket::signalBubbleChanged, this, [=]{
        item->setSizeHint(QSize(dialogues_list_widget->contentsRect().width() - dialogues_list_widget->verticalScrollBar()->width(), bucket->sizeHint().height()));
    });
    return item;
}
