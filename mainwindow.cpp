#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "codeBlock.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Code Quest");

    // Show the welcome/start page on launch
    //ui->stackedWidget->setCurrentIndex(START_PAGE);
    ui->stackedWidget->setCurrentIndex(CODE_PAGE);

    // Apply gold border style only to the level number buttons
    setLevelButtonStyles();

    CodeBlock *moveBlock = new CodeBlock("move();", "move");
    moveBlock->setStyleSheet("background-color: lightgreen; padding: 8px;");
    ui->blockLayout->addWidget(moveBlock);

    CodeBlock *ifBlock = new CodeBlock("if {", "ifStatement");
    ifBlock->setStyleSheet("background-color: purple; padding: 8px;");
    ui->blockLayout->addWidget(ifBlock);

    ui->workspaceWidget->setStyleSheet("background-color: lightgray;");

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

void MainWindow::on_levelOneButton_clicked()
{

}

void MainWindow::on_levelTwoButton_clicked()
{

}

void MainWindow::on_levelThreeButton_clicked()
{

}

void MainWindow::on_levelFourButton_clicked()
{

}

void MainWindow::on_levelFiveButton_clicked()
{

}
