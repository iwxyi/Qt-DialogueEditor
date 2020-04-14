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

    dialogues_list_widget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    dialogues_list_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    dialogues_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    dialogues_list_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    figure_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    figure_list_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
    connect(dialogues_list_widget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotListMenuShowed(QPoint)));

    connect(editor, SIGNAL(signalSaveFigure(DialogueBucket*)), this, SLOT(slotSaveFigure(DialogueBucket*)));
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
    manager->setDataDir(dir);
    manager->loadData();
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

void DialogueGroup::slotListMenuShowed(QPoint)
{
    if (!dialogues_list_widget->currentIndex().isValid())
        return ;
    QMenu* menu = new QMenu("菜单");
    QAction* insert_left_action = new QAction("插入左边", this);
    QAction* insert_narr_action = new QAction("插入旁白", this);
    QAction* insert_right_action = new QAction("插入右边", this);
    QAction* move_up_action = new QAction("上移", this);
    QAction* move_down_action = new QAction("下移", this);
    QAction* delete_action = new QAction("删除", this);

    menu->addAction(insert_left_action);
    menu->addAction(insert_narr_action);
    menu->addAction(insert_right_action);
    menu->addSeparator();
    menu->addAction(move_up_action);
    menu->addAction(move_down_action);
    menu->addSeparator();
    menu->addAction(delete_action);

    connect(insert_left_action, SIGNAL(triggered()), this, SLOT(actionInsertLeftChat()));
    connect(insert_narr_action, SIGNAL(triggered()), this, SLOT(actionInsertNarrator()));
    connect(insert_right_action, SIGNAL(triggered()), this, SLOT(actionInsertRightChat()));
    connect(move_up_action, SIGNAL(triggered()), this, SLOT(actionChatMoveUp()));
    connect(move_down_action, SIGNAL(triggered()), this, SLOT(actionChatMoveDown()));
    connect(delete_action, SIGNAL(triggered()), this, SLOT(actionChatDelete()));

    menu->exec(QCursor::pos());

    menu->deleteLater();
}

void DialogueGroup::slotSaveFigure(DialogueBucket *bucket)
{
    manager->saveFigure(bucket);
    refreshFigures();
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
        int row = dialogues_list_widget->row(item);
        auto bucket = new DialogueBucket(OppoChat, "名字", QPixmap(":/avatars/girl_1"), "说的话", this);
        bucket->setStyleSheet(DialogueBucket::getDefaultChatStyleSheet());
        auto itm = addChat(bucket, row);
        dialogues_list_widget->setCurrentItem(itm);
    }
}

void DialogueGroup::actionInsertNarrator()
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        int row = dialogues_list_widget->row(item);
        auto bucket = new DialogueBucket("旁白", this);
        bucket->setStyleSheet(DialogueBucket::getDefaultNarratorStyleSheet());
        auto itm = addChat(bucket, row);
        dialogues_list_widget->setCurrentItem(itm);
    }
}

void DialogueGroup::actionInsertRightChat()
{
    auto items = dialogues_list_widget->selectedItems();
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        int row = dialogues_list_widget->row(item);
        auto bucket = new DialogueBucket(SelfChat, "我", QPixmap(":/avatars/boy_1"), "说的话", this);
        bucket->setStyleSheet(DialogueBucket::getDefaultChatStyleSheet());
        bucket->setNameVisible(false);
        auto itm = addChat(bucket, row);
        dialogues_list_widget->setCurrentItem(itm);
    }
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
        auto widget = static_cast<DialogueBucket*>(dialogues_list_widget->itemWidget(item));
        dialogues_list_widget->insertItem(row-1, item);
        dialogues_list_widget->setItemWidget(item, widget);

        buckets.takeAt(row);
        buckets.insert(row-1, widget);
    }
}

void DialogueGroup::actionChatMoveDown()
{
    auto items = dialogues_list_widget->selectedItems();
    for (int i = items.size()-1; i >= 0; i--) // 倒着来
    {
        int row = dialogues_list_widget->row(items.at(i));
        if (row >= dialogues_list_widget->count()-1)
            continue;
        auto item = dialogues_list_widget->takeItem(row);
        auto widget = static_cast<DialogueBucket*>(dialogues_list_widget->itemWidget(item));
        dialogues_list_widget->insertItem(row-1, item);
        dialogues_list_widget->setItemWidget(item, widget);

        buckets.takeAt(row);
        buckets.insert(row+1, widget);
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
