#include "blockSpace.h"
#include "codeBlock.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

BlockSpace::BlockSpace(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setStyleSheet("background-color: #eeeeee;");

    layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);
}

void BlockSpace::addBlock(const QString &text, const QString &type)
{
    CodeBlock *block = new CodeBlock(text, type);
    layout->addWidget(block);
}

void BlockSpace::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-codeblock")) {
        event->acceptProposedAction();
    }
}

void BlockSpace::dropEvent(QDropEvent *event)
{
    // This is your DELETE ZONE logic (workspace → palette = delete)

    CodeBlock *block = qobject_cast<CodeBlock*>(event->source());

    if (block && block->parentWidget()) {

        // only delete blocks that came from workspace
        if (block->parentWidget()->inherits("Workspace")) {
            delete block;
        }
    }

    event->acceptProposedAction();
}
