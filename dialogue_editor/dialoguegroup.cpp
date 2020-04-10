#include "dialoguegroup.h"

DialogueGroup::DialogueGroup(QWidget *parent) : QWidget(parent)
{
    initView();
    initStyle();
    initData();
}

void DialogueGroup::initView()
{
    dialogues_list_widget = new QListWidget(this);
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

    QHBoxLayout* main_hlayout = new QHBoxLayout(this);
    editor = new DialogueEditor(this);
    main_hlayout->addLayout(list_vlayout);
    main_hlayout->addWidget(editor);

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
}

void DialogueGroup::initStyle()
{

}

void DialogueGroup::initData()
{

}

void DialogueGroup::slotAddLeftChat()
{
    auto bucket = new DialogueBucket(OppoChat, "名字", QPixmap(":/avatars/girl_1"), "说的话", this);
    bucket->setStyleSheet(DialogueBucket::getDefaultChatStyleSheet());
    addChat(bucket);
}

void DialogueGroup::slotAddNarrator()
{
    auto bucket = new DialogueBucket("旁白", this);
    bucket->setStyleSheet(DialogueBucket::getDefaultNarratorStyleSheet());
    addChat(bucket);
}

void DialogueGroup::slotAddRightChat()
{
    auto bucket = new DialogueBucket(SelfChat, "我", QPixmap(":/avatars/boy_1"), "说的话", this);
    bucket->setStyleSheet(DialogueBucket::getDefaultChatStyleSheet());
    bucket->setNameVisible(false);
    addChat(bucket);
}

void DialogueGroup::addChat(DialogueBucket *bucket, int row)
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
    item->setSizeHint(bucket->size());
    bucket->show();
}
