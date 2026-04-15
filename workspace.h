#include <QWidget>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "codeBlock.h"

class Workspace : public QWidget {
    Q_OBJECT

public:
    Workspace(QWidget *parent = nullptr);
    void clearBlocks();
    QList<CodeBlock*> getBlocks();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
private:
    QVBoxLayout *layout;
    QList<CodeBlock*> blocks;
};
