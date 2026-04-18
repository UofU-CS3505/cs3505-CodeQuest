#ifndef BLOCKSPACE_H
#define BLOCKSPACE_H

#include <QWidget>
#include <QVBoxLayout>
#include "codeBlock.h"

class BlockSpace : public QWidget
{
    Q_OBJECT

public:
    BlockSpace(QWidget *parent = nullptr);

    void addBlock(CodeBlock *block);
    QVBoxLayout* getLayout();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QVBoxLayout *layout;
};

#endif // BLOCKSPACE_H
