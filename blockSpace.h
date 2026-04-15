#ifndef BLOCKSPACE_H
#define BLOCKSPACE_H

#include <QWidget>
#include <QVBoxLayout>

class BlockSpace : public QWidget
{
    Q_OBJECT

public:
    BlockSpace(QWidget *parent = nullptr);

    void addBlock(const QString &text, const QString &type);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QVBoxLayout *layout;
};

#endif // BLOCKSPACE_H
