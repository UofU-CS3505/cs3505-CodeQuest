#include <QWidget>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include "codeBlock.h"

#pragma once

class CodeBlock;

class Workspace : public QWidget {
    Q_OBJECT

public:
    Workspace(QWidget *parent = nullptr);
    void clearBlocks();
    QList<CodeBlock*> getBlocks();

    void removeBlock(CodeBlock *block);
    void insertRoutePointAfterMove(CodeBlock *moveBlock, CodeBlock *routePointBlock);
    void reindentAll();

signals:
    void moveBlockClicked(CodeBlock *block);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
private:
    QVBoxLayout *layout;
    void registerBlock(CodeBlock *block);
    QList<CodeBlock*> blocks;
};
