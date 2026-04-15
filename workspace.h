#include <QWidget>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>

class Workspace : public QWidget {
    Q_OBJECT

public:
    Workspace(QWidget *parent = nullptr);
    void clearBlocks();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QVBoxLayout *layout;
};
