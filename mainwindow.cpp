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

    currentLevelRoutePoints = {"moveToA();", "moveToB();", "moveToC();", "moveToD();", "moveToE();", "moveToF();", "moveToG();"};

    for (const QString &pointName : currentLevelRoutePoints) {
        CodeBlock *pointBlock = new CodeBlock(pointName, "method");
        ui->blockSpaceWidget->addBlock(pointBlock);
        routePointBlocks.append(pointBlock);
    }
    setUpLevels();

    ui->workspaceWidget->setStyleSheet("background-color: lightgray;");

    // Wire the new game-page navigation buttons
    connect(ui->backToLevelsButton, &QPushButton::clicked,
            this, &MainWindow::on_backToLevelsButton_clicked);
    connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::on_runButton_clicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
    connect(ui->hintButton, &QPushButton::clicked, this, &MainWindow::on_hintButton_clicked);
    connect(ui->restartButton, &QPushButton::clicked, this, &MainWindow::on_restartButton_clicked);

    // select level buttons
    connect(ui->level_1_Button, &QPushButton::clicked, this, &MainWindow::on_levelOneButton_clicked);
    connect(ui->level_2_Button, &QPushButton::clicked, this, &MainWindow::on_levelTwoButton_clicked);
    connect(ui->level_3_Button, &QPushButton::clicked, this, &MainWindow::on_levelThreeButton_clicked);
    connect(ui->level_4_Button, &QPushButton::clicked, this, &MainWindow::on_levelFourButton_clicked);
    connect(ui->level_5_Button, &QPushButton::clicked, this, &MainWindow::on_levelFiveButton_clicked);
    connect(ui->workspaceWidget, &Workspace::moveBlockClicked,
            this, &MainWindow::on_workspace_wove_block_clicked);

    // add confetti
    Confetti *confetti = new Confetti(this);
    confetti->setGeometry(this->rect());
    confetti->raise();
    connect(this, &MainWindow::levelCompleted, confetti, &Confetti::startConfetti);

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
    ui->aboutButton_2->setStyleSheet(menuButtonStyle);

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
    ui->restartButton->setStyleSheet(codePageButtonStyle);
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
    levelPointPositions["start"] = QPoint(20, 215);
    levelPointPositions["a"] = QPoint(90, 80);
    levelPointPositions["b"] = QPoint(220, 80);
    levelPointPositions["c"] = QPoint(350, 80);
    levelPointPositions["d"] = QPoint(90, 350);
    levelPointPositions["e"] = QPoint(220, 350);
    levelPointPositions["f"] = QPoint(350, 350);
    levelPointPositions["g"] = QPoint(430, 215);

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
            "background-color: white;"
            "border: 2px solid black;"
            "border-radius: 14px;"
            "font-weight: bold;"
            );
    }

    point->show();
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

bool MainWindow::won()
{
    return princess->pos() == castle->pos();
}

// -------------------- Start Page Slots --------------------
void MainWindow::on_startButton_clicked()
{
    on_levelOneButton_clicked();
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
    // TODO add the rest of this functionality
    if (won()) {
        emit levelCompleted();
    }

}

void MainWindow::on_resetButton_clicked()
{
    exitRoutePointSelectionMode();
    ui->workspaceWidget->clearBlocks();
}

void MainWindow::on_hintButton_clicked()
{
}

void MainWindow::on_restartButton_clicked()
{
    //TODO:reset characters/npc/game's state
    ui->workspaceWidget->clearBlocks();

    // show temporary message
    QLabel *msg = new QLabel("Game Restarted!", ui->codePage);
    msg->setAlignment(Qt::AlignCenter);
    msg->setStyleSheet(
        "background-color: #1a1a2e;"
        "color: #ffd700;"
        "border: 2px solid #ffd700;"
        "border-radius: 8px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        );
    msg->adjustSize();
    msg->move((ui->codePage->width() - msg->width()) / 2, 70);//center
    msg->show();
    msg->raise();

    // disappear after 1.2 seconds
    QTimer::singleShot(1200, msg, &QLabel::deleteLater);
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
