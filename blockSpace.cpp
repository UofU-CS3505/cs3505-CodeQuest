#include "blockSpace.h"
#include "workspace.h"
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

void BlockSpace::addBlock(CodeBlock *block)
{
    block->setParent(this);
    layout->addWidget(block);
    block->show();
}

QVBoxLayout* BlockSpace::getLayout() {
    return layout;
}

void BlockSpace::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-codeblock")) {
        event->acceptProposedAction();
    }
}

void BlockSpace::dropEvent(QDropEvent *event)
{
    CodeBlock *block = qobject_cast<CodeBlock*>(event->source());

    if (block && block->parentWidget()) {
        Workspace *workspace = qobject_cast<Workspace*>(block->parentWidget());

        if (workspace) {
            workspace->removeBlock(block);
            delete block;
        }
    }

    event->acceptProposedAction();
}
