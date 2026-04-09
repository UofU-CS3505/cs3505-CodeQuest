#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setLevelButtonStyles();
}

MainWindow::~MainWindow()
{
    delete ui;
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
