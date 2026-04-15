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
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "codeBlock.h"
#include "confetti.h"
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    // currentLevelRoutePoints = {"moveToA();", "moveToB();", "moveToC();", "moveToD();", "moveToE();", "moveToF();", "moveToG();"};

    // for (const QString &pointName : currentLevelRoutePoints) {
    //     CodeBlock *pointBlock = new CodeBlock(pointName, "method");
    //     ui->blockSpaceWidget->addBlock(pointBlock);
    //     routePointBlocks.append(pointBlock);
    // }
    setUpLevels();

    ui->workspaceWidget->setStyleSheet("background-color: lightgray;");

    // Wire the new game-page navigation buttons
    connect(ui->backToLevelsButton, &QPushButton::clicked,
            this, &MainWindow::on_backToLevelsButton_clicked);
    connect(ui->hintButton, &QPushButton::clicked, this, &MainWindow::on_runButton_clicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
    connect(ui->hintButton, &QPushButton::clicked, this, &MainWindow::on_hintButton_clicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::on_saveButton_clicked);

    // select level buttons
    connect(ui->level_1_Button, &QPushButton::clicked, this, &MainWindow::on_levelOneButton_clicked);
    connect(ui->level_2_Button, &QPushButton::clicked, this, &MainWindow::on_levelTwoButton_clicked);
    connect(ui->level_3_Button, &QPushButton::clicked, this, &MainWindow::on_levelThreeButton_clicked);
    connect(ui->level_4_Button, &QPushButton::clicked, this, &MainWindow::on_levelFourButton_clicked);
    connect(ui->level_5_Button, &QPushButton::clicked, this, &MainWindow::on_levelFiveButton_clicked);

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

// -------------------- Helper methods --------------------
void MainWindow::setUpLevels()
{
    setUpLevel1();
    ui->blockSpaceWidget->addBlock(new CodeBlock("moveUp();", "method"));
    ui->blockSpaceWidget->addBlock(new CodeBlock("moveDown();", "method"));
    ui->blockSpaceWidget->addBlock(new CodeBlock("moveRight();", "method"));
    ui->blockSpaceWidget->addBlock(new CodeBlock("moveLeft();", "method"));
}

void MainWindow::setUpLevel1()
{
    QWidget *canvas = ui->gameCanvasFrame;
    canvas->setFixedSize(COLS * TILE, ROWS * TILE);

    QLabel *bg = new QLabel(canvas);
    bg->setPixmap(QPixmap(":/images/grass.png").scaled(
        canvas->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        ));
    bg->setGeometry(0, 0, canvas->width(), canvas->height());
    bg->lower();

    // ---- PRINCESS ----
    princess = new QLabel(canvas);
    princess->setPixmap(QPixmap(":/images/princess.png"));
    princess->setScaledContents(true);
    princess->setFixedSize(TILE, TILE);
    princess->move(0 * TILE, 10 * TILE);

    // ---- CASTLE ----
    castle = new QLabel(canvas);
    castle->setPixmap(QPixmap(":/images/castle.png"));
    castle->setScaledContents(true);
    castle->setFixedSize(2 * TILE, 2 * TILE);
    castle->move(10 * TILE, 0 * TILE);

    // ---- MAZE LAYOUT ----
    int maze[ROWS][COLS] = {
        {1,1,1,1,1,1,1,1,1,1,0,0},
        {1,0,0,0,0,1,0,0,0,0,0,0},
        {1,0,1,1,0,1,0,1,1,1,0,1},
        {1,0,1,0,0,0,0,1,0,0,0,1},
        {1,0,1,0,1,1,1,1,0,1,0,1},
        {1,0,0,0,1,0,0,0,0,1,0,1},
        {1,1,1,0,1,0,1,1,1,1,0,1},
        {1,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,1},
        {0,0,0,0,0,0,0,0,0,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (maze[r][c] == 1) {
                addWall(r, c);
            }
        }
    }
    princess->raise();
    castle->raise();
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
                                               "Learn how to use the move(); method");
        } else if (levelButton == ui->level_2_Button){
            ui->levelDescriptionLabel->setText("Level Two:\n"
                                               "for loop\n"
                                               "Learn how to simplify code by using a loop\n"
                                               "simplify the code you made last round by using loops");
        } else if (levelButton == ui->level_3_Button){
            ui->levelDescriptionLabel->setText("Level Three:\n"
                                               "if statement\n"
                                               "learn that if you see the enemy, wait");
        } else if (levelButton == ui->level_4_Button){
            ui->levelDescriptionLabel->setText("Level Four\n"
                                               "function\n"
                                               "learn that you can put multiple methods in a function and call it later");
        } else {
            ui->levelDescriptionLabel->setText("Level Five:\n"
                                               "free for all: solve the level however you want using the skills that you learned\n"
                                               "try and get the simplest code that completes the same task");
        }
    }
    return QMainWindow::eventFilter(levelButton, hoverEvent);
}

void MainWindow::unlockLevelButtons()
{
    int unlocked = saveFile.getLatestUnlocked();

    // Lock and unlock levels based on different conditions.
    ui->level_2_Button->setEnabled(unlocked >= 2);
    ui->level_3_Button->setEnabled(unlocked >= 3);
    ui->level_4_Button->setEnabled(unlocked >= 4);
    ui->level_5_Button->setEnabled(unlocked >= 5);
}

void MainWindow::openSpecificLevel(int levelNumber)
{
    int unlockedLevel = saveFile.getLatestUnlocked();

    // If you attempt to enter a level that has not yet been unlocked, a notification message will appear.
    if (levelNumber > unlockedLevel) {
        ui->levelDescriptionLabel->setText("This level is locked. You need to complete the previous level.");
    }
    return;

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
            unlockedLevel++;
        }
    }

    unlockLevelButtons();
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
    ui->workspaceWidget->clearBlocks();
}

void MainWindow::on_hintButton_clicked()
{
}

void MainWindow::on_saveButton_clicked()
{
}
