/* IDEA FOR LEVELS

level one: teach user what a method is
- has to use basic move methods to move princess to castle through a maze

level two: teach user what a for-loop is
- same thing as level one, move princess to castle through a maze, but use a for-loop this time

level three: teach user what an if-statement is
- have a dragon be moving across the screen side to side and have the user move the princess to the castle,
but if a dragon is in the way, use wait method (or something like that?)
- move princess to castle while avoiding the dragon

level four: teach user what a function is
- have the user use a function called something like "moveToCastle" or "avoidDragon" and then they can add
the code in for that function and then use that function name in the "main method" to get the princess to the castle.

level five: ? I was thinking maybe we could do like a free for all and the user can try and get the princess to the
castle using the skills they learned and try to use the least amount of code blocks while accomplishing this task?
*/

#include "mainwindow.h"
#include "workspace.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "codeBlock.h"
#include "confetti.h"
#include <QEvent>
#include <QTimer>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    saveFile = SaveFile();
    ui->setupUi(this);

    ui->workspaceWidget->setAttribute(Qt::WA_StyledBackground, true);
    ui->blockSpaceWidget->setAttribute(Qt::WA_StyledBackground, true);

    applyDarkTheme();
    setWindowTitle("Code Quest");

    // Show the welcome/start page on launch
    ui->stackedWidget->setCurrentIndex(START_PAGE);

    // Apply gold border style only to the level number buttons
    setLevelButtonStyles();
    setLevelDescriptionStyles();

    // Enables the specified filter object to receive the target object within the eventFilter() function.
    ui->level_1_Button->installEventFilter(this);
    ui->level_2_Button->installEventFilter(this);
    ui->level_3_Button->installEventFilter(this);
    ui->level_4_Button->installEventFilter(this);
    ui->level_5_Button->installEventFilter(this);

    // Add code blocks to blockSpace
    //ui->blockSpaceWidget->addBlock("move();", "move");
    // ui->blockSpaceWidget->addBlock("if {", "ifStatement");
    // ui->blockSpaceWidget->addBlock("turnLeft();", "turnLeft");
    // ui->blockSpaceWidget->addBlock("turnRight();", "turnRight");

    setUpLevels();

    ui->workspaceWidget->setStyleSheet("background-color: lightgray;");

    // select level buttons
    connect(ui->level_1_Button, &QPushButton::clicked, this, &MainWindow::on_levelOneButton_clicked);
    connect(ui->level_2_Button, &QPushButton::clicked, this, &MainWindow::on_levelTwoButton_clicked);
    connect(ui->level_3_Button, &QPushButton::clicked, this, &MainWindow::on_levelThreeButton_clicked);
    connect(ui->level_4_Button, &QPushButton::clicked, this, &MainWindow::on_levelFourButton_clicked);
    connect(ui->level_5_Button, &QPushButton::clicked, this, &MainWindow::on_levelFiveButton_clicked);
    connect(ui->workspaceWidget, &Workspace::moveBlockClicked,
            this, &MainWindow::on_workspace_move_block_clicked);

    // add confetti
    Confetti *confetti = new Confetti(this);
    confetti->setGeometry(this->rect());
    confetti->raise();
    connect(this, &MainWindow::levelCompleted, confetti, &Confetti::startConfetti);

    moveTimer = new QTimer(this);
    moveTimer->setInterval(16);

    connect(moveTimer, &QTimer::timeout,
            this, &MainWindow::stepPlayerMovement);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applyDarkTheme()
{
    ui->startPage->setStyleSheet(
        "QWidget#startPage {"
        "  background-color: #0d0d1a;"
        "}"
        );

    ui->levelSelectionPage->setStyleSheet(
        "QWidget#levelSelectionPage {"
        "  background-color: #0d0d1a;"
        "}"
        );

    ui->titleLabel->setStyleSheet(
        "QLabel {"
        "  color: #ffd700;"
        "  background-color: transparent;"
        "  letter-spacing: 4px;"
        "}"
        );

    ui->subtitleLabel->setStyleSheet(
        "QLabel {"
        "  color: #8899aa;"
        "  background-color: transparent;"
        "}"
        );

    // Shared style for all start-page buttons
    QString menuButtonStyle =
        "QPushButton {"
        "  background-color: #1a1a2e;"
        "  color: #e0e0ff;"
        "  border: 2px solid #3a3a6e;"
        "  border-radius: 8px;"
        "  padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #252550;"
        "  border: 2px solid #ffd700;"
        "  color: #ffd700;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #111130;"
        "  border: 2px solid #cc9900;"
        "}";

    ui->startButton->setStyleSheet(menuButtonStyle);
    ui->levelsButton->setStyleSheet(menuButtonStyle);
    ui->aboutButton->setStyleSheet(menuButtonStyle);
    ui->restartButton->setStyleSheet(menuButtonStyle);

    ui->codePage->setStyleSheet(
        "QWidget#codePage {"
        "  background-color: #111120;"
        "}"
        );

    // Header bar on code page
    ui->headerWidget->setStyleSheet(
        "QWidget#headerWidget {"
        "  background-color: #0d0d1a;"
        "  border-bottom: 2px solid #ffd700;"
        "}"
        );

    ui->levelTitleLabel->setStyleSheet(
        "QLabel {"
        "  color: #ffd700;"
        "  background-color: transparent;"
        "}"
        );

    // Section labels: Game Board, Blocks, Your Code
    QString sectionLabelStyle =
        "QLabel {"
        "  color: #aabbcc;"
        "  background-color: transparent;"
        "}";
    ui->gameAreaLabel->setStyleSheet(sectionLabelStyle);
    ui->blocksLabel->setStyleSheet(sectionLabelStyle);
    ui->workspaceLabel->setStyleSheet(sectionLabelStyle);

    // Game canvas frame
    ui->gameCanvasFrame->setStyleSheet(
        "QFrame#gameCanvasFrame {"
        "  background-color: #1a1a2e;"
        "  border: 2px solid #3a3a6e;"
        "  border-radius: 4px;"
        "}"
        );

    // Header buttons on code page (Back, Restart, Run, Reset, Hint)
    QString codePageButtonStyle =
        "QPushButton {"
        "  background-color: #1a1a2e;"
        "  color: #c8d0e0;"
        "  border: 1px solid #3a3a6e;"
        "  border-radius: 5px;"
        "  padding: 4px 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #252550;"
        "  border: 1px solid #ffd700;"
        "  color: #ffd700;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #111130;"
        "}";

    ui->backToLevelsButton->setStyleSheet(codePageButtonStyle);
    ui->runButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #1a3a1a;"
        "  color: #66ff88;"
        "  border: 1px solid #33aa44;"
        "  border-radius: 5px;"
        "  padding: 4px 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #224422;"
        "  border: 1px solid #88ffaa;"
        "  color: #88ffaa;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #112211;"
        "}"
        );
    ui->resetButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #3a1a1a;"
        "  color: #ff8866;"
        "  border: 1px solid #aa4433;"
        "  border-radius: 5px;"
        "  padding: 4px 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #442222;"
        "  border: 1px solid #ffaa88;"
        "  color: #ffaa88;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #221111;"
        "}"
        );
    ui->hintButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #1a2a3a;"
        "  color: #88bbff;"
        "  border: 1px solid #3355aa;"
        "  border-radius: 5px;"
        "  padding: 4px 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #223344;"
        "  border: 1px solid #88bbff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #111a22;"
        "}"
        );

    // Divider lines on code page
    ui->headerDivider->setStyleSheet("background-color: #ffd700;");
    ui->divider1->setStyleSheet("color: #3a3a6e;");
    ui->divider2->setStyleSheet("color: #3a3a6e;");

    // Block space and workspace panels
    ui->blockSpaceWidget->setStyleSheet(
        "QListWidget {"
        "background-color: #2a2a40;"
        "border: 1px solid #444466;"
        "border-radius: 6px;"
        "}"
        );
    ui->workspaceWidget->setStyleSheet(
        "background-color: #202040;"
        "border: 1px solid #3a3a6e;"
        "border-radius: 6px;"
        "background-image: radial-gradient(#2a2a50 1px, transparent 1px);"
        "background-size: 10px 10px;"
        );
}

// -------------------- Helper methods --------------------
void MainWindow::setUpLevels()
{
    setUpLevel1();
    CodeBlock *moveBlock = new CodeBlock("move", "move");
    ui->blockSpaceWidget->addBlock(moveBlock);
    normalPaletteBlocks.append(moveBlock);

    currentLevelRoutePoints = {"a", "b", "c", "d", "e", "f", "g"};
    createRoutePointBlocks();
    setRoutePointsVisible(false);
}

void MainWindow::setUpLevel1()
{
    QWidget *canvas = ui->gameCanvasFrame;
    canvas->setFixedSize(COLS * TILE, ROWS * TILE);

    clearLevel1Visuals();

    QLabel *bg = new QLabel(canvas);
    bg->setPixmap(QPixmap(":/images/grass.png").scaled(
        canvas->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        ));
    bg->setGeometry(0, 0, canvas->width(), canvas->height());
    bg->lower();

    levelPointPositions.clear();
    levelPointPositions["start"] = QPoint(20, 200);
    levelPointPositions["a"] = QPoint(90, 80);
    levelPointPositions["b"] = QPoint(220, 80);
    levelPointPositions["c"] = QPoint(350, 80);
    levelPointPositions["d"] = QPoint(90, 320);
    levelPointPositions["e"] = QPoint(220, 320);
    levelPointPositions["f"] = QPoint(350, 320);
    levelPointPositions["g"] = QPoint(430, 200);

    addPointVisual("a", levelPointPositions["a"] + QPoint(6, 6));
    addPointVisual("b", levelPointPositions["b"] + QPoint(6, 6));
    addPointVisual("c", levelPointPositions["c"] + QPoint(6, 6));
    addPointVisual("d", levelPointPositions["d"] + QPoint(6, 6));
    addPointVisual("e", levelPointPositions["e"] + QPoint(6, 6));
    addPointVisual("f", levelPointPositions["f"] + QPoint(6, 6));
    addPointVisual("g", levelPointPositions["g"] + QPoint(6, 6));

    princess = new QLabel(canvas);
    princess->setPixmap(QPixmap(":/images/princess.png"));
    princess->setScaledContents(true);
    princess->setFixedSize(TILE, TILE);
    princess->move(levelPointPositions[startPoint]);

    castle = new QLabel(canvas);
    castle->setPixmap(QPixmap(":/images/castle.png"));
    castle->setScaledContents(true);
    castle->setFixedSize(TILE, TILE);
    castle->move(levelPointPositions[goalPoint] + QPoint(25, -10));

    setUpDragons();

    princess->raise();
}

void MainWindow::clearLevel1Visuals()
{
    QWidget *canvas = ui->gameCanvasFrame;

    QList<QLabel*> labels = canvas->findChildren<QLabel*>(QString(), Qt::FindDirectChildrenOnly);
    for (QLabel *label : labels) {
        delete label;
    }

    walls.clear();
    princess = nullptr;
    castle = nullptr;
}

void MainWindow::addPointVisual(const QString &name, const QPoint &pos)
{
    QLabel *point = new QLabel(name, ui->gameCanvasFrame);
    point->setAlignment(Qt::AlignCenter);
    point->setGeometry(pos.x(), pos.y(), 28, 28);

    if (name == goalPoint) {
        point->setStyleSheet(
            "background-color: gold;"
            "border: 2px solid black;"
            "border-radius: 14px;"
            "font-weight: bold;"
            );
    } else {
        point->setStyleSheet(
            "background-color: blue;"
            "border: 2px solid black;"
            "border-radius: 14px;"
            "font-weight: bold;"
            );
    }

    point->show();
}

QStringList MainWindow::getProgramRoute() const
{
    QStringList route;
    QList<CodeBlock*> blocks = ui->workspaceWidget->getBlocks();

    for (CodeBlock *block : blocks) {
        if (block && block->getType() == "routePoint") {
            route << block->text();
        }
    }

    return route;
}

void MainWindow::resetPrincessToStart()
{
    if (moveTimer) {
        moveTimer->stop();
    }

    activeRoute.clear();

    if (princess && levelPointPositions.contains(startPoint)) {
        princess->move(levelPointPositions[startPoint]);
    }
}

void MainWindow::resetDragonsToStart(){
    if(dragonTimer){
        dragonTimer->stop();
    }

    activeRoute.clear();

    if(dragon1){
        dragon1->move(125, 180);
        dragon2->move(265, 180);
    } else if (dragon2){
        dragon1->move(125, 180);
        dragon2->move(265, 180);
    }

    dragon1Direction = -1;
    dragon2Direction = -1;

    // stop for 2s then continue moving.
    QTimer::singleShot(2000, this, [this]() {
        dragonTimer->start(20);
    });
}

void MainWindow::startPlayerMovement(const QStringList &route)
{
    activeRoute = route;

    if (!activeRoute.isEmpty() && moveTimer) {
        moveTimer->start();
    }
}

void MainWindow::stepPlayerMovement()
{
    if (!princess || activeRoute.isEmpty()) {
        if (moveTimer) {
            moveTimer->stop();
        }

        if (won()) {
            completeCurrentLevel();
            emit levelCompleted();
        }
        return;
    }

    QString targetName = activeRoute.first();

    if (!levelPointPositions.contains(targetName)) {
        activeRoute.removeFirst();
        return;
    }

    QPoint currentPos = princess->pos();
    QPoint targetPos = levelPointPositions[targetName];

    double dx = targetPos.x() - currentPos.x();
    double dy = targetPos.y() - currentPos.y();
    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= playerSpeed) {
        princess->move(targetPos);

        if (isTouchingDragon()) {
            handlePlayerDeath();
            return;
        }

        activeRoute.removeFirst();

        if (activeRoute.isEmpty()) {
            moveTimer->stop();

            if (won()) {
                completeCurrentLevel();
                emit levelCompleted();
            }
        }
        return;
    }

    int nextX = static_cast<int>(std::round(currentPos.x() + (dx / distance) * playerSpeed));
    int nextY = static_cast<int>(std::round(currentPos.y() + (dy / distance) * playerSpeed));

    princess->move(nextX, nextY);

    if (isTouchingDragon()) {
        handlePlayerDeath();
        return;
    }
}

bool MainWindow::isTouchingDragon() const
{
    if (!princess || !dragon1 || !dragon2) {
        return false;
    }

    QRect playerRect = princess->geometry();
    QRect dragon1Rect = dragon1->geometry();
    QRect dragon2Rect = dragon2->geometry();

    return playerRect.intersects(dragon1Rect) || playerRect.intersects(dragon2Rect);
}

void MainWindow::handlePlayerDeath()
{
    resetPrincessToStart();
    resetDragonsToStart();
}

void MainWindow::addWall(int row, int col)
{
    QLabel *wall = new QLabel(ui->gameCanvasFrame);
    wall->setStyleSheet(
        "background-color: rgba(0,0,0,200);"
        "border-radius: 4px;"
        );

    wall->setFixedSize(TILE, TILE);
    wall->move(col * TILE, row * TILE);
    wall->show();

    walls.append(wall);
}

void MainWindow::movePrincess(int dx, int dy)
{
    QPoint pos = princess->pos();

    int newX = pos.x() + dx * TILE;
    int newY = pos.y() + dy * TILE;

    if (newX < 0 || newY < 0 ||
        newX >= COLS * TILE || newY >= ROWS * TILE)
        return;

    princess->move(newX, newY);
}

void MainWindow::setUpDragons(){
    QWidget *canvas = ui->gameCanvasFrame;

    // add two dragons on map
    dragon1 = new QLabel(canvas);
    dragon1->setPixmap(QPixmap(":/images/dragon.png"));
    dragon1->setScaledContents(true);
    dragon1->setFixedSize(90, 90);
    dragon1->move(125, 180);

    dragon2 = new QLabel(canvas);
    dragon2->setPixmap(QPixmap(":/images/dragon.png"));
    dragon2->setScaledContents(true);
    dragon2->setFixedSize(90, 90);
    dragon2->move(265, 180);

    // set up how far the dragon moves each time, and how often it triggers a movement.
    dragonTimer = new QTimer(this);

    connect(dragonTimer, &QTimer::timeout, this, [this]() {
        moveDragons(2);
    });
}

void MainWindow::dragonStartMove()
{
    if (dragonTimer) {
        dragonTimer->start(20);
    }
}

void MainWindow::moveDragons(int dy){
    // update dragon position
    int newY1 = dragon1->y() + dy * dragon1Direction;
    int newY2 = dragon2->y() + dy * dragon2Direction;

    // Change movement direction upon reaching the specified coordinates.
    if (newY1 <= DRAGON_MOVE_TOP_Y) {
        newY1 = DRAGON_MOVE_TOP_Y;
        dragon1Direction = 1;
    } else if (newY1 >= DRAGON_MOVE_BOTTOM_Y) {
        newY1 = DRAGON_MOVE_BOTTOM_Y;
        dragon1Direction = -1;
    }

    if (newY2 <= DRAGON_MOVE_TOP_Y) {
        newY2 = DRAGON_MOVE_TOP_Y;
        dragon2Direction = 1;
    } else if (newY2 >= DRAGON_MOVE_BOTTOM_Y) {
        newY2 = DRAGON_MOVE_BOTTOM_Y;
        dragon2Direction = -1;
    }

    dragon1->move(dragon1->x(), newY1);
    dragon2->move(dragon2->x(), newY2);

    if (isTouchingDragon()) {
        handlePlayerDeath();
    }
}

// void MainWindow::dragonStartMove()
// {
//     if (dragonTimer) {
//         dragonTimer->start(20);
//     }
// }

bool MainWindow::won()
{
    return princess &&
           levelPointPositions.contains(goalPoint) &&
           princess->pos() == levelPointPositions[goalPoint];
}

// -------------------- Start Page Slots --------------------
void MainWindow::on_startButton_clicked()
{
    on_levelOneButton_clicked();
    dragonStartMove();
}

void MainWindow::on_levelsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(LEVEL_PAGE);
}

void MainWindow::on_aboutButton_clicked()
{
    QMessageBox::about(
        this,
        "About Code Quest",
        "Code Quest\n\n"
        "A block-based coding education game.\n"
        "Drag and drop code blocks to solve each challenge.\n\n"
        "Use the level buttons to pick where you want to start!"
        "-----Developed by bitcat.-----"
        );
}

void MainWindow::on_backToStartButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(START_PAGE);
}

//----------------Level Button Slots-------------------
void MainWindow::setLevelButtonStyles()
{
    QString levelStyle =
        "QPushButton {"
        "border: 3px solid rgb(255, 215, 0);"
        "border-radius: 12px;"
        "color: rgb(255, 215, 0);"
        "}"
        "QPushButton:hover {"
        "border: 4px solid rgb(255, 245, 160);"
        "color: rgb(255, 245, 160);"
        "}"
        "QPushButton {"
        "  background-color: #1a1a2e;"
        "  color: #e0e0ff;"
        "  border: 2px solid #3a3a6e;"
        "  border-radius: 8px;"
        "  padding: 8px 20px;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #111130;"
        "  border: 2px solid #cc9900;"
        "}";

    ui->level_1_Button->setStyleSheet(levelStyle);
    ui->level_2_Button->setStyleSheet(levelStyle);
    ui->level_3_Button->setStyleSheet(levelStyle);
    ui->level_4_Button->setStyleSheet(levelStyle);
    ui->level_5_Button->setStyleSheet(levelStyle);
}

void MainWindow::setLevelDescriptionStyles()
{
    ui->levelDescriptionLabel->setStyleSheet(
        "border: 3px solid rgb(255, 215, 0);"
        "border-radius: 12px;"
        );
}

void MainWindow::on_levelOneButton_clicked()
{
    ui->levelTitleLabel->setText("Level 1");
    ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    // completeCurrentLevel();
}

void MainWindow::on_levelTwoButton_clicked()
{
    // ui->levelTitleLabel->setText("Level 2");
    // ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    openSpecificLevel(2);
}

void MainWindow::on_levelThreeButton_clicked()
{
    // ui->levelTitleLabel->setText("Level 3");
    // ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    openSpecificLevel(3);
}

void MainWindow::on_levelFourButton_clicked()
{
    // ui->levelTitleLabel->setText("Level 4");
    // ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    openSpecificLevel(4);
}

void MainWindow::on_levelFiveButton_clicked()
{
    // ui->levelTitleLabel->setText("Level 5");
    // ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    openSpecificLevel(5);
}

bool MainWindow::eventFilter(QObject *levelButton, QEvent *hoverEvent)
{
    // Check if the event corresponds to a mouse hover state.
    if (hoverEvent->type() == QEvent::Enter){

        // Generate corresponding descriptive text for each distinct level.
        if (levelButton == ui->level_1_Button){
            // ui->levelDescriptionLabel->setText("Level One:\n"
            //                                    "Run first and observe the enemy patrols.\n"
            //                                    "Use move(point) to send the robot to route points.\n"
            //                                    "Avoid the two enemies and reach the target.");
            ui->levelDescriptionLabel->setText("Level One:\n"
                                               "Basic method\n"
                                               "Learn how to use the move(); method\n"
                                               "Score: " + QString::number(saveFile.getScore(1)));
        } else if (levelButton == ui->level_2_Button){
            ui->levelDescriptionLabel->setText("Level Two:\n"
                                               "for loop\n"
                                               "Learn how to simplify code by using a loop\n"
                                               "simplify the code you made last round by using loops\n"
                                               "Score: " + QString::number(saveFile.getScore(2)));
        } else if (levelButton == ui->level_3_Button){
            ui->levelDescriptionLabel->setText("Level Three:\n"
                                               "if statement\n"
                                               "learn that if you see the enemy, wait\n"
                                               "Score: " + QString::number(saveFile.getScore(3)));
        } else if (levelButton == ui->level_4_Button){
            ui->levelDescriptionLabel->setText("Level Four\n"
                                               "function\n"
                                               "learn that you can put multiple methods\n"
                                               "in a function and call it later\n"
                                               "Score: " + QString::number(saveFile.getScore(4)));
        } else {
            ui->levelDescriptionLabel->setText("Level Five:\n"
                                               "free for all: solve the level however you want\n"
                                               "using the skills that you learned\n"
                                               "try and get the simplest code that completes the same task\n"
                                               "Score: " + QString::number(saveFile.getScore(5)));
        }
    }
    return QMainWindow::eventFilter(levelButton, hoverEvent);
}

void MainWindow::unlockLevelButtons()
{
    // Lock and unlock levels based on different conditions.
    ui->level_2_Button->setEnabled(saveFile.isLevelUnlocked(2));
    ui->level_3_Button->setEnabled(saveFile.isLevelUnlocked(3));
    ui->level_4_Button->setEnabled(saveFile.isLevelUnlocked(4));
    ui->level_5_Button->setEnabled(saveFile.isLevelUnlocked(5));
}

void MainWindow::openSpecificLevel(int levelNumber)
{
    // int unlockedLevel = saveFile.getLatestUnlocked();

    // If you attempt to enter a level that has not yet been unlocked, a notification message will appear.
    if (!saveFile.isLevelUnlocked(levelNumber)) {
        ui->levelDescriptionLabel->setText("This level is locked. You need to complete the previous level.");
        return;
    }

    // If the level is already unlocked, enter the current level.
    currentLevel = levelNumber;
    ui->levelTitleLabel->setText("Level " + QString::number(levelNumber));
    ui->stackedWidget->setCurrentIndex(CODE_PAGE);
}

void MainWindow::completeCurrentLevel()
{
    int unlockedLevel = saveFile.getLatestUnlocked();

    // If the currently completed level is one prior to Level 5, unlock the next level.
    if (unlockedLevel < 5) {
        if (currentLevel == unlockedLevel) {
            saveFile.updateUnlockedLevels(unlockedLevel + 1);
            saveFile.save();
        }
    }

    unlockLevelButtons();

    // If the current level is cleared, a window pops up (containing text, the score, and options).
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Level Complete!");
    msgBox.setText("Congratulations! You completed this level!\n"
                   "Your total score is " + QString::number(saveFile.getScore(currentLevel)));

    // Create three option buttons.
    QPushButton *restartButton = msgBox.addButton("Restart", QMessageBox::ActionRole);
    QPushButton *nextLevelButton = msgBox.addButton("Next Level", QMessageBox::ActionRole);
    QPushButton *quitButton = msgBox.addButton("Quit", QMessageBox::ActionRole);

    msgBox.exec();

    // Implement the functionality of each button.
    if (msgBox.clickedButton() == restartButton) {
        ui->workspaceWidget->clearBlocks();
        openSpecificLevel(currentLevel);
    } else if (msgBox.clickedButton() == nextLevelButton) {
        openSpecificLevel(currentLevel + 1);
    } else if (msgBox.clickedButton() == quitButton) {
        ui->stackedWidget->setCurrentIndex(START_PAGE);
    }
}

// -------------------- Game Page Slots --------------------
void MainWindow::on_backToLevelsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(LEVEL_PAGE);
}

void MainWindow::on_runButton_clicked()
{
    exitRoutePointSelectionMode();

    QStringList route = getProgramRoute();
    if (route.isEmpty()) {
        return;
    }

    resetPrincessToStart();
    startPlayerMovement(route);
}

void MainWindow::on_resetButton_clicked()
{
    exitRoutePointSelectionMode();
    ui->workspaceWidget->clearBlocks();
    resetPrincessToStart();
    resetDragonsToStart();
}

void MainWindow::on_hintButton_clicked()
{
    // Hint content per level
    struct HintContent {
        QString title;
        QString body;
    };

    static const HintContent hints[] = {
        // Level 1
        {
            "Level 1 Hint — Methods",
            "<b>What's a method?</b><br><br>"
            "A method is a named action you can call to make something happen.<br><br>"
            "<b>How to play:</b><br>"
            "1. Drag a <span style='color:#90ee90;'>move</span> block into your workspace.<br>"
            "2. Click the move block to pick a destination (A–G).<br>"
            "3. Chain multiple move blocks to navigate around the dragons.<br><br>"
            "<b>Tip:</b> The dragons patrol vertically in the middle. "
            "Try going (some routes, need to be fill later)...."
        },
        // Level 2
        {
            "Level 2 Hint — For Loops",
            "<b>Coming soon!</b><br><br>"
            "This level will teach you how to use a <b>for loop</b> "
            "to repeat actions without duplicating blocks.<br><br>"
            "<i>()</i>"
        },
        // Level 3
        {
            "Level 3 Hint — If Statements",
            "<b>Coming soon!</b><br><br>"
            "This level will teach you how to use an <b>if statement</b> "
            "to react to what's happening on screen.<br><br>"
            "<i>()</i>"
        },
        // Level 4
        {
            "Level 4 Hint — Functions",
            "<b>Coming soon!</b><br><br>"
            "This level will teach you how to define a <b>function</b> "
            "and call it by name.<br><br>"
            "<i>()</i>"
        },
        // Level 5
        {
            "Level 5 Hint — Free For All",
            "<b>Coming soon!</b><br><br>"
            "Use everything you've learned to reach the castle "
            "in as <b>few blocks as possible</b>.<br><br>"
            "<i>()</i>"
        },
    };

    int idx = qBound(1, currentLevel, 5) - 1;
    const HintContent &hint = hints[idx];

    QDialog dialog(this);
    dialog.setWindowTitle("Hint");
    dialog.setFixedSize(420, 300);
    dialog.setStyleSheet(
        "QDialog {"
        "  background-color: #0d0d1a;"
        "  border: 2px solid #ffd700;"
        "  border-radius: 12px;"
        "}"
        );

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(14);

    // Title
    QLabel *titleLabel = new QLabel(hint.title, &dialog);
    titleLabel->setStyleSheet(
        "color: #ffd700;"
        "font-size: 15px;"
        "font-weight: bold;"
        "background: transparent;"
        );
    titleLabel->setWordWrap(true);

    // Body
    QLabel *bodyLabel = new QLabel(&dialog);
    bodyLabel->setText(hint.body);
    bodyLabel->setTextFormat(Qt::RichText);
    bodyLabel->setWordWrap(true);
    bodyLabel->setStyleSheet(
        "color: #c8d8e8;"
        "font-size: 13px;"
        "background: transparent;"
        "line-height: 1.5;"
        );

    // Close button
    QPushButton *closeBtn = new QPushButton("Got it!", &dialog);
    closeBtn->setFixedHeight(36);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1a3a1a;"
        "  color: #66ff88;"
        "  border: 1px solid #33aa44;"
        "  border-radius: 6px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #224422;"
        "  border: 1px solid #88ffaa;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #112211;"
        "}"
        );
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addWidget(bodyLabel, 1);
    layout->addWidget(closeBtn);

    dialog.exec();
}

void MainWindow::on_restartButton_clicked()
{
    exitRoutePointSelectionMode();
    ui->workspaceWidget->clearBlocks();
    resetPrincessToStart();
    // Only warn if the user has actual progress beyond level 1
    if (saveFile.getLatestUnlocked() > 1) {
        QMessageBox confirmBox(this);
        confirmBox.setWindowTitle("Start New Game?");
        confirmBox.setText("You have existing progress. Starting a new game will erase all saved data and return to Level 1.");
        confirmBox.setIcon(QMessageBox::Warning);

        QPushButton *confirmBtn = confirmBox.addButton("Start New Game", QMessageBox::DestructiveRole);
        QPushButton *cancelBtn  = confirmBox.addButton("Cancel",         QMessageBox::RejectRole);
        confirmBox.setDefaultButton(cancelBtn);

        confirmBox.exec();

        if (confirmBox.clickedButton() != confirmBtn)
            return;
    }

    // Wipe save data
    saveFile.resetScores();
    saveFile.updateUnlockedLevels(1);
    saveFile.save();

    // Reset UI state
    ui->workspaceWidget->clearBlocks();
    exitRoutePointSelectionMode();
    unlockLevelButtons();

    // Go to level 1
    currentLevel = 1;
    ui->levelTitleLabel->setText("Level 1");
    ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    dragonStartMove();
}

void MainWindow::createRoutePointBlocks()
{
    for (CodeBlock *block : routePointBlocks) {
        if (block) {
            block->deleteLater();
        }
    }
    routePointBlocks.clear();

    for (const QString &pointName : currentLevelRoutePoints) {
        CodeBlock *pointBlock = new CodeBlock(pointName, "routePoint");
        ui->blockSpaceWidget->addBlock(pointBlock);
        pointBlock->hide();

        connect(pointBlock, &CodeBlock::blockClicked,
                this, &MainWindow::on_route_point_block_clicked);

        routePointBlocks.append(pointBlock);
    }
}

void MainWindow::setNormalPaletteBlocksVisible(bool visible)
{
    for (CodeBlock *block : normalPaletteBlocks) {
        if (block) {
            block->setVisible(visible);
        }
    }
}

void MainWindow::enterRoutePointSelectionMode(CodeBlock *moveBlock)
{
    if (!moveBlock) return;

    routePointSelectionMode = true;
    selectedMoveBlock = moveBlock;
    setNormalPaletteBlocksVisible(false);
    setRoutePointsVisible(true);
}

void MainWindow::exitRoutePointSelectionMode()
{
    setRoutePointsVisible(false);
    setNormalPaletteBlocksVisible(true);

    selectedMoveBlock = nullptr;
    routePointSelectionMode = false;
}
void MainWindow::on_workspace_move_block_clicked(CodeBlock *block)
{
    if (!block) return;
    if (!routePointSelectionMode) {
        enterRoutePointSelectionMode(block);
        return;
    }

    if (selectedMoveBlock == block) {
        exitRoutePointSelectionMode();
        return;
    }
}

void MainWindow::on_route_point_block_clicked(CodeBlock *block)
{
    if (!routePointSelectionMode || !selectedMoveBlock || !block) {
        return;
    }

    CodeBlock *newPointBlock = new CodeBlock(block->text(), "routePoint");
    ui->workspaceWidget->insertRoutePointAfterMove(selectedMoveBlock, newPointBlock);

    exitRoutePointSelectionMode();
}

void MainWindow::setRoutePointsVisible(bool visible)
{
    for (CodeBlock *pointBlock : routePointBlocks) {
        if (pointBlock) {
            pointBlock->setVisible(visible);
        }
    }

    routePointsVisible = visible;
}
