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
    void setLevelDescriptionStyles();

    // Page indices in the QStackedWidget
    static constexpr int START_PAGE  = 0;
    static constexpr int LEVEL_PAGE  = 1;
    static constexpr int CODE_PAGE   = 2;
protected:
    // --------Mouse events, Qt calls these when user interacts with the window----------
    /**
     * When the mouse hovers over the corresponding level button,
     * the corresponding level description text should appear.
     * @brief eventFilter handles Qt mouse hover.
     * @param levelButton Buttons related to hover operations.
     * @param hoverEvent Content triggered when hovering over the correct position.
     * @return true if the event is handled, false to continue normal processing.
     */
    bool eventFilter(QObject *levelButton, QEvent *hoverEvent) override;

private slots:
    // --- Start Page ---
    /**
     * Navigates to level 1 directly (quick-start from the welcome page).
     * @brief on_startButton_clicked
     */
    void on_startButton_clicked();

    /**
     * Navigates to the level selection page.
     * @brief on_levelsButton_clicked
     */
    void on_levelsButton_clicked();

    /**
     * Shows an About dialog with game info.
     * @brief on_aboutButton_clicked
     */
    void on_aboutButton_clicked();

    /**
     * Returns to the start/welcome page from the level selection page.
     * @brief on_backToStartButton_clicked
     */
    void on_backToStartButton_clicked();

    // --- Level Selection Page ---
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

    // --- Game / Code Page ---
    /**
     * Returns to the level selection page from the game screen.
     * @brief on_backToLevelsButton_clicked
     */
    void on_backToLevelsButton_clicked();

    /**
     * Executes the player's block sequence against the current level.
     * @brief on_runButton_clicked
     */
    void on_runButton_clicked();

    /**
     * Clears the workspace and resets the game state.
     * @brief on_resetButton_clicked
     */
    void on_resetButton_clicked();

    /**
     * @brief on_hintButton_clicked
     */
    void on_hintButton_clicked();

    /**
     * @brief on_saveButton_clicked
     */
    void on_saveButton_clicked();
};
#endif // MAINWINDOW_H
