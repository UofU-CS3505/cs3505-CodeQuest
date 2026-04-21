#include "workspace.h"
#include <QMimeData>

Workspace::Workspace(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setStyleSheet("background-color: lightgray;");

    layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(6);
    setLayout(layout);
}

void Workspace::clearBlocks()
{
    QLayoutItem *item;

    while ((item = layout->takeAt(0)) != nullptr) {
        QWidget *widget = item->widget();

        if (widget) {
            blocks.removeAll(static_cast<CodeBlock*>(widget));
            delete widget;
        }

        delete item;
    }

    blocks.clear();
}

QList<CodeBlock*> Workspace::getBlocks()
{
    return blocks;
}

void Workspace::removeBlock(CodeBlock *block)
{
    if (!block) return;

    layout->removeWidget(block);
    blocks.removeAll(block);
    reindentAll();
}

void Workspace::insertRoutePointAfterMove(CodeBlock *moveBlock, CodeBlock *routePointBlock)
{
    if (!moveBlock || !routePointBlock) return;

    int moveIndex = blocks.indexOf(moveBlock);
    if (moveIndex == -1) return;

    int insertIndex = moveIndex + 1;

    while (insertIndex < blocks.size() &&
           blocks[insertIndex] &&
           blocks[insertIndex]->getType() == "routePoint") {
        insertIndex++;
    }

    routePointBlock->setParent(this);
    registerBlock(routePointBlock);

    blocks.insert(insertIndex, routePointBlock);
    layout->insertWidget(insertIndex, routePointBlock);
    routePointBlock->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    routePointBlock->show();
    reindentAll();
}

void Workspace::reindentAll()
{
    const int NORMAL_WIDTH  = 180;
    const int BODY_WIDTH    = 150;  // narrower inside {}

    int depth = 0;
    for (CodeBlock *b : blocks) {
        if (!b) continue;
        QString type = b->getType();
        QString text = b->text();

        if (type == "scopeBracket" && text == "}") {
            if (depth > 0) depth--;
        }

        if (type == "scopeBracket") {
            // { and } keep their own small fixed size set in codeBlock.cpp
            b->setMaximumWidth(60);
            b->setMinimumWidth(40);
        } else {
            int w = (depth > 0) ? BODY_WIDTH : NORMAL_WIDTH;
            b->setFixedWidth(w);
        }

        if (type == "scopeBracket" && text == "{") {
            depth++;
        }
    }
}

void Workspace::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-codeblock")) {
        event->acceptProposedAction();
    }
}

void Workspace::registerBlock(CodeBlock *block)
{
    connect(block, &CodeBlock::blockClicked,
            this,
            [this](CodeBlock *clickedBlock) {
                if (clickedBlock->getType() == "move") {
                    emit moveBlockClicked(clickedBlock);
                }
            });
}

// This method was generated using AI
void Workspace::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat("application/x-codeblock"))
        return;

    QString text = event->mimeData()->text();
    QString type = QString::fromUtf8(event->mimeData()->data("application/x-codeblock"));

    CodeBlock *sourceBlock = qobject_cast<CodeBlock*>(event->source());

    CodeBlock *block = nullptr;

    if (sourceBlock && sourceBlock->parentWidget() == this) {
        block = sourceBlock;

        layout->removeWidget(block);
        blocks.removeAll(block);
    }

    else {
        block = new CodeBlock(text, type);
        registerBlock(block);
    }

    int insertIndex = layout->count();
    QPoint dropPosition = event->position().toPoint();

    for (int i = 0; i < layout->count(); i++) {
        QWidget *w = layout->itemAt(i)->widget();
        if (!w) continue;

        int midY = w->y() + w->height() / 2;

        if (dropPosition.y() < midY) {
            insertIndex = i;
            break;
        }
    }

    blocks.insert(insertIndex, block);
    layout->insertWidget(insertIndex, block);
    block->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    event->acceptProposedAction();

    reindentAll();
}
