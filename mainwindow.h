#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QPushButton>
#include "savefile.h"
#include <Box2D/Box2D.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class CodeBlock;

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
    void setRoutePointsVisible(bool visible);
    void setNormalPaletteBlocksVisible(bool visible);
    void enterRoutePointSelectionMode(CodeBlock *moveBlock);
    void exitRoutePointSelectionMode();
    void createRoutePointBlocks();
    bool running;
    bool playerTriggeredMonster = false;


    QVector<CodeBlock*> routePointBlocks;
    QVector<CodeBlock*> normalPaletteBlocks;
    QStringList currentLevelRoutePoints;

    // Page indices in the QStackedWidget
    static constexpr int START_PAGE  = 0;
    static constexpr int LEVEL_PAGE  = 1;
    static constexpr int CODE_PAGE   = 2;

    bool routePointsVisible = false;
    bool routePointSelectionMode = false;
    CodeBlock *selectedMoveBlock = nullptr;

    QString normalButtonStyle;

    SaveFile saveFile;
    int currentLevel = 1;

    QMap<QString, QPoint> levelPointPositions;

    QString startPoint = "start";
    QString goalPoint;
    const QString GOAL_BLOCK = "moveToCastle();";

    QTimer *moveTimer = nullptr;
    QStringList activeRoute;
    double playerSpeed = 2.5;

    QStringList getProgramRoute() const;
    void resetPrincessToStart();
    void startPlayerMovement(const QStringList &route);
    void stepPlayerMovement();

    // Lock and Unlock Levels
    void unlockLevelButtons();
    void openSpecificLevel(int levelNumber);
    void completeCurrentLevel();
    void failCurrentLevel();

    // Canvas setup
    QLabel *princess;
    QLabel *dragon1;
    QLabel *dragon2;
    QLabel *dragonAlertZone = nullptr;
    QLabel *guard = nullptr;
    QLabel *guardZone = nullptr;
    QTimer *dragonTimer;
    QVector<QLabel*> walls;
    static const int TILE = 40;
    static const int COLS = 12;
    static const int ROWS = 12;
    static const int DRAGON_MOVE_TOP_Y = 80;
    static const int DRAGON_MOVE_BOTTOM_Y = 280;


    // -1 is up, 1 is down.
    int dragon1Direction = -1;
    int dragon2Direction = -1;

    bool gameStarted;
    bool dragonMovementEnabled;

    void setUpLevels();
    void setUpLevel1();
    void setUpLevel3();
    void setUpLevel4();
    void addWall(int x, int y);
    void movePrincess(int dx, int dy);
    void setUpDragons();
    void moveDragons(int dy);
    void dragonStartMove();
    void showDragonAlertZone();
    void resetDragonsToStart();
    bool won();

    void applyDarkTheme();
    void clearLevel1Visuals();
    void addPointVisual(const QString &name, const QPoint &pos);
    bool isTouchingDragon() const;
    void handlePlayerDeath();

    void setRunning(bool isRunning);
    void dimButtons(bool gameStarted);

    // box2d jiggle for menu buttons
    struct ButtonJiggle {
        QPushButton *button = nullptr;
        QPoint       origin;
        b2Body      *body   = nullptr;
    };
    QVector<ButtonJiggle> jiggleBodies;
    b2World               jiggleWorld{b2Vec2(0.f, 0.f)};  // no gravity, spring only
    QTimer               *jiggleTimer = nullptr;
    void initJiggle();
    void stepJiggle();

    //for level2
    void setUpGuard();
    void resetLevel2();
    void updateGuardCheck();
    void updateIfLevelLogic();
    void setUpLevel2();
    void clearLevel2Visuals();

    // for level3
    int doorHitCount = 0;
    static const int DOOR_HITS_NEEDED = 2;
    QLabel *door = nullptr;
    QVector<QLabel*> lockLabels;   // locks above A, disappear one by one on each A visit
    QVector<QLabel*> keyLabels;    // keys above B, one picked up on each B visit
    QLabel *carriedKey = nullptr;  // the key currently following the princess
    void clearLevel3Visuals();
    void resetLevel3();
    QStringList expandForLoops(const QStringList &raw) const;

    // for get the level score
    QTimer *scoreTimer = nullptr;

    // initial score is 5000.
    int finalScore = 5000;

    void startScoreTracking();
    void stopScoreTracking();
    void resetScore();

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
     * Submits the player's code and checks if they won or lost the round.
     * @brief on_submitButton_clicked
     */
    void on_submitButton_clicked();

    /**
     * Clears the workspace.
     * @brief on_resetButton_clicked
     */
    void on_clearButton_clicked();

    /**
     * Activates when the hint button is clicked.
     * @brief on_hintButton_clicked
     */
    void on_hintButton_clicked();

    /**
     * Activates when the restart button is clicked.
     * @brief on_restartButton_clicked
     */
    void on_restartButton_clicked();

    /**
     * Activates when the workspace move block is clicked.
     * @brief on_workspace_move_block_clicked
     */
    void on_workspace_move_block_clicked(CodeBlock *block);

    /**
     * Activates when the on route point block is clicked.
     * @brief on_route_point_block_clicked
     */
    void on_route_point_block_clicked(CodeBlock *block);

signals:
    /**
     * Activated when the level is completed.
     * @brief levelCompleted
     */
    void levelCompleted();
};
#endif // MAINWINDOW_H
