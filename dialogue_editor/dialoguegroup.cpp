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
    setAcceptDrops(true);

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
    figure_list_widget->setToolTip("角色模板\n【双】击或【ctrl+数字】添加至对话框\n通过最右边“保存角色”创建");

    connect(left_button, SIGNAL(clicked()), this, SLOT(slotAddLeftChat()));
    connect(mid_button, SIGNAL(clicked()), this, SLOT(slotAddNarrator()));
    connect(right_button, SIGNAL(clicked()), this, SLOT(slotAddRightChat()));

    connect(dialogues_list_widget, &QListWidget::currentRowChanged, this, [=](int row){
        if (_multi_adding) // 正在批量编辑，暂不进行操作
            return ;
        if (row == -1 || row >= dialogues_list_widget->count())
        {
            editor->setBucket(QList<DialogueBucket*>{}, nullptr);
            return ;
        }
        auto bucket = buckets[row];
        QList<DialogueBucket*> selecteds;
        if (dialogues_list_widget->selectedItems().size() == 1)
        {
            selecteds.append(bucket);
        }
        else
        {
            // 单选判断的机制不一样，获取到的是上一次单选的
            for (int i = 0; i < dialogues_list_widget->count(); i++)
                if (dialogues_list_widget->item(i)->isSelected())
                    selecteds.append(buckets[i]);
        }
        editor->setBucket(selecteds, bucket);
    });
    connect(dialogues_list_widget, &QListWidget::doubleClicked, this, [=](const QModelIndex& index) {
        // int row = index.row(); // 不用确定行数，肯定已经是当前行了
        editor->focusSaid();
    });
    connect(dialogues_list_widget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDialogueMenuShowed(QPoint)));

    connect(editor, SIGNAL(signalSaveFigure(DialogueBucket*)), this, SLOT(slotSaveFigure(DialogueBucket*)));
    connect(editor, SIGNAL(signalSaveToFile()), this, SLOT(slotSaveToFile()));

    connect(figure_list_widget, &QListWidget::doubleClicked, this, [=](const QModelIndex& index) {
        int row = index.row();
        auto figure = manager->getFigures().at(row);
        slotInsertFromFigure(figure);
        if (dialogues_list_widget->selectedItems().size() == 1)
            editor->focusSaid();
        else
            dialogues_list_widget->setFocus();
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

void DialogueGroup::beginMultiAdd()
{
    _multi_adding = true;

    editor->setBucket(QList<DialogueBucket*>{}, nullptr);
}

void DialogueGroup::endMultiAdd()
{
    _multi_adding = false;

    // 传递信号给editor
    int row = dialogues_list_widget->currentRow();
    if (row == -1 || row >= dialogues_list_widget->count())
    {
        editor->setBucket(QList<DialogueBucket*>{}, nullptr);
        return ;
    }
    auto bucket = buckets[row];
    QList<DialogueBucket*> selecteds;
    for (int i = 0; i < dialogues_list_widget->count(); i++)
        if (dialogues_list_widget->item(i)->isSelected())
            selecteds.append(buckets[i]);
    editor->setBucket(selecteds, bucket);
}

/**
 * 从文本导入
 * 多段导入可使用 beginMultiAdd，提高性能
 *
 * 格式1：  小明：“”
 * 格式2：  “”小明说，“”
 * 格式3：  “”小明说道。
 * 格式4：  【前缀】小明 2020.2.2  \n  说的话
 */
void DialogueGroup::fromText(QString full)
{
    QStringList paras = full.split("\n", QString::SkipEmptyParts);
    auto figures = manager->getFigures();
    DialogueFigure* prev_figure = nullptr; // 两段格式的话
    foreach (auto para, paras)
    {
        para = para.trimmed();
        QString judgeable_para = para; // 去掉可能误解的用来判断的para
        // 因为已经匹配的不会连续替换，就直接替换两次了
        judgeable_para.replace(QRegExp("(\\d+\\s?):(\\s?\\d+)"), "\\1\\2").replace(QRegExp("(\\d+\\s?):(\\s?\\d+)"), "\\1\\2");
        if (para.isEmpty())
            continue;

        if (para.indexOf("“") == -1 && judgeable_para.indexOf(":") == -1 && para.indexOf("：") == -1) // 是旁白
        {
            // 判断是不是上一段的人说的话
            if (prev_figure)
            {
                auto bucket = new DialogueBucket(prev_figure->type, prev_figure->nickname, prev_figure->avatar, para.trimmed(), this);
                bucket->setStyleSheet(prev_figure->qss);
                addChat(bucket);
                prev_figure = nullptr;
                continue;
            }

            // 判断有没有人名符合
            foreach (auto figure, figures)
            {
                if (figure->line_pattern.isEmpty())
                    continue;

                if (figure->line_reg.match(para).hasMatch())
                {
                    // 找到匹配的人，先保存figure，下一段就是他说的话
                    prev_figure = figure;
                    break;
                }
            }

            // 没有人名符合，是旁白
            if (prev_figure == nullptr)
            {
                auto bucket = new DialogueBucket(para, this);
                bucket->setStyleSheet(DialogueBucket::getDefaultNarratorStyleSheet());
                addChat(bucket);
            }
        }
        else // 是说话
        {
            QString name;
            // 使用正则表达式提取双引号里面的
            QRegularExpression quotes_reg("“.+?”");
            QRegularExpressionMatchIterator iterator = quotes_reg.globalMatch(para);
            QStringList saids;
            if (iterator.hasNext()) // 优先正则匹配
            {
                do {
                    auto match = iterator.next();
                    auto capd = match.captured();
                    saids.append(capd.mid(1, capd.length()-2)); // 去掉双引号
                } while (iterator.hasNext());

                // 去掉双引号及内部话语，只剩下引号外的内容
                /*para.replace(quotes_reg, "");
                name = para.replace(QRegExp("[，。！？：]+$"), "");*/
                QStringList outs = para.split(quotes_reg, QString::SkipEmptyParts);
                for (int i = 0; i < outs.size(); i++)
                {
                    outs[i].replace(QRegExp("[，。！？：]+$"), ""); // 去掉后面的标点
                    if (outs[i].isEmpty())
                        outs.removeAt(i--);
                }

                if (outs.size())
                {
                    // 先判断有没有角色模板
                    foreach (auto out, outs)
                    {
                        foreach (auto figure, figures)
                        {
                            if (figure->isNarrator())
                                continue;
                            if (out.indexOf(figure->nickname) != -1) // 包含这个名字，就是他了
                            {
                                name = figure->nickname;
                                break;
                            }
                            else if (!figure->name_pattern.isEmpty() && figure->name_reg.match(out).hasMatch())
                            {
                                name = figure->nickname;
                                break;
                            }
                        }
                        if (!name.isEmpty())
                            break;
                    }

                    // 没有找到角色模板
                    if (name.isEmpty())
                        name = outs[0];
                }
            }
            else if (para.indexOf("“") != -1) // 有前引号
            {
                int l_pos = para.indexOf("“");
                int r_pos = para.indexOf("”", l_pos+1);
                if (r_pos == -1) // 没有后引号，可能就是这种情况
                    r_pos = para.length();

                name = para.mid(0, l_pos).trimmed();
                QString said = para.mid(l_pos+1, r_pos-l_pos-1).trimmed();
                if (name.endsWith("：") || name.endsWith(":"))
                    name = name.left(name.length()-1);
                saids.append(said);
            }
            else // 只有冒号的
            {
                int c_pos= para.indexOf("：");
                if (c_pos == -1)
                    c_pos = para.indexOf(":");
                name = para.left(c_pos);
                QString said = para.right(para.length() - c_pos - 1);
                saids.append(said);
            }

            // 通过 figure 获取 bucket 信息
            ChatType type = (name=="我") ? ChatType::SelfChat : ChatType::OppoChat;
            QPixmap avatar = QPixmap(":/avatars/girl_1"); // TODO: 默认头像应该用个全局数值代替
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
            foreach (QString said, saids)
            {
                auto bucket = new DialogueBucket(type, name, avatar, said, this);
                bucket->setStyleSheet(style);
                addChat(bucket);
            }
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

void DialogueGroup::fromJson(QString text)
{

}

QString DialogueGroup::toJson()
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

void DialogueGroup::keyPressEvent(QKeyEvent *event)
{
    const Qt::Key key = (Qt::Key)event->key();
    const auto modifiers = event->modifiers();
    const auto ctrl = Qt::ControlModifier, shift = Qt::ShiftModifier, alt = Qt::AltModifier;
    const bool inDlg = dialogues_list_widget->hasFocus(), inFgr = figure_list_widget->hasFocus();
    auto sc = [=](Qt::KeyboardModifiers mdf, Qt::Key k) {
        return (key == k && modifiers == mdf);
    };

    if (sc(ctrl | alt, Qt::Key_Q))
        return slotAddLeftChat();
    else if (sc(ctrl | alt, Qt::Key_W))
        return slotAddNarrator();
    else if (sc(ctrl | alt, Qt::Key_E))
        return slotAddRightChat();
    else if (sc(ctrl | shift, Qt::Key_Q))
        return actionInsertLeftChat();
    else if (sc(ctrl | shift, Qt::Key_W))
        return actionInsertNarrator();
    else if (sc(ctrl | shift, Qt::Key_E))
        return actionInsertRightChat();
    else if (sc(ctrl, Qt::Key_Q))
        return actionInsertLeftChat(true);
    else if (sc(ctrl, Qt::Key_W))
        return actionInsertNarrator(true);
    else if (sc(ctrl, Qt::Key_E))
        return actionInsertRightChat(true);
    else if (inDlg && sc(ctrl, Qt::Key_R))
        return actionRenameChatNickname();
    else if (inDlg && sc(ctrl, Qt::Key_C))
        return actionCopyChat();
    else if (inDlg && sc(ctrl, Qt::Key_V))
        return actionPasteChat();
    else if (inDlg && sc(ctrl, Qt::Key_Left))
        return actionChatMoveUp();
    else if (inDlg && sc(ctrl, Qt::Key_Right))
        return actionChatMoveDown();
    else if (inDlg && sc(nullptr, Qt::Key_Delete))
        return actionChatDelete();
    else if (inFgr && sc(nullptr, Qt::Key_Enter))
        return actionInsertFigureDialogue();
    else if (inFgr && sc(nullptr, Qt::Key_Return))
        return actionInsertFigureDialogue();
    else if (inFgr && sc(nullptr, Qt::Key_Insert))
        return actionInsertFigureDialogue();
    else if (inFgr && sc(shift, Qt::Key_Enter))
        return actionSelectFigureDialogue();
    else if (inFgr && sc(shift, Qt::Key_Return))
        return actionSelectFigureDialogue();
    else if (inFgr && sc(ctrl, Qt::Key_U))
        return actionUpdateSelectedDialogues();
    else if (inFgr && sc(ctrl, Qt::Key_R))
        return actionRenameFigureAndDialogues();
    else if (inFgr && sc(ctrl, Qt::Key_Left))
        return actionFigureMoveUp();
    else if (inFgr && sc(ctrl, Qt::Key_Right))
        return actionFigureMoveDown();
    else if (inFgr && sc(nullptr, Qt::Key_Delete))
        return actionFigureDelete();
    else if (sc(ctrl, Qt::Key_O))
        return slotSaveToFile();
    else if (sc(ctrl, Qt::Key_I))
        return slotLoadFromFile();
    else if (sc(ctrl, Qt::Key_1))
        return actionInsertFigureDialogueByIndex(0);
    else if (sc(ctrl, Qt::Key_2))
        return actionInsertFigureDialogueByIndex(1);
    else if (sc(ctrl, Qt::Key_3))
        return actionInsertFigureDialogueByIndex(2);
    else if (sc(ctrl, Qt::Key_4))
        return actionInsertFigureDialogueByIndex(3);
    else if (sc(ctrl, Qt::Key_5))
        return actionInsertFigureDialogueByIndex(4);
    else if (sc(ctrl, Qt::Key_6))
        return actionInsertFigureDialogueByIndex(5);
    else if (sc(ctrl, Qt::Key_7))
        return actionInsertFigureDialogueByIndex(6);
    else if (sc(ctrl, Qt::Key_8))
        return actionInsertFigureDialogueByIndex(7);
    else if (sc(ctrl, Qt::Key_9))
        return actionInsertFigureDialogueByIndex(8);

    QWidget::keyPressEvent(event);
}

void DialogueGroup::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mime = event->mimeData();
    if (mime->hasText() || mime->hasUrls())
    {
        return event->accept();
    }

    QWidget::dragMoveEvent(event);
}

void DialogueGroup::dragMoveEvent(QDragMoveEvent *event)
{
    const QMimeData* mime = event->mimeData();
    if (mime->hasText() || mime->hasUrls())
    {
        if (dialogues_list_widget->geometry().contains(event->pos()))
        {
            return event->accept();
        }
    }

    QWidget::dragMoveEvent(event);
}

void DialogueGroup::dropEvent(QDropEvent *event)
{
    const QMimeData* mime = event->mimeData();
    if (mime->parent() == this)
        return QWidget::dropEvent(event); // 自己拖出去的，不要导入
    if (mime->hasText() || mime->hasUrls())
    {
        if (dialogues_list_widget->geometry().contains(event->pos()))
        {
            QString text;
            if (mime->hasText())
                text = mime->text();
            else if (mime->hasUrls())
            {
                QString url = mime->urls().first().toLocalFile();
                QFileInfo file(url);
                if (!file.exists() || file.isDir())
                    return QWidget::dropEvent(event);
                if (file.suffix() == "txt")
                    text = DialogueManager::readTextFile(url);
                else if (file.suffix() == "json")
                {
                    beginMultiAdd();
                    text = DialogueManager::readTextFile(url);
                    fromJson(text);
                    endMultiAdd();
                    return event->accept();
                }
                else
                    return QWidget::dropEvent(event);
            }
            beginMultiAdd();
            int old_count = dialogues_list_widget->count();
            fromText(text);
            dialogues_list_widget->clearSelection();
            for (int i = old_count; i < dialogues_list_widget->count(); i++)
                dialogues_list_widget->setCurrentRow(i, QItemSelectionModel::Select);
            endMultiAdd();

            return event->accept();
        }
    }


    QWidget::dropEvent(event);
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
    QAction* insert_left_action = new QAction("插入左边对话", this);
    QAction* insert_narr_action = new QAction("插入中间旁白", this);
    QAction* insert_right_action = new QAction("插入右边对话", this);
    QAction* rename_nickname_action = new QAction("重命名该角色", this);
    QAction* copy_chat_action = new QAction("复制为文本", this);
    QAction* paste_chat_action = new QAction("粘贴文本", this);
    QAction* move_up_action = new QAction("上移", this);
    QAction* move_down_action = new QAction("下移", this);
    QAction* delete_action = new QAction("删除", this);

    menu->addAction(insert_left_action);
    menu->addAction(insert_narr_action);
    menu->addAction(insert_right_action);
    menu->addSeparator();
    menu->addAction(rename_nickname_action);
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
    connect(rename_nickname_action, SIGNAL(triggered()), this, SLOT(actionRenameChatNickname()));
    connect(copy_chat_action, SIGNAL(triggered()), this, SLOT(actionCopyChat()));
    connect(paste_chat_action, SIGNAL(triggered()), this, SLOT(actionPasteChat()));
    connect(move_up_action, SIGNAL(triggered()), this, SLOT(actionChatMoveUp()));
    connect(move_down_action, SIGNAL(triggered()), this, SLOT(actionChatMoveDown()));
    connect(delete_action, SIGNAL(triggered()), this, SLOT(actionChatDelete()));

    if (dialogues_list_widget->selectedItems().size() == 0) // 没选
    {
        insert_left_action->setEnabled(false);
        insert_narr_action->setEnabled(false);
        insert_right_action->setEnabled(false);
        rename_nickname_action->setEnabled(false);
        copy_chat_action->setEnabled(false);
        move_up_action->setEnabled(false);
        move_down_action->setEnabled(false);
        delete_action->setEnabled(false);
    }
    else if (dialogues_list_widget->selectedItems().size() > 1) // 多选
    {
        rename_nickname_action->setEnabled(false);
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
    QAction* insert_dialogue_action = new QAction("添加TA的对话", this);
    QAction* select_all_dialogue_action = new QAction("选中TA的所有对话", this);
    QAction* figure_update_all_action = new QAction("更新TA的对话样式", this);
    QAction* figure_update_select_action = new QAction("将选中对话设为TA", this);
    QAction* name_reg_action = new QAction("名字匹配 表达式", this);
    QAction* line_reg_action = new QAction("整行匹配 表达式", this);
    QAction* figure_rename_action = new QAction("重命名", this);
    QAction* move_up_action = new QAction("上移", this);
    QAction* move_down_action = new QAction("下移", this);
    QAction* delete_action = new QAction("删除", this);

    menu->addAction(insert_dialogue_action);
    menu->addAction(select_all_dialogue_action);
    menu->addSeparator();
    menu->addAction(figure_update_all_action);
    menu->addAction(figure_update_select_action);
    menu->addSeparator();
    menu->addAction(name_reg_action);
    menu->addAction(line_reg_action);
    menu->addAction(figure_rename_action);
    menu->addSeparator();
    menu->addAction(move_up_action);
    menu->addAction(move_down_action);
    menu->addSeparator();
    menu->addAction(delete_action);

    connect(insert_dialogue_action, SIGNAL(triggered()), this, SLOT(actionInsertFigureDialogue()));
    connect(select_all_dialogue_action, SIGNAL(triggered()), this, SLOT(actionSelectFigureDialogue()));
    connect(figure_update_all_action, SIGNAL(triggered()), this, SLOT(actionUpdateFigureDialogues()));
    connect(figure_update_select_action, SIGNAL(triggered()), this, SLOT(actionUpdateSelectedDialogues()));
    connect(name_reg_action, SIGNAL(triggered()), this, SLOT(actionEditFigureLineReg()));
    connect(line_reg_action, SIGNAL(triggered()), this, SLOT(actionEditFigureNameReg()));
    connect(figure_rename_action, SIGNAL(triggered()), this, SLOT(actionRenameFigureAndDialogues()));
    connect(move_up_action, SIGNAL(triggered()), this, SLOT(actionFigureMoveUp()));
    connect(move_down_action, SIGNAL(triggered()), this, SLOT(actionFigureMoveDown()));
    connect(delete_action, SIGNAL(triggered()), this, SLOT(actionFigureDelete()));

    if (figure_list_widget->selectedItems().size() == 0) // 没有选中
    {
        insert_dialogue_action->setEnabled(false);
        select_all_dialogue_action->setEnabled(false);
        figure_update_all_action->setEnabled(false);
        figure_update_select_action->setEnabled(false);
        figure_rename_action->setEnabled(false);
        name_reg_action->setEnabled(false);
        line_reg_action->setEnabled(false);
        move_up_action->setEnabled(false);
        move_down_action->setEnabled(false);
        delete_action->setEnabled(false);
    }
    else if (figure_list_widget->selectedItems().size() > 1) // 多选
    {
        insert_dialogue_action->setEnabled(false);
        figure_update_select_action->setEnabled(false);
        figure_rename_action->setEnabled(false);
        name_reg_action->setEnabled(false);
        line_reg_action->setEnabled(false);
    }
    else // 单选
    {
        auto figure = manager->getFigures().at(figure_list_widget->currentRow());
        if (figure->isNarrator()) // 是旁白
        {
            figure_rename_action->setEnabled(false);
            line_reg_action->setEnabled(false);
            name_reg_action->setEnabled(false);
        }
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

void DialogueGroup::insertBucketAndSetQSS(QListWidgetItem* item, DialogueBucket *bucket, QString qss, bool next)
{
    int row = (item == nullptr ? -1 : dialogues_list_widget->row(item));
    insertBucketAndSetQSS(row, bucket, qss, next);
}

void DialogueGroup::insertBucketAndSetQSS(int row, DialogueBucket *bucket, QString qss, bool next)
{
    if (!qss.isEmpty())
        bucket->setStyleSheet(qss);
    if (next)
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
        QString name = figure->nickname;
        if (i < 9)
            name = "[" + QString::number(i+1) + "] " + name;
        QListWidgetItem* item = new QListWidgetItem(figure->avatar, name);
        figure_list_widget->addItem(item);
    }
}

/**
 * 横向拖动对话
 */
void DialogueGroup::slotBucketDragOutEvent(QPoint press_pos)
{
    // 获取导出图片的高度
    int total_height = 0;
    QList<DialogueBucket*> selected_buckets;
    for (int i = 0; i < dialogues_list_widget->count(); i++)
    {
        if (dialogues_list_widget->item(i)->isSelected())
        {
            auto bucket = buckets.at(i);
            selected_buckets.append(bucket);
            total_height += bucket->height();
        }
    }
    QPixmap pixmap(QSize(dialogues_list_widget->contentsRect().width()-dialogues_list_widget->verticalScrollBar()->width(), total_height));
    QColor bg_color = dialogues_list_widget->palette().window().color();
    pixmap.fill(Qt::white);

    // 绘制到图片
    int current_height = 0;
    foreach (auto bucket, selected_buckets)
    {
        bucket->render(&pixmap, QPoint(0, current_height), QRect(0,0,bucket->width(),bucket->height()), QWidget::DrawChildren);
        current_height += bucket->height()+8;
    }

    // 设置 Mime
    QMimeData* mime = new QMimeData;
    mime->setParent(this);

    // mime文字
    mime->setText(toText(selected_buckets));

    // mime图片
    mime->setImageData(pixmap);

    // mime文件
    QString path = data_dir + "/对话.png";
    pixmap.save(path);
    mime->setUrls(QList<QUrl>{QUrl::fromLocalFile(path)});

    // 开始拖拽
    QPixmap drag_pixmap(QSize(dialogues_list_widget->contentsRect().width()-dialogues_list_widget->verticalScrollBar()->width(), dialogues_list_widget->contentsRect().height()));
    dialogues_list_widget->render(&drag_pixmap);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mime);
    drag->setPixmap(drag_pixmap);
    drag->setHotSpot(mapFromGlobal(QCursor::pos()) - dialogues_list_widget->pos());
    connect(drag, &QDrag::destroyed, this, [=](QObject*){
        QTimer::singleShot(10000, [=]{
            QFile file(path);
            if(file.exists())
                file.remove();
        });
    });
    drag->exec();
}

void DialogueGroup::actionInsertLeftChat(bool next)
{
    beginMultiAdd();
    auto items = dialogues_list_widget->selectedItems();
    if (items.size() == 0)
        items = QList<QListWidgetItem*>{nullptr};
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        insertBucketAndSetQSS(item,
                              new DialogueBucket(OppoChat, "名字", QPixmap(":/avatars/girl_1"), "说的话", this),
                              DialogueBucket::getDefaultChatStyleSheet(), next);
    }
    if (items.size() == 1)
        editor->focusSaid();
    endMultiAdd();
}

void DialogueGroup::actionInsertNarrator(bool next)
{
    beginMultiAdd();
    auto items = dialogues_list_widget->selectedItems();
    if (items.size() == 0)
        items = QList<QListWidgetItem*>{nullptr};
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        insertBucketAndSetQSS(item, new DialogueBucket("旁白", this),
                              DialogueBucket::getDefaultNarratorStyleSheet(), next);
    }
    if (items.size() == 1)
        editor->focusSaid();
    endMultiAdd();
}

void DialogueGroup::actionInsertRightChat(bool next)
{
    beginMultiAdd();
    auto items = dialogues_list_widget->selectedItems();
    if (items.size() == 0)
        items = QList<QListWidgetItem*>{nullptr};
    dialogues_list_widget->clearSelection();
    foreach (auto item, items)
    {
        auto bucket = new DialogueBucket(SelfChat, "我", QPixmap(":/avatars/boy_1"), "说的话", this);
        bucket->setNameVisible(false);
        insertBucketAndSetQSS(item, bucket,
                              DialogueBucket::getDefaultChatStyleSheet(), next);
    }
    if (items.size() == 1)
        editor->focusSaid();
    endMultiAdd();
}

/**
 * 重命名对话的人名
 * 暂时不包括角色模板名字？（不知道要不要加入这个效果）
 */
void DialogueGroup::actionRenameChatNickname()
{
    auto bucket = buckets.at(dialogues_list_widget->currentRow());
    if (bucket == nullptr || bucket->isNarrator())
        return ;
    QString old_name = bucket->getName();

    // 输入姓名
    QString new_name = QInputDialog::getText(this, "重命名对话人名", "请输入新名字\n如果要同步修改角色名，请改用左边角色模板的重命名", QLineEdit::Normal, old_name);
    if (new_name.isEmpty())
        return ;

    beginMultiAdd();
    // 开始重命名对话框同名
    dialogues_list_widget->clearSelection();
    for (int i = 0; i < buckets.size(); i++)
    {
        auto bucket = buckets.at(i);
        if (bucket->isNarrator())
            continue;
        if (bucket->getName() == old_name)
        {
            bucket->setName(new_name);
            bucket->update();
            dialogues_list_widget->setCurrentRow(i, QItemSelectionModel::Select);
        }
    }
    dialogues_list_widget->setFocus(); // 要 setFocus 才会 实时更新
    endMultiAdd();
}

void DialogueGroup::actionCopyChat()
{
    QList<DialogueBucket*>widgets;
    for (int i = 0; i < dialogues_list_widget->count(); i++)
    {
        if (!dialogues_list_widget->item(i)->isSelected())
            continue;
        auto widget = buckets.at(i);
        widgets.append(widget);
    }
    if (widgets.size())
        QApplication::clipboard()->setText(toText(widgets));
}

void DialogueGroup::actionPasteChat()
{
    QString s = QApplication::clipboard()->text();
    if (s.isEmpty())
        return ;
    beginMultiAdd();
    int old_count = dialogues_list_widget->count();
    fromText(s);
    dialogues_list_widget->clearSelection();
    for (int i = old_count; i < dialogues_list_widget->count(); i++)
        dialogues_list_widget->setCurrentRow(i, QItemSelectionModel::Select);
    endMultiAdd();
}

void DialogueGroup::actionChatMoveUp()
{
    beginMultiAdd();
    bool above_moved = false; // 上面选中的是否上移了（排除第一行开始就一直选中的情况）
    for (int row = 0; row < dialogues_list_widget->count(); row++)
    {
        if (dialogues_list_widget->item(row)->isSelected())
        {
            if (!above_moved) // 此行及上面全都选中，没法上移
                continue;
            else
            {
                auto item = dialogues_list_widget->takeItem(row);
                auto widget = buckets.takeAt(row);
                insertBucketAndSetQSS(row-1, new DialogueBucket(widget), widget->styleSheet());
                delete item;
                delete widget;
            }
        }
        else
        {
            above_moved = true;
        }
    }
    endMultiAdd();
}

void DialogueGroup::actionChatMoveDown()
{
    beginMultiAdd();
    bool follow_moved = false; // 下面选中的是否下移了（排除最后一行开始就一直选中的情况）
    for (int row = dialogues_list_widget->count()-1; row >= 0; row--)
    {
        if (dialogues_list_widget->item(row)->isSelected())
        {
            if (!follow_moved) // 此行及上面全都选中，没法上移
                continue;
            else
            {
                auto item = dialogues_list_widget->takeItem(row);
                auto widget = buckets.takeAt(row);
                insertBucketAndSetQSS(row+1, new DialogueBucket(widget), widget->styleSheet());
                delete item;
                delete widget;
            }
        }
        else
        {
            follow_moved = true;
        }
    }
    endMultiAdd();
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

/**
 * 重命名窗口以及对话框中重名的对话
 */
void DialogueGroup::actionRenameFigureAndDialogues()
{
    auto figure = manager->getFigures().at(figure_list_widget->currentRow());
    if (figure->isNarrator()) // 旁白不能重命名
        return ;

    // 输入姓名
    QString new_name = QInputDialog::getText(this, "重命名角色", "请输入新名字\n将更新至该角色模板，及其所有对话", QLineEdit::Normal, figure->nickname);
    if (new_name.isEmpty() || new_name == figure->nickname)
        return ;
    if (manager->getFigureByName(new_name) != nullptr)
    {
        if (QMessageBox::warning(this, "提示", QString("已经存在【%1】的角色模板，是否依旧使用此名字？").arg(new_name), "放弃", "继续", nullptr, 0) != 1)
            return ;
    }
    // 开始重命名
    QString old_name = figure->nickname;
    figure->nickname = new_name;
    manager->saveData(figure);
    dialogues_list_widget->clearSelection();
    for (int i = 0; i < buckets.size(); i++)
    {
        auto bucket = buckets.at(i);
        if (bucket->isNarrator())
            continue;
        if (bucket->getName() == old_name)
        {
            bucket->setName(new_name);
            bucket->update();
            dialogues_list_widget->setCurrentRow(i, QItemSelectionModel::Select);
        }
    }
    refreshFigures();
    dialogues_list_widget->setFocus(); // 要 setFocus 才会 实时更新
}

void DialogueGroup::actionEditFigureNameReg()
{
    auto figure = manager->getFigures().at(figure_list_widget->currentRow());
    QString old_reg = figure->line_pattern;
    bool ok;
    QString new_reg = QInputDialog::getText(this, "角色匹配表达式", "匹配该角色的正则表达式\n含双引号的段落中出现时，本段作为TA说的话", QLineEdit::Normal, old_reg, &ok);
    if (!ok)
        return ;
    figure->setNameReg(new_reg);

    manager->saveData(figure);
}

void DialogueGroup::actionEditFigureLineReg()
{
    auto figure = manager->getFigures().at(figure_list_widget->currentRow());
    QString old_reg = figure->line_pattern;
    bool ok;
    QString new_reg = QInputDialog::getText(this, "角色行匹配表达式", "匹配该角色的正则表达式\n不含双引号的段落中出现时，下一行作为TA说的话\n可用来从 QQ/微信/TIM 中粘贴消息", QLineEdit::Normal, old_reg, &ok);
    if (!ok)
        return ;
    figure->setLineReg(new_reg);

    manager->saveData(figure);
}

void DialogueGroup::actionFigureMoveUp()
{
    QList<int>selected_rows; // 保存之前的选中
    auto& figures = manager->getFigures();
    bool above_moved = false; // 上面选中的是否上移了（排除第一行开始就一直选中的情况）
    for (int row = 0; row < figure_list_widget->count(); row++)
    {
        if (figure_list_widget->item(row)->isSelected())
        {
            if (!above_moved) // 此行及上面全都选中，没法上移
            {
                selected_rows.append(row);
                continue;
            }
            else
            {
                auto figure = figures.takeAt(row);
                figures.insert(row-1, figure);
                selected_rows.append(row-1);
            }
        }
        else
        {
            above_moved = true;
        }
    }
    manager->saveOrder();

    refreshFigures();

    // 恢复之前选中的
    for (int i = 0; i < selected_rows.count(); i++)
    {
        figure_list_widget->setCurrentRow(selected_rows.at(i), QItemSelectionModel::Select);
    }
}

void DialogueGroup::actionFigureMoveDown()
{
    QList<int>selected_rows; // 保存之前的选中
    auto& figures = manager->getFigures();
    bool follow_moved = false; // 下面选中的是否下移了（排除最后一行开始就一直选中的情况）
    for (int row = figure_list_widget->count()-1; row >= 0 ; row--)
    {
        if (figure_list_widget->item(row)->isSelected())
        {
            if (!follow_moved) // 此行及上面全都选中，没法上移
            {
                selected_rows.append(row);
                continue;
            }
            else
            {
                auto figure = figures.takeAt(row);
                figures.insert(row+1, figure);
                selected_rows.append(row+1);
            }
        }
        else
        {
            follow_moved = true;
        }
    }
    manager->saveOrder();

    refreshFigures();

    // 恢复之前选中的
    for (int i = 0; i < selected_rows.count(); i++)
    {
        figure_list_widget->setCurrentRow(selected_rows.at(i), QItemSelectionModel::Select);
    }
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

void DialogueGroup::actionInsertFigureDialogueByIndex(int index)
{
    if (index < 0 || index >= manager->getFigures().size())
        return ;
    slotInsertFromFigure(manager->getFigures().at(index));
    editor->focusSaid();
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

    beginMultiAdd();
    // 先清理掉原来的（从头导入）
    dialogues_list_widget->clear();
    foreach (auto bucket, buckets)
        bucket->deleteLater();
    buckets.clear();

    if (path.endsWith(".txt")) // 纯文本
    {
        fromText(DialogueManager::readTextFile(path));
    }
    else if (path.endsWith(".json")) // json 文件
    {
        fromJson(DialogueManager::readTextFile(path));
    }
    endMultiAdd();
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
        update();
    });
    connect(bucket, SIGNAL(signalDragOutEvent(QPoint)), this, SLOT(slotBucketDragOutEvent(QPoint)));
    return item;
}
