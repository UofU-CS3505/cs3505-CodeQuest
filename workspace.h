#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QWidget>

class QVBoxLayout;

class Workspace : public QWidget {
    Q_OBJECT

public:
    Workspace(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QVBoxLayout *layout;
};

#endif
