#include <QWidget>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>

#pragma once

class CodeBlock;

class Workspace : public QWidget {
    Q_OBJECT

public:
    explicit Workspace(QWidget *parent = nullptr);

signals:
    void moveBlockClicked(CodeBlock *block);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QVBoxLayout *layout;
    void registerBlock(CodeBlock *block);
};
