#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void setLevelButtonStyles();

protected:

private slots:
    /**
     * A button that directs users to the tutorial and challenge for Level 1.
     * @brief on_levelOneButton_clicked handles level 1 button
     */
    void on_levelOneButton_clicked();

    /**
     * A button that directs users to the tutorial and challenge for Level 2.
     * @brief on_levelTwoButton_clicked handles level 2 button
     */
    void on_levelTwoButton_clicked();

    /**
     * A button that directs users to the tutorial and challenge for Level 3.
     * @brief on_levelThreeButton_clicked handles level 3 button
     */
    void on_levelThreeButton_clicked();

    /**
     * A button that directs users to the tutorial and challenge for Level 4.
     * @brief on_levelFourButton_clicked handles level 4 button
     */
    void on_levelFourButton_clicked();

    /**
     * A button that directs users to the tutorial and challenge for Level 5.
     * @brief on_levelFiveButton_clicked handles level 5 button.
     */
    void on_levelFiveButton_clicked();
};
#endif // MAINWINDOW_H
