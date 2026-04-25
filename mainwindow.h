/**
 * @file MainWindow.h
 * @brief Header file for MainWindow class.
 * @author BitCat
 *
 * This file defines the MainWindow class, which controls the main gameplay
 * loop, level system, UI navigation, and player interaction logic for Code Quest.
 *
 * The MainWindow handles:
 * - Level loading and progression
 * - Player movement execution from block-based code
 * - Game state management (win/loss/reset)
 * - Route point selection system
 * - Dragon and obstacle behavior
 * - Score tracking and persistence
 */

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
    /**
     * Initializes the main game window and all UI/game systems.
     * @brief MainWindow constructor
     * @param parent parent widget
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * Cleans up dynamically allocated memory and timers.
     * @brief ~MainWindow destructor
     */
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // ---------------- UI styling ----------------

    /**
     * Applies styling to level selection buttons.
     * @brief setLevelButtonStyles
     */
    void setLevelButtonStyles();

    /**
     * Applies styling to the level description panel.
     * @brief setLevelDescriptionStyles
     */
    void setLevelDescriptionStyles();

    // ---------------- Route / block system ----------------

    /**
     * Toggles visibility of route point blocks.
     * @brief setRoutePointsVisible
     * @param visible true to show route points
     */
    void setRoutePointsVisible(bool visible);

    /**
     * Toggles visibility of normal palette blocks.
     * @brief setNormalPaletteBlocksVisible
     * @param visible true to show palette blocks
     */
    void setNormalPaletteBlocksVisible(bool visible);

    /**
     * Enters route selection mode for a movement block.
     * @brief enterRoutePointSelectionMode
     * @param moveBlock selected move block
     */
    void enterRoutePointSelectionMode(CodeBlock *moveBlock);

    /**
     * Exits route selection mode and restores UI state.
     * @brief exitRoutePointSelectionMode
     */
    void exitRoutePointSelectionMode();

    /**
     * Creates route point blocks for the current level.
     * @brief createRoutePointBlocks
     */
    void createRoutePointBlocks();

    // ---------------- Game state ----------------

    bool running = false;
    bool playerTriggeredMonster = false;

    QVector<CodeBlock*> routePointBlocks;
    QVector<CodeBlock*> normalPaletteBlocks;
    QStringList currentLevelRoutePoints;

    static constexpr int START_PAGE = 0;
    static constexpr int LEVEL_PAGE = 1;
    static constexpr int CODE_PAGE = 2;

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
    double playerSpeed = 2.3;

    // ---------------- Movement system ----------------

    /**
     * Builds the route from workspace blocks.
     * @brief getProgramRoute
     * @return ordered list of commands
     */
    QStringList getProgramRoute() const;

    /**
     * Resets princess to starting position.
     * @brief resetPrincessToStart
     */
    void resetPrincessToStart();

    /**
     * Starts player movement along route.
     * @brief startPlayerMovement
     * @param route movement instructions
     */
    void startPlayerMovement(const QStringList &route);

    /**
     * Advances player one step.
     * @brief stepPlayerMovement
     */
    void stepPlayerMovement();

    QList<int> highlightIndices;
    int originalRouteSize = 0;

    /**
     * Builds mapping between route steps and UI blocks.
     * @brief buildHighlightMap
     * @param expandedRoute full route including loops
     */
    void buildHighlightMap(const QStringList &expandedRoute);

    // ---------------- Level management ----------------

    /**
     * Unlocks level buttons based on save state.
     * @brief unlockLevelButtons
     */
    void unlockLevelButtons();

    /**
     * Loads a specific level.
     * @brief openSpecificLevel
     * @param levelNumber level index
     */
    void openSpecificLevel(int levelNumber);

    /**
     * Handles level completion logic.
     * @brief completeCurrentLevel
     */
    void completeCurrentLevel();

    /**
     * Handles level failure logic.
     * @brief failCurrentLevel
     */
    void failCurrentLevel();

    // ---------------- Canvas / entities ----------------

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

    int dragon1Direction = -1;
    int dragon2Direction = -1;

    bool gameStarted = false;
    bool dragonMovementEnabled = false;

    // ---------------- Level setup ----------------

    /**
     * Initializes all levels.
     * @brief setUpLevels
     */
    void setUpLevels();

    void setUpLevel1();
    void setUpLevel2();
    void setUpLevel3();
    void setUpLevel4();

    void addWall(int x, int y);
    void movePrincess(int dx, int dy);
    void setUpDragons();
    void moveDragons(int dy);
    void dragonStartMove();
    void showDragonAlertZone();
    void resetDragonsToStart();

    /**
     * Checks win condition.
     * @brief won
     * @return true if level completed
     */
    bool won();

    void applyDarkTheme();
    void clearLevel1Visuals();
    void addPointVisual(const QString &name, const QPoint &pos);

    /**
     * Checks collision with dragon.
     * @brief isTouchingDragon
     * @return true if player is touching dragon
     */
    bool isTouchingDragon() const;

    /**
     * Handles player death.
     * @brief handlePlayerDeath
     */
    void handlePlayerDeath();

    void setRunning(bool isRunning);
    void dimButtons(bool gameStarted);

    // ---------------- Box2D menu physics ----------------

    struct ButtonJiggle {
        QPushButton *button = nullptr;
        QPoint origin;
        b2Body *body = nullptr;
    };

    QVector<ButtonJiggle> jiggleBodies;
    b2World jiggleWorld{b2Vec2(0.f, 0.f)};
    QTimer *jiggleTimer = nullptr;

    void initJiggle();
    void stepJiggle();

    // ---------------- Level 2 ----------------

    void setUpGuard();
    void resetLevel2();
    void updateGuardCheck();
    void updateIfLevelLogic();

    void clearLevel2Visuals();

    // ---------------- Level 3 ----------------

    int doorHitCount = 0;
    static const int DOOR_HITS_NEEDED = 2;

    QLabel *door = nullptr;
    QVector<QLabel*> lockLabels;
    QVector<QLabel*> keyLabels;
    QLabel *carriedKey = nullptr;

    void clearLevel3Visuals();
    void resetLevel3();

    /**
     * Expands for-loop syntax into executable steps.
     * @brief expandForLoops
     * @param raw raw route
     * @return expanded route
     */
    QStringList expandForLoops(const QStringList &raw) const;

    // ---------------- Scoring ----------------

    QTimer *scoreTimer = nullptr;
    int finalScore = 5000;

    void startScoreTracking();
    void stopScoreTracking();
    void resetScore();

protected:
    /**
     * Handles hover events for level buttons.
     * @brief eventFilter
     * @param levelButton hovered object
     * @param hoverEvent event data
     * @return true if handled
     */
    bool eventFilter(QObject *levelButton, QEvent *hoverEvent) override;

private slots:
    // ---------------- Start page ----------------

    void on_startButton_clicked();
    void on_levelsButton_clicked();
    void on_aboutButton_clicked();
    void on_backToStartButton_clicked();

    // ---------------- Level selection ----------------

    void on_levelOneButton_clicked();
    void on_levelTwoButton_clicked();
    void on_levelThreeButton_clicked();
    void on_levelFourButton_clicked();
    void on_levelFiveButton_clicked();

    // ---------------- Game page ----------------

    void on_backToLevelsButton_clicked();
    void on_runButton_clicked();
    void on_submitButton_clicked();
    void on_clearButton_clicked();
    void on_hintButton_clicked();
    void on_restartButton_clicked();

    void on_workspace_move_block_clicked(CodeBlock *block);
    void on_route_point_block_clicked(CodeBlock *block);

signals:
    /**
     * Emitted when a level is completed.
     * @brief levelCompleted
     */
    void levelCompleted();
};

#endif // MAINWINDOW_H
