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

    currentLevelRoutePoints = {"moveToA();", "moveToB();", "moveToC();", "moveToD();", "moveToE();", "moveToF();", "moveToG();"};

    for (const QString &pointName : currentLevelRoutePoints) {
        CodeBlock *pointBlock = new CodeBlock(pointName, "method");
        ui->blockSpaceWidget->addBlock(pointBlock);
        routePointBlocks.append(pointBlock);
    }

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
    confetti->raise(); // draw on top
    connect(this, &MainWindow::levelCompleted, confetti, &Confetti::startConfetti);
}

MainWindow::~MainWindow()
{
    delete ui;
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
            ui->levelDescriptionLabel->setText("Level One:\n"
                                               "Run first and observe the enemy patrols.\n"
                                               "Use move(point) to send the robot to route points.\n"
                                               "Avoid the two enemies and reach the target.");
        } else if (levelButton == ui->level_2_Button){
            ui->levelDescriptionLabel->setText("Level Two:");
        } else if (levelButton == ui->level_3_Button){
            ui->levelDescriptionLabel->setText("Level Three:");
        } else if (levelButton == ui->level_4_Button){
            ui->levelDescriptionLabel->setText("Level Four:");
        } else {
            ui->levelDescriptionLabel->setText("Level Five:");
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

    // if they completed the level,
    emit levelCompleted();

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
