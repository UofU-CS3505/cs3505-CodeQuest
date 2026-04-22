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

    setUpLevels();

    ui->workspaceWidget->setStyleSheet("background-color: lightgray;");

    // select level buttons
    connect(ui->level_1_Button, &QPushButton::clicked, this, &MainWindow::on_levelOneButton_clicked);
    connect(ui->level_2_Button, &QPushButton::clicked, this, &MainWindow::on_levelTwoButton_clicked);
    connect(ui->level_3_Button, &QPushButton::clicked, this, &MainWindow::on_levelThreeButton_clicked);
    connect(ui->level_4_Button, &QPushButton::clicked, this, &MainWindow::on_levelFourButton_clicked);
    connect(ui->workspaceWidget, &Workspace::moveBlockClicked,
            this, &MainWindow::on_workspace_move_block_clicked);

    // add confetti
    Confetti *confetti = new Confetti(this);
    confetti->setGeometry(this->rect());
    confetti->raise();
    connect(this, &MainWindow::levelCompleted, confetti, &Confetti::startConfetti);

    running = false;
    gameStarted = false;
    dragonMovementEnabled = false;

    moveTimer = new QTimer(this);
    moveTimer->setInterval(16);

    connect(moveTimer, &QTimer::timeout,
            this, &MainWindow::stepPlayerMovement);

    // tracking score
    scoreTimer = new QTimer(this);
    scoreTimer->setInterval(100);

    connect(scoreTimer, &QTimer::timeout, this, [this]() {
        finalScore -= 5;
        if (finalScore < 0) {
            finalScore = 0;
        }
    });

    // set up menu button jiggle after layout is fully resolved
    QTimer::singleShot(0, this, &MainWindow::initJiggle);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// -------------------- Menu button jiggle (Box2D) --------------------

void MainWindow::initJiggle()
{
    const QList<QPushButton*> btns = {
        ui->restartButton, ui->levelsButton, ui->startButton, ui->aboutButton
    };

    for (QPushButton *btn : btns) {
        b2BodyDef def;
        def.type           = b2_dynamicBody;
        def.position.Set(0.f, 0.f);
        def.linearDamping  = 8.f;  // heavy damping — settles in ~0.5s
        def.angularDamping = 8.f;
        b2Body *body = jiggleWorld.CreateBody(&def);

        b2PolygonShape shape;
        shape.SetAsBox(1.f, 1.f);
        b2FixtureDef fix;
        fix.shape   = &shape;
        fix.density = 1.f;
        body->CreateFixture(&fix);

        ButtonJiggle jg;
        jg.button = btn;
        jg.origin = btn->pos();
        jg.body   = body;
        jiggleBodies.append(jg);

        btn->installEventFilter(this);
    }

    jiggleTimer = new QTimer(this);
    jiggleTimer->setInterval(16);  // ~60fps
    connect(jiggleTimer, &QTimer::timeout, this, &MainWindow::stepJiggle);
    jiggleTimer->start();
}

void MainWindow::stepJiggle()
{
    jiggleWorld.Step(1.f / 60.f, 6, 2);

    for (ButtonJiggle &jg : jiggleBodies) {
        if (!jg.body || !jg.button) continue;

        b2Vec2 pos = jg.body->GetPosition();

        // spring force pulling back to origin
        const float k = 40.f;
        jg.body->ApplyForceToCenter(b2Vec2(-k * pos.x, -k * pos.y), true);

        // apply offset to button position
        jg.button->move(jg.origin + QPoint(static_cast<int>(pos.x),
                                           static_cast<int>(pos.y)));
    }
}

void MainWindow::applyDarkTheme()
{
    ui->startPage->setStyleSheet(
        "QWidget#startPage {"
        "  background-color: #232338;"
        "}"
        );

    ui->levelSelectionPage->setStyleSheet(
        "QWidget#levelSelectionPage {"
        "  background-color: #232338"
        "}"
        );

    ui->levelSelectionLabel->setStyleSheet(
        "QLabel {"
        "  color: #FFCC00;"
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

    ui->levelDescriptionFrame->setStyleSheet(
        "QFrame#levelDescriptionFrame {"
        "  background-color: #232338;"
        "  border-radius: 12px;"
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
        "  background-color: #232338"
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
        "}"
        "QPushButton:disabled {"
        "  background-color: #0f0f18;"
        "  color: #555566;"
        "  border: 1px solid #222233;"
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
    ui->clearButton->setStyleSheet(
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
        "QPushButton:disabled {"
        "  background-color: #0f0f18;"
        "  color: #555566;"
        "  border: 1px solid #222233;"
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
        "QPushButton:disabled {"
        "  background-color: #0f0f18;"
        "  color: #555566;"
        "  border: 1px solid #222233;"
        "}"
        );
    ui->submitButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #3a3a10;"
        "  color: #ffff88;"
        "  border: 2px solid #ffdd33;"
        "  border-radius: 6px;"
        "  padding: 5px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #4a4a18;"
        "  border: 2px solid #ffee55;"
        "  color: #ffffff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #2a2a08;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #0f0f18;"
        "  color: #555566;"
        "  border: 1px solid #222233;"
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
    //normalPaletteBlocks.append(moveBlock);

    currentLevelRoutePoints = {"moveToA();", "moveToB();", "moveToC();", "moveToD();", "moveToE();", "moveToF();", "moveToCastle();"};
    createRoutePointBlocks();
    setRoutePointsVisible(true);
}

void MainWindow::setUpLevel1()
{
    QWidget *canvas = ui->gameCanvasFrame;
    canvas->setFixedSize(COLS * TILE, ROWS * TILE);

    startPoint = "start";
    goalPoint = "moveToCastle();";

    clearLevel1Visuals();

    QLabel *bg = new QLabel(canvas);
    bg->setPixmap(QPixmap(":/images/grass.png").scaled(
        canvas->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        ));
    bg->setGeometry(0, 0, canvas->width(), canvas->height());
    bg->lower();
    bg->show();

    levelPointPositions.clear();
    levelPointPositions["start"] = QPoint(20, 200);
    levelPointPositions["moveToA();"] = QPoint(90, 80);
    levelPointPositions["moveToB();"] = QPoint(220, 80);
    levelPointPositions["moveToC();"] = QPoint(350, 80);
    levelPointPositions["moveToD();"] = QPoint(90, 320);
    levelPointPositions["moveToE();"] = QPoint(220, 320);
    levelPointPositions["moveToF();"] = QPoint(350, 320);
    levelPointPositions["moveToCastle();"] = QPoint(400, 160);

    addPointVisual("a", levelPointPositions["moveToA();"] + QPoint(6, 6));
    addPointVisual("b", levelPointPositions["moveToB();"] + QPoint(6, 6));
    addPointVisual("c", levelPointPositions["moveToC();"] + QPoint(6, 6));
    addPointVisual("d", levelPointPositions["moveToD();"] + QPoint(6, 6));
    addPointVisual("e", levelPointPositions["moveToE();"] + QPoint(6, 6));
    addPointVisual("f", levelPointPositions["moveToF();"] + QPoint(6, 6));
    addPointVisual("g", levelPointPositions["moveToCastle();"] + QPoint(6, 6));

    princess = new QLabel(canvas);
    princess->setPixmap(QPixmap(":/images/princess.png"));
    princess->setScaledContents(true);
    princess->setFixedSize(TILE, TILE);
    princess->move(levelPointPositions[startPoint]);
    princess->show();
    princess->raise();

    setUpDragons();
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
    dragon1 = nullptr;
    dragon2 = nullptr;
    dragonAlertZone = nullptr;
    guard = nullptr;
    guardZone = nullptr;
}

void MainWindow::setUpLevel2()
{
    QWidget *canvas = ui->gameCanvasFrame;
    canvas->setFixedSize(COLS * TILE, ROWS * TILE);

    clearLevel2Visuals();

    QLabel *bg = new QLabel(canvas);
    bg->setPixmap(QPixmap(":/images/grass.png").scaled(
        canvas->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        ));
    bg->setGeometry(0, 0, canvas->width(), canvas->height());
    bg->show();
    bg->lower();

    levelPointPositions.clear();

    startPoint = "start";
    goalPoint = "moveToB();";
    playerTriggeredMonster = false;

    levelPointPositions["start"] = QPoint(40, 200);
    levelPointPositions["moveToA();"] = QPoint(220, 70);
    levelPointPositions["moveToB();"] = QPoint(400, 200);

    addPointVisual("A", levelPointPositions["moveToA();"] + QPoint(6, 6));
    addPointVisual("B", levelPointPositions["moveToB();"] + QPoint(6, 6));

    princess = new QLabel(canvas);
    princess->setPixmap(QPixmap(":/images/princess.png"));
    princess->setScaledContents(true);
    princess->setFixedSize(TILE, TILE);
    princess->move(levelPointPositions["start"]);
    princess->show();

    dragon1 = new QLabel(canvas);
    dragon1->setPixmap(QPixmap(":/images/dragon.png"));
    dragon1->setScaledContents(true);
    dragon1->setFixedSize(TILE, TILE);
    dragon1->move(350, 200);
    dragon1->show();

    setUpGuard();
    showDragonAlertZone();

    bg->lower();

    if (guardZone) {
        guardZone->raise();
    }
    if (dragonAlertZone) {
        dragonAlertZone->raise();
    }

    dragon1->raise();
    princess->raise();

    if (guard) {
        guard->raise();
    }
}


void MainWindow::setUpGuard()
{
    QWidget *canvas = ui->gameCanvasFrame;

    if (guard) {
        delete guard;
        guard = nullptr;
    }
    if (guardZone) {
        delete guardZone;
        guardZone = nullptr;
    }

    QPoint guardPos = levelPointPositions["moveToA();"] + QPoint(55, -5);

    guard = new QLabel("G", canvas);
    guard->setPixmap(QPixmap(":/images/knight.png"));
    guard->setAlignment(Qt::AlignCenter);
    guard->setFixedSize(TILE * 1.4, TILE * 1.4);
    guard->setGeometry(guardPos.x(), guardPos.y(), 32, 32);
    guard->setScaledContents(true);
    guard->show();

    const int zoneDiameter = 160;

    guardZone = new QLabel(canvas);
    guardZone->setFixedSize(zoneDiameter, zoneDiameter);

    QPoint guardCenter = guard->geometry().center();
    guardZone->move(guardCenter.x() - zoneDiameter / 2,
                    guardCenter.y() - zoneDiameter / 2);

    guardZone->setStyleSheet(
        "background-color: rgba(255, 215, 0, 45);"
        "border: 3px solid rgba(255, 215, 0, 170);"
        "border-radius: 80px;"
        );
    guardZone->show();
}

void MainWindow::resetLevel2()
{
    playerTriggeredMonster = false;

    if (dragon1) {
        dragon1->show();
        dragon1->move(350, 200);
    }

    if (guard) {
        guard->show();
    }

    if (guardZone) {
        guardZone->show();
    }

    showDragonAlertZone();
}

void MainWindow::updateGuardCheck()
{
    if (currentLevel != 2 || !dragon1 || !guardZone || !dragon1->isVisible()) {
        return;
    }

    if (dragon1->geometry().intersects(guardZone->geometry())) {
        dragon1->hide();

        if (dragonAlertZone) {
            dragonAlertZone->hide();
        }

        playerTriggeredMonster = false;
    }
}

void MainWindow::updateIfLevelLogic()
{
    if ((currentLevel != 2 && currentLevel != 4) || !princess || !dragon1 || !dragon1->isVisible()) {
        return;
    }

    if (!playerTriggeredMonster &&
        dragonAlertZone &&
        princess->geometry().intersects(dragonAlertZone->geometry())) {
        playerTriggeredMonster = true;
    }

    if (currentLevel == 4 &&
        levelPointPositions.contains("moveToH();") &&
        princess->pos() == levelPointPositions["moveToH();"]) {
        playerTriggeredMonster = false;
    }

    if (playerTriggeredMonster && currentLevel == 2) {
        QPoint dragonPos = dragon1->pos();
        QPoint playerPos = princess->pos();

        double dx = playerPos.x() - dragonPos.x();
        double dy = playerPos.y() - dragonPos.y();
        double distance = std::sqrt(dx * dx + dy * dy);

        const double dragonSpeed = 1.8;

        if (distance > dragonSpeed) {
            int nextX = static_cast<int>(std::round(dragonPos.x() + (dx / distance) * dragonSpeed));
            int nextY = static_cast<int>(std::round(dragonPos.y() + (dy / distance) * dragonSpeed));
            dragon1->move(nextX, nextY);
        } else {
            dragon1->move(playerPos);
        }

        if (dragonAlertZone) {
            QPoint center = dragon1->geometry().center();
            dragonAlertZone->move(center.x() - dragonAlertZone->width() / 2,
                                  center.y() - dragonAlertZone->height() / 2);
        }
    }

    updateGuardCheck();

    if (isTouchingDragon()) {
        handlePlayerDeath();
    }
}

void MainWindow::showDragonAlertZone()
{
    if (dragonAlertZone) {
        delete dragonAlertZone;
        dragonAlertZone = nullptr;
    }

    QWidget *canvas = ui->gameCanvasFrame;

    int alertDiameter = 140;
    if (currentLevel == 4) {
        alertDiameter = 95;
    }

    dragonAlertZone = new QLabel(canvas);
    dragonAlertZone->setFixedSize(alertDiameter, alertDiameter);

    QPoint dragonCenter = dragon1->geometry().center();
    int x = dragonCenter.x() - alertDiameter / 2;
    int y = dragonCenter.y() - alertDiameter / 2;
    dragonAlertZone->move(x, y);

    dragonAlertZone->setStyleSheet(QString(
                                       "background-color: rgba(255, 0, 0, 50);"
                                       "border: 3px solid rgba(153, 0, 0, 180);"
                                       "border-radius: %1px;"
                                       ).arg(alertDiameter / 2));

    dragonAlertZone->show();
}

void MainWindow::clearLevel2Visuals()
{
    QWidget *canvas = ui->gameCanvasFrame;

    QList<QLabel*> labels = canvas->findChildren<QLabel*>(QString(), Qt::FindDirectChildrenOnly);
    for (QLabel *label : labels) {
        delete label;
    }

    walls.clear();
    princess = nullptr;
    guard = nullptr;
    guardZone = nullptr;
    dragon1 = nullptr;
    dragon2 = nullptr;
    dragonAlertZone = nullptr;
}

QStringList MainWindow::expandForLoops(const QStringList &raw) const
{
    QStringList result;
    int i = 0;
    while (i < raw.size()) {
        const QString &token = raw[i];
        if (token.startsWith("for(") && token.endsWith(")")) {
            bool ok = false;
            int times = token.mid(4, token.size() - 5).toInt(&ok);
            if (!ok || times <= 0) { i++; continue; }

            // collect everything between for(N) and the closing }
            QStringList body;
            int j = i + 1;
            while (j < raw.size()
                   && raw[j] != "}"
                   && !raw[j].startsWith("for(")) {
                body << raw[j++];
            }
            // consume the } so it doesn't leak into the result
            if (j < raw.size() && raw[j] == "}") j++;

            // repeat the body N times
            for (int t = 0; t < times; t++)
                result << body;
            i = j;
        } else if (token == "}") {
            // orphan } with no matching for — skip it
            i++;
        } else {
            result << token;
            i++;
        }
    }
    return result;
}

void MainWindow::setUpLevel3()
{
    QWidget *canvas = ui->gameCanvasFrame;
    canvas->setFixedSize(COLS * TILE, ROWS * TILE);
    clearLevel3Visuals();

    QLabel *bg = new QLabel(canvas);
    bg->setPixmap(QPixmap(":/images/grass.png").scaled(
        canvas->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, canvas->width(), canvas->height());
    bg->lower(); bg->show();

    startPoint = "start";
    goalPoint  = "moveToC();";
    doorHitCount = 0;

    levelPointPositions.clear();
    levelPointPositions["start"]      = QPoint(20,  200);
    levelPointPositions["moveToA();"] = QPoint(140, 200);
    levelPointPositions["moveToB();"] = QPoint(300, 200);
    levelPointPositions["moveToC();"] = QPoint(400, 170);

    addPointVisual("A", levelPointPositions["moveToA();"] + QPoint(6, 6));
    addPointVisual("B", levelPointPositions["moveToB();"] + QPoint(6, 6));
    addPointVisual("C", levelPointPositions["moveToC();"] + QPoint(6, 6));

    // 2 locks above A
    lockLabels.clear();
    for (int i = 0; i < DOOR_HITS_NEEDED; i++) {
        QLabel *lock = new QLabel(canvas);
        lock->setPixmap(QPixmap(":/images/lock.png").scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lock->setGeometry(levelPointPositions["moveToA();"].x() - 10 + i * 26, 160, 22, 22);
        lock->setStyleSheet("background: transparent;");
        lock->show();
        lockLabels.append(lock);
    }

    // 2 keys above B
    keyLabels.clear();
    for (int i = 0; i < DOOR_HITS_NEEDED; i++) {
        QLabel *key = new QLabel(canvas);
        key->setPixmap(QPixmap(":/images/key.png").scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        key->setGeometry(levelPointPositions["moveToB();"].x() - 10 + i * 26, 160, 22, 22);
        key->setStyleSheet("background: transparent;");
        key->show();
        keyLabels.append(key);
    }

    carriedKey = nullptr;

    door = new QLabel("X\nX\nX\nX\nX\nX\nX\nX", canvas);
    door->setAlignment(Qt::AlignCenter);
    door->setGeometry(368, 0, 28, canvas->height());
    door->setStyleSheet(
        "background-color: #aa2222; color: white; font-weight: bold;"
        "font-size: 14px; letter-spacing: 2px;"
        "border-left: 3px solid #660000; border-right: 3px solid #660000;");
    door->show();

    princess = new QLabel(canvas);
    princess->setPixmap(QPixmap(":/images/princess.png"));
    princess->setScaledContents(true);
    princess->setFixedSize(TILE, TILE);
    princess->move(levelPointPositions[startPoint]);
    princess->show();
    princess->raise();
    door->raise();
}

void MainWindow::clearLevel3Visuals()
{
    QWidget *canvas = ui->gameCanvasFrame;
    const auto labels = canvas->findChildren<QLabel*>(QString(), Qt::FindDirectChildrenOnly);
    for (QLabel *l : labels) delete l;

    walls.clear();
    princess = nullptr;
    dragon1  = nullptr;
    dragon2 = nullptr;
    door     = nullptr;
    dragonAlertZone = nullptr;
    lockLabels.clear();
    keyLabels.clear();
    carriedKey = nullptr;
}

void MainWindow::resetLevel3()
{
    doorHitCount = 0;

    if (door) {
        door->setText("X\nX\nX\nX\nX\nX\nX\nX");
        door->setStyleSheet(
            "background-color: #aa2222; color: white; font-weight: bold;"
            "font-size: 14px; letter-spacing: 2px;"
            "border-left: 3px solid #660000; border-right: 3px solid #660000;");
        door->show();
    }

    // restore all locks and keys, drop any carried key
    for (QLabel *lock : lockLabels) if (lock) lock->show();
    for (QLabel *key  : keyLabels)  if (key)  key->show();
    carriedKey = nullptr;
}

//-------------------- For score --------------------
void MainWindow::resetScore()
{
    finalScore = 5000;
}

void MainWindow::startScoreTracking()
{
    resetScore();

    if (scoreTimer) {
        scoreTimer->start();
    }
}

void MainWindow::stopScoreTracking()
{
    if (scoreTimer) {
        scoreTimer->stop();
    }
}

void MainWindow::addPointVisual(const QString &name, const QPoint &pos)
{
    QLabel *point = new QLabel(name, ui->gameCanvasFrame);
    point->setAlignment(Qt::AlignCenter);
    point->setGeometry(pos.x(), pos.y(), 28, 28);

    bool isGoal =
        (goalPoint == "moveToCastle();" && name.toLower() == "g") ||
        (goalPoint == "moveToB();" && name.toLower() == "b") ||
        (goalPoint == "moveToC();" && name.toLower() == "c");

    if (name.toUpper() == "H") {
        point->setStyleSheet(
            "background-color: mediumseagreen;"
            "color: white;"
            "border: 2px solid black;"
            "border-radius: 14px;"
            "font-weight: bold;"
            );
    } else if (isGoal) {
        point->setPixmap(QPixmap(":/images/castle.png"));
        point->setScaledContents(true);
        point->setFixedSize(TILE * 2.5, TILE * 2.5);
        point->show();
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

void MainWindow::setUpLevel4()
{
    QWidget *canvas = ui->gameCanvasFrame;
    canvas->setFixedSize(COLS * TILE, ROWS * TILE);
    clearLevel3Visuals();

    QLabel *bg = new QLabel(canvas);
    bg->setPixmap(QPixmap(":/images/grass.png").scaled(
        canvas->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, canvas->width(), canvas->height());
    bg->lower();
    bg->show();

    startPoint = "start";
    goalPoint  = "moveToC();";
    doorHitCount = 0;
    playerTriggeredMonster = false;
    dragon1Direction = 1;

    levelPointPositions.clear();
    levelPointPositions["start"]      = QPoint(20,  220);
    levelPointPositions["moveToA();"] = QPoint(80,  220);
    levelPointPositions["moveToB();"] = QPoint(260, 220);
    levelPointPositions["moveToH();"] = QPoint(170, 90);
    levelPointPositions["moveToC();"] = QPoint(410, 220);

    addPointVisual("A", levelPointPositions["moveToA();"] + QPoint(6, 6));
    addPointVisual("B", levelPointPositions["moveToB();"] + QPoint(6, 6));
    addPointVisual("H", levelPointPositions["moveToH();"] + QPoint(6, 6));
    addPointVisual("C", levelPointPositions["moveToC();"] + QPoint(6, 6));

    lockLabels.clear();
    for (int i = 0; i < DOOR_HITS_NEEDED; i++) {
        QLabel *lock = new QLabel(canvas);
        lock->setPixmap(QPixmap(":/images/lock.png").scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lock->setGeometry(levelPointPositions["moveToA();"].x() - 10 + i * 26, 160, 22, 22);
        lock->setStyleSheet("background: transparent;");
        lock->show();
        lockLabels.append(lock);
    }

    keyLabels.clear();
    for (int i = 0; i < DOOR_HITS_NEEDED; i++) {
        QLabel *key = new QLabel(canvas);
        key->setPixmap(QPixmap(":/images/key.png").scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        key->setGeometry(levelPointPositions["moveToB();"].x() - 10 + i * 26, 160, 22, 22);
        key->setStyleSheet("background: transparent;");
        key->show();
        keyLabels.append(key);
    }

    carriedKey = nullptr;

    door = new QLabel("X\nX\nX\nX\nX\nX\nX\nX", canvas);
    door->setAlignment(Qt::AlignCenter);
    door->setGeometry(340, 0, 28, canvas->height());
    door->setStyleSheet(
        "background-color: #aa2222; color: white; font-weight: bold;"
        "font-size: 14px; letter-spacing: 2px;"
        "border-left: 3px solid #660000; border-right: 3px solid #660000;");
    door->show();

    princess = new QLabel(canvas);
    princess->setPixmap(QPixmap(":/images/princess.png"));
    princess->setScaledContents(true);
    princess->setFixedSize(TILE, TILE);
    princess->move(levelPointPositions[startPoint]);
    princess->show();

    dragon1 = new QLabel(canvas);
    dragon1->setPixmap(QPixmap(":/images/dragon.png"));
    dragon1->setScaledContents(true);
    dragon1->setFixedSize(TILE, TILE);
    dragon1->move(170, 220);
    dragon1->show();

    showDragonAlertZone();

    if (dragonAlertZone) dragonAlertZone->raise();
    dragon1->raise();
    princess->raise();
    door->raise();
}

QStringList MainWindow::getProgramRoute() const
{
    QStringList route;
    QList<CodeBlock*> blocks = ui->workspaceWidget->getBlocks();

    for (CodeBlock *block : blocks) {
        if (!block) continue;

        if (block->getType() == "routePoint" ||
            block->getType() == "ifStatement" ||
            block->getType() == "forLoop" ||
            (block->getType() == "scopeBracket" && block->text() == "}")) {
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
    dragonMovementEnabled = false;

    if(dragonTimer){
        dragonTimer->stop();
    }

    activeRoute.clear();

    if (currentLevel == 1){
        if (dragon1) {
            dragon1->move(125, 180);
            dragon2->move(265, 180);
        } else if (dragon2) {
            dragon1->move(125, 180);
            dragon2->move(265, 180);
        }
    } else if (currentLevel == 2){
        dragon1->move(230, 180);
    }
    dragon1Direction = -1;
    dragon2Direction = -1;

    // stop for 2s then continue moving
    if (gameStarted) {
        QTimer::singleShot(2000, this, [this]() {
            if (dragonMovementEnabled && gameStarted) {
                dragonTimer->start(20);
            }
        });
    }
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

        // if (won()) {
        //     completeCurrentLevel();
        // }
        return;
    }

    if (activeRoute.isEmpty()) {
        if (moveTimer) {
            moveTimer->stop();
        }

        // if (won()) {
        //     completeCurrentLevel();
        // }
        return;
    }

    QString targetName = activeRoute.first();

    if (currentLevel == 2 &&
        activeRoute.size() >= 2 &&
        (activeRoute[0] == "if (InAlertZone)" || activeRoute[0] == "if (monsterDead)") &&
        levelPointPositions.contains(activeRoute[1])) {

        updateIfLevelLogic();

        bool frontIfTriggered = false;

        if (activeRoute[0] == "if (InAlertZone)") {
            frontIfTriggered = playerTriggeredMonster;
        } else if (activeRoute[0] == "if (monsterDead)") {
            frontIfTriggered = (!dragon1 || !dragon1->isVisible());
        }

        if (frontIfTriggered) {
            activeRoute.removeFirst();
        } else {
            return;
        }
    }

    bool hasGlobalAlertHandler = false;
    if (currentLevel == 4) {
        hasGlobalAlertHandler = activeRoute.contains("if (InAlertZone) -> H");
    }

    bool hasReactiveIf =
        (currentLevel == 2 &&
         activeRoute.size() >= 3 &&
         (activeRoute[1] == "if (InAlertZone)" ||
          activeRoute[1] == "if (monsterDead)") &&
         levelPointPositions.contains(activeRoute[2]));

    auto reactiveIfTriggered = [this]() -> bool {
        if (currentLevel != 2 || activeRoute.size() < 3) {
            return false;
        }

        if (activeRoute[1] == "if (InAlertZone)") {
            return playerTriggeredMonster;
        }

        if (activeRoute[1] == "if (monsterDead)") {
            return (!dragon1 || !dragon1->isVisible());
        }

        return false;
    };

    if (!levelPointPositions.contains(targetName)) {
        activeRoute.removeFirst();
        return;
    }

    QPoint currentPos = princess->pos();
    QPoint targetPos = levelPointPositions[targetName];
    if (targetName == goalPoint) {
        targetPos.setY(targetPos.y() +TILE);
        targetPos.setX(targetPos.x() + TILE / 2);
    }

    double dx = targetPos.x() - currentPos.x();
    double dy = targetPos.y() - currentPos.y();
    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= playerSpeed) {
        princess->move(targetPos);

        if (currentLevel == 2) {
            updateIfLevelLogic();

            if (hasReactiveIf) {
                if (reactiveIfTriggered()) {
                    activeRoute.removeFirst();
                    activeRoute.removeFirst();
                    return;
                } else {
                    activeRoute.removeFirst();
                    if (activeRoute.isEmpty()) {
                        moveTimer->stop();
                        //if (won()) completeCurrentLevel();
                    }
                    return;
                }
            }
        }

        if (currentLevel == 4) {
            updateIfLevelLogic();
        }

        if (currentLevel == 4 &&
            targetName == "moveToH();" &&
            dragon1 &&
            levelPointPositions.contains("moveToA();")) {

            int defaultY = levelPointPositions["moveToA();"].y();

            if (dragon1->y() != defaultY) {
                return;
            }
        }

        if (currentLevel == 3 || currentLevel == 4) {
            // arriving at B: pick up one key (it will follow the princess until she reaches A)
            if (targetName == "moveToB();") {
                if (!keyLabels.isEmpty()) {
                    carriedKey = keyLabels.takeLast();
                }
            }

            // arriving at A while carrying a key: drop the key and remove one lock
            if (targetName == "moveToA();" && carriedKey) {
                carriedKey->hide();
                carriedKey = nullptr;
                doorHitCount++;
                if (!lockLabels.isEmpty()) {
                    QLabel *lock = lockLabels.takeLast();
                    lock->hide();
                }
                if (doorHitCount >= DOOR_HITS_NEEDED && door) {
                    door->hide();
                }
            }

            // if the player tries to go to C before the door is open,
            // snap back to B
            if (targetName == "moveToC();" && doorHitCount < DOOR_HITS_NEEDED) {
                princess->move(levelPointPositions["moveToB();"]);
                activeRoute.removeFirst();
                if (activeRoute.isEmpty()) moveTimer->stop();
                return;
            }
        } else {
            if (isTouchingDragon()) {
                handlePlayerDeath();
                return;
            }
        }

        activeRoute.removeFirst();

        if (activeRoute.isEmpty()) {
            moveTimer->stop();

            // if (won()) {
            //     completeCurrentLevel();
            // }
        }
        return;
    }

    int nextX = static_cast<int>(std::round(currentPos.x() + (dx / distance) * playerSpeed));
    int nextY = static_cast<int>(std::round(currentPos.y() + (dy / distance) * playerSpeed));

    princess->move(nextX, nextY);

    if ((currentLevel == 3 || currentLevel == 4) && door && door->isVisible()) {
        const int wallX = door->x();
        if (princess->x() + princess->width() > wallX) {
            princess->move(wallX - princess->width(), princess->y());
        }
    }

    // carried key floats just above the princess while she's moving
    if ((currentLevel == 3 || currentLevel == 4) && carriedKey) {
        carriedKey->move(princess->x() + princess->width() / 2 - 11,
                         princess->y() - 24);
    }

    if (currentLevel == 2) {
        updateIfLevelLogic();

        if (hasReactiveIf && reactiveIfTriggered()) {
            activeRoute.removeFirst();
            activeRoute.removeFirst();
            return;
        }
    }
    else if (currentLevel == 4) {
        updateIfLevelLogic();

        if (targetName != "moveToH();" &&
            hasGlobalAlertHandler &&
            playerTriggeredMonster) {

            QString interruptedTarget = targetName;

            activeRoute.removeFirst();
            activeRoute.prepend(interruptedTarget);
            activeRoute.prepend("moveToH();");
            return;
        }
    }
    else {
        if (isTouchingDragon()) {
            handlePlayerDeath();
            return;
        }
    }
}

bool MainWindow::isTouchingDragon() const
{
    if (!princess) {
        return false;
    }

    QRect playerRect = princess->geometry();

    if (currentLevel == 4) {
        playerRect = playerRect.adjusted(10, 10, -10, -10);
    }

    if (dragon1 && dragon1->isVisible()) {
        QRect dragon1Rect = dragon1->geometry();

        dragon1Rect = dragon1Rect.adjusted(15, 15, -15, -15);

        if (playerRect.intersects(dragon1Rect)) {
            return true;
        }
    }

    if (dragon2 && dragon2->isVisible()) {
        QRect dragon2Rect = dragon2->geometry();

        if (currentLevel == 4) {
            dragon2Rect = dragon2Rect.adjusted(15, 15, -15, -15);
        }

        if (playerRect.intersects(dragon2Rect)) {
            return true;
        }
    }

    return false;
}

void MainWindow::handlePlayerDeath()
{
    stopScoreTracking();
    on_runButton_clicked();
    //failCurrentLevel();
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
    dragon1->setFixedSize(70, 70);
    dragon1->move(125, 180);
    dragon1->show();

    dragon2 = new QLabel(canvas);
    dragon2->setPixmap(QPixmap(":/images/dragon.png"));
    dragon2->setScaledContents(true);
    dragon2->setFixedSize(70, 70);
    dragon2->move(265, 180);
    dragon2->show();

    // set up how far the dragon moves each time, and how often it triggers a movement.
    dragonTimer = new QTimer(this);

    connect(dragonTimer, &QTimer::timeout, this, [this]() {
        if (currentLevel == 1) {
            moveDragons(3);
        } else {
            moveDragons(2);
        }
    });
}

void MainWindow::dragonStartMove()
{
    dragonMovementEnabled = true;

    if (dragonTimer) {
        dragonTimer->start(20);
    }
}

void MainWindow::moveDragons(int dy){
    // for level 4
    if (currentLevel == 4) {
        if (!dragon1) return;

        const int leftX = levelPointPositions["moveToA();"].x();
        const int rightX = levelPointPositions["moveToB();"].x();
        const int defaultY = levelPointPositions["moveToA();"].y();

        bool playerAtH =
            princess &&
            levelPointPositions.contains("moveToH();") &&
            princess->pos() == levelPointPositions["moveToH();"];

        if (playerAtH) {
            playerTriggeredMonster = false;
        }

        if (playerTriggeredMonster && princess && !playerAtH) {
            QPoint dragonPos = dragon1->pos();
            QPoint playerPos = princess->pos();

            double dx = playerPos.x() - dragonPos.x();
            double dy2 = playerPos.y() - dragonPos.y();
            double distance = std::sqrt(dx * dx + dy2 * dy2);
            const double dragonSpeed = 1.8;

            if (distance > dragonSpeed) {
                int nextX = static_cast<int>(std::round(dragonPos.x() + (dx / distance) * dragonSpeed));
                int nextY = static_cast<int>(std::round(dragonPos.y() + (dy2 / distance) * dragonSpeed));
                dragon1->move(nextX, nextY);
            } else {
                dragon1->move(playerPos);
            }
        }
        else if (dragon1->y() != defaultY) {
            int newY = dragon1->y();

            if (newY < defaultY) {
                newY += dy;
                if (newY > defaultY) newY = defaultY;
            } else if (newY > defaultY) {
                newY -= dy;
                if (newY < defaultY) newY = defaultY;
            }

            dragon1->move(dragon1->x(), newY);
        }
        else {
            int newX = dragon1->x() + dy * dragon1Direction;

            if (newX <= leftX) {
                newX = leftX;
                dragon1Direction = 1;
            } else if (newX >= rightX) {
                newX = rightX;
                dragon1Direction = -1;
            }

            dragon1->move(newX, defaultY);
        }

        if (dragonAlertZone) {
            QPoint center = dragon1->geometry().center();
            dragonAlertZone->move(center.x() - dragonAlertZone->width() / 2,
                                  center.y() - dragonAlertZone->height() / 2);
        }

        if (isTouchingDragon()) {
            handlePlayerDeath();
        }
        return;
    }

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

bool MainWindow::won()
{
    if (!princess || !levelPointPositions.contains(goalPoint)) {
        return false;
    }

    QRect princessRect = princess->geometry();

    // Goal area the same size as the castle
    QRect goalRect(
        levelPointPositions[goalPoint],
        QSize(TILE * 2.5, TILE * 2.5)
        );

    bool reachedGoal = princessRect.intersects(goalRect);

    if (!reachedGoal) {
        return false;
    }

    if (currentLevel == 2) {
        return !dragon1 || !dragon1->isVisible();
    }

    if (currentLevel == 3 || currentLevel == 4) {
        return doorHitCount >= DOOR_HITS_NEEDED;
    }

    return true;
}

// -------------------- Start Page Slots --------------------
void MainWindow::on_startButton_clicked()
{
    on_levelOneButton_clicked();
    // dragonStartMove();
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
    // ui->level_5_Button->setStyleSheet(levelStyle);
}

void MainWindow::setLevelDescriptionStyles()
{
    ui->levelDescriptionLabel->setStyleSheet(
        "border: 3px solid rgb(255, 215, 0);"
        "background-color: rgba(255, 255, 0, 80);"
        "color: black;"
        "border-radius: 12px;"
        );
}

void MainWindow::on_levelOneButton_clicked()
{
    setRunning(false);

    if (moveTimer) moveTimer->stop();
    if (dragonTimer) dragonTimer->stop();
    activeRoute.clear();

    setUpLevel1();

    ui->levelTitleLabel->setText("Level 1");
    ui->stackedWidget->setCurrentIndex(CODE_PAGE);
    // completeCurrentLevel();
    openSpecificLevel(1);
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
    // menu button jiggle — kick the body on mouse enter
    if (hoverEvent->type() == QEvent::Enter) {
        for (ButtonJiggle &jg : jiggleBodies) {
            if (jg.button == levelButton && jg.body) {
                float ix = ((rand() % 201) - 100) / 10.f;  // ±10 units
                float iy = ((rand() % 201) - 100) / 10.f;
                jg.body->ApplyLinearImpulse(b2Vec2(ix, iy), jg.body->GetWorldCenter(), true);
                break;
            }
        }
    }

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
                                               "Highest Score: " + QString::number(saveFile.getScore(1)));
        } else if (levelButton == ui->level_2_Button){
            ui->levelDescriptionLabel->setText("Level Two:\n"
                                               "if statement\n"
                                               "learn that if you see the enemy, wait\n"
                                               "Highest Score: " + QString::number(saveFile.getScore(2)));
        } else if (levelButton == ui->level_3_Button){
            ui->levelDescriptionLabel->setText("Level Three:\n"
                                               "for loop\n"
                                               "Learn how to simplify code by using a loop\n"
                                               "simplify the code you made last round by using loops\n"
                                               "Highest Score: " + QString::number(saveFile.getScore(3)));
        } else if (levelButton == ui->level_4_Button){
            ui->levelDescriptionLabel->setText("Level Four:\n"
                                               "function\n"
                                               "learn that you can put multiple methods\n"
                                               "in a function and call it later\n"
                                               "Highest Score: " + QString::number(saveFile.getScore(4)));
        } else {
            ui->levelDescriptionLabel->setText("Level Check:\n"
                                               "free for all: solve the level however you want\n"
                                               "using the skills that you learned\n"
                                               "try and get the simplest code that completes the same task\n"
                                               "Highest Score: " + QString::number(saveFile.getScore(5)));
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
    // ui->level_5_Button->setEnabled(saveFile.isLevelUnlocked(5));
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

    setRunning(false);

    if (moveTimer) moveTimer->stop();
    if (dragonTimer) dragonTimer->stop();
    activeRoute.clear();
    gameStarted = false;

    ui->workspaceWidget->clearBlocks();

    if (levelNumber == 1) {
        currentLevelRoutePoints = {
            "moveToA();", "moveToB();", "moveToC();",
            "moveToD();", "moveToE();", "moveToF();", "moveToCastle();"
        };
        createRoutePointBlocks();
        setUpLevel1();
        resetPrincessToStart();
        resetDragonsToStart();
        QTimer::singleShot(0, this, &MainWindow::on_hintButton_clicked);
    }
    else if (levelNumber == 2) {
        currentLevelRoutePoints = { "moveToA();", "moveToB();", "if (InAlertZone)", "if (monsterDead)"};
        createRoutePointBlocks();
        setUpLevel2();
        resetPrincessToStart();
        QTimer::singleShot(0, this, &MainWindow::on_hintButton_clicked);
    }
    else if (levelNumber == 3) {
        currentLevelRoutePoints = {
            "moveToA();", "moveToB();", "moveToC();",
            "for(3)", "for(2)", "{", "}"
        };
        createRoutePointBlocks();
        setUpLevel3();
        resetPrincessToStart();
        QTimer::singleShot(0, this, &MainWindow::on_hintButton_clicked);
    }
    else if (levelNumber == 4) {
        currentLevelRoutePoints = {
            "moveToA();", "moveToB();", "moveToC();",
            "if (InAlertZone) -> H",
            "for(2)", "{", "}"
        };
        createRoutePointBlocks();
        setUpLevel4();
        resetPrincessToStart();
        QTimer::singleShot(0, this, &MainWindow::on_hintButton_clicked);
    }
}

void MainWindow::completeCurrentLevel()
{
    stopScoreTracking();
    setRunning(false);

    saveFile.updateScore(currentLevel, finalScore);
    saveFile.save();

    int unlockedLevel = saveFile.getLatestUnlocked();

    // only unlock up to level 4 — that's all we have for now
    if (unlockedLevel < 4) {
        if (currentLevel == unlockedLevel) {
            saveFile.updateUnlockedLevels(unlockedLevel + 1);
            saveFile.save();
        }
    }

    unlockLevelButtons();

    // If the current level is cleared, a window pops up (containing text, the score, and options).
    emit levelCompleted();
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Level Complete!");
    msgBox.setText("Congratulations! You completed this level!\n"
                   "Your total score is " + QString::QString::number(finalScore));

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
        if (currentLevel >= 4) {
            // show a styled "coming soon" dialog instead of trying to open level 5
            QDialog done(this);
            done.setWindowTitle("You finished it!");
            done.setFixedSize(380, 200);
            done.setStyleSheet(
                "QDialog {"
                "  background-color: #0d0d1a;"
                "  border: 2px solid #ffd700;"
                "  border-radius: 12px;"
                "}");

            QVBoxLayout *vl = new QVBoxLayout(&done);
            vl->setContentsMargins(28, 24, 28, 24);
            vl->setSpacing(12);

            QLabel *title = new QLabel("You've completed all levels!", &done);
            title->setStyleSheet("color: #ffd700; font-size: 15px; font-weight: bold; background: transparent;");
            title->setWordWrap(true);
            title->setAlignment(Qt::AlignCenter);

            QLabel *body = new QLabel("More levels are coming soon.\nStay tuned!", &done);
            body->setStyleSheet("color: #c8d8e8; font-size: 13px; background: transparent;");
            body->setAlignment(Qt::AlignCenter);

            QPushButton *okBtn = new QPushButton("Back to Menu", &done);
            okBtn->setFixedHeight(34);
            okBtn->setStyleSheet(
                "QPushButton {"
                "  background-color: #1a1a2e; color: #ffd700;"
                "  border: 1px solid #ffd700; border-radius: 6px;"
                "  font-size: 13px; font-weight: bold;"
                "}"
                "QPushButton:hover { background-color: #252550; }");
            connect(okBtn, &QPushButton::clicked, &done, &QDialog::accept);

            vl->addWidget(title);
            vl->addWidget(body);
            vl->addStretch();
            vl->addWidget(okBtn);

            done.exec();
            ui->stackedWidget->setCurrentIndex(START_PAGE);
        } else {
            openSpecificLevel(currentLevel + 1);
        }
    } else if (msgBox.clickedButton() == quitButton) {
        ui->stackedWidget->setCurrentIndex(START_PAGE);
    }
}

void MainWindow::failCurrentLevel()
{
    stopScoreTracking();
    setRunning(false);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("You Failed!");
    msgBox.setText("You Failed this level!\n");

    // Create three option buttons.
    QPushButton *restartButton = msgBox.addButton("Restart", QMessageBox::ActionRole);
    QPushButton *chooseLevelButton = msgBox.addButton("Choose Level", QMessageBox::ActionRole);
    QPushButton *quitButton = msgBox.addButton("Quit", QMessageBox::ActionRole);

    msgBox.exec();

    // Implement the functionality of each button.
    if (msgBox.clickedButton() == restartButton) {
        ui->workspaceWidget->clearBlocks();
        openSpecificLevel(currentLevel);
    } else if (msgBox.clickedButton() == chooseLevelButton) {
        ui->stackedWidget->setCurrentIndex(LEVEL_PAGE);
    } else if (msgBox.clickedButton() == quitButton) {
        ui->stackedWidget->setCurrentIndex(START_PAGE);
    }
}

// -------------------- Game Page Slots --------------------
void MainWindow::on_backToLevelsButton_clicked()
{
    stopScoreTracking();
    ui->stackedWidget->setCurrentIndex(LEVEL_PAGE);
}

void MainWindow::on_runButton_clicked()
{
    if (running) {
        // reset
        setRunning(false);

        if (moveTimer) moveTimer->stop();
        if (dragonTimer) dragonTimer->stop();
        stopScoreTracking();

        activeRoute.clear();
        resetPrincessToStart();
        if (currentLevel == 1) {
            resetDragonsToStart();
        } else if (currentLevel == 2) {
            resetLevel2();
        } else if (currentLevel == 3) {
            setUpLevel3();
            resetPrincessToStart();
        }else if (currentLevel == 4) {
            setUpLevel4();
        }
        return;
    }

    QStringList rawRoute = getProgramRoute();
    QStringList route = (currentLevel == 3 || currentLevel == 4) ? expandForLoops(rawRoute) : rawRoute;

    setRunning(true);

    gameStarted = true;
    startScoreTracking();
    startPlayerMovement(route);
    if (currentLevel == 1 || currentLevel == 4) {
        dragonStartMove();
    }
}

void MainWindow::on_submitButton_clicked() {
    if (won()) {
        completeCurrentLevel();
    } else {
        failCurrentLevel();
    }
}

void MainWindow::setRunning(bool isRunning) {
    running = isRunning;
    ui->runButton->setText(isRunning ? "Reset" : "Run");
    ui->hintButton->setDisabled(isRunning);
    ui->clearButton->setDisabled(isRunning);
    ui->backToLevelsButton->setDisabled(isRunning);
    ui->submitButton->setDisabled(!isRunning);
}

void MainWindow::on_clearButton_clicked()
{
    ui->workspaceWidget->clearBlocks();
}

// This part's text content and symbols are generated by AI, the structure is inspired by ai
void MainWindow::on_hintButton_clicked()
{
    // each level has 3 sections: concept, mechanics, path hint
    struct HintPage {
        QString subtitle;
        QString body;
    };
    struct LevelHint {
        QString title;
        QVector<HintPage> pages;
    };

    static const LevelHint hints[] = {
                                      // Level 1
                                      {
                                          "Level 1 — Methods",
                                          {
                                              {
                                                  "What you'll learn",
                                                  "A <b>method</b> is a named action — when you call it, something happens.<br><br>"
                                                  "<span style='color:#90ee90;'>moveToA();</span> tells the princess to walk to point A. "
                                                  "Each block in your workspace is one method call.<br><br>"
                                                  "Blocks run <b>top to bottom</b>, one at a time — just like lines in a real program."
                                              },
                                              {
                                                  "How the game works",
                                                  "Drag blocks from <i>Blocks</i> into <i>Your Code</i> and press <b>Run</b>.<br><br>"
                                                  "Two dragons patrol <b>up and down</b> along fixed columns. "
                                                  "They never leave those columns.<br><br>"
                                                  "If the princess touches a dragon at any point, the run resets."
                                              },
                                              {
                                                  "Finding a path",
                                                  "<i>The dragons only cover certain vertical columns.<br><br>"
                                                  "Some points sit above or below them — and some are completely outside their range.<br><br>"
                                                  "A route that avoids both patrol columns the whole way will reach the castle safely.</i>"
                                              }
                                          }
                                      },
                                      // Level 2
                                      {
                                          "Level 2 — if Statements",
                                          {
                                              {
                                                  "What you'll learn",
                                                  "An <b>if statement</b> lets your code make a decision.<br><br>"
                                                  "Place a destination block, then an <span style='color:#dd88ff;'>if</span> block below it. "
                                                  "When the princess arrives, the condition is checked:<br><br>"
                                                  "· Condition <b>true</b> → next destination is <b>skipped</b><br>"
                                                  "· Condition <b>false</b> → next destination is <b>visited</b>"
                                              },
                                              {
                                                  "How the game works",
                                                  "The dragon starts chasing the princess once she enters its <b>red alert zone</b>.<br><br>"
                                                  "The guard near A will <b>eliminate any dragon</b> that wanders into its gold zone.<br><br>"
                                                  "The goal is at B — but the dragon is in the way."
                                              },
                                              {
                                                  "Finding a path",
                                                  "<i>The dragon won't move on its own — something has to lure it toward the guard.<br><br>"
                                                  "Think about what draws the dragon out, and what condition tells you it's safe to proceed.<br><br>"
                                                  "Once the obstacle is gone, the path to B is open.</i>"
                                              }
                                          }
                                      },
                                      // Level 3
                                      {
                                          "Level 3 — for Loops",
                                          {
                                              {
                                                  "What you'll learn",
                                                  "A <b>for loop</b> repeats actions a fixed number of times.<br><br>"
                                                  "Use <span style='color:#88ff88;'>for(N)</span> with blocks wrapped in "
                                                  "<span style='color:#ffd700;'>{</span> and <span style='color:#ffd700;'>}</span> "
                                                  "to repeat them N times.<br><br>"
                                                  "Anything <b>outside</b> the braces runs once, after the loop finishes."
                                              },
                                              {
                                                  "How the game works",
                                                  "There are <b>2 locks</b> above A and <b>2 keys</b> above B.<br><br>"
                                                  "Each time the princess reaches B, she picks up a key — it follows her. "
                                                  "When she returns to A with a key, the key and one lock disappear.<br><br>"
                                                  "Once both locks are gone, the red wall opens and C is reachable."
                                              },
                                              {
                                                  "Finding a path",
                                                  "<i>Which steps need to repeat, and which step only happens once at the end?<br><br>"
                                                  "The loop count should match exactly how many exchanges need to happen.<br><br>"
                                                  "Once the last lock falls, one final move reaches the goal.</i>"
                                              }
                                          }
                                      },
                                      // Level 4
                                      {
                                          "Level 4 — Putting It Together",
                                          {
                                              {
                                                  "What you'll learn",
                                                  "Real programs combine concepts. This level uses both a <b>for loop</b> and an <b>if statement</b>.<br><br>"
                                                  "The loop handles a repeating task. The if statement reacts to something unexpected happening during that loop."
                                              },
                                              {
                                                  "How the game works",
                                                  "Same key-and-lock mechanic: pick up a key at B, return to A to unlock.<br><br>"
                                                  "A <b>dragon patrols</b> between A and B. Entering its red zone triggers a chase. "
                                                  "Point <b>H</b> is a safe hiding spot — reaching it resets the dragon.<br><br>"
                                                  "<span style='color:#dd88ff;'>if (InAlertZone) -&gt; H</span> automatically redirects "
                                                  "the princess to H when chased, then resumes from where she left off."
                                              },
                                              {
                                                  "Finding a path",
                                                  "<i>The loop handles the exchange — but the dragon may interrupt at any step.<br><br>"
                                                  "The if block needs to be placed where it can catch a chase mid-loop.<br><br>"
                                                  "Once both locks are gone, one final move reaches the castle.</i>"
                                              }
                                          }
                                      },
                                      };

    int idx = qBound(1, currentLevel, 5) - 1;
    const LevelHint &hint = hints[idx];

    int totalPages = 0;
    for (const auto &p : hint.pages)
        if (!p.body.isEmpty()) totalPages++;

    QDialog dialog(this);
    dialog.setWindowTitle("Hint");
    dialog.setFixedSize(440, 320);
    dialog.setStyleSheet(
        "QDialog {"
        "  background-color: #0d0d1a;"
        "  border: 2px solid #ffd700;"
        "  border-radius: 12px;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(10);

    QHBoxLayout *topRow = new QHBoxLayout;

    QLabel *titleLabel = new QLabel(hint.title, &dialog);
    titleLabel->setStyleSheet(
        "color: #ffd700; font-size: 14px; font-weight: bold; background: transparent;");

    QPushButton *closeBtn = new QPushButton("X", &dialog);
    closeBtn->setFixedSize(24, 24);
    closeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #888; border: none; font-size: 14px; }"
        "QPushButton:hover { color: #ffd700; }");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    topRow->addWidget(titleLabel);
    topRow->addStretch();
    topRow->addWidget(closeBtn);
    mainLayout->addLayout(topRow);

    QLabel *subtitleLabel = new QLabel(&dialog);
    subtitleLabel->setStyleSheet(
        "color: #ffd700; font-size: 12px; font-weight: bold; background: transparent;");

    QLabel *bodyLabel = new QLabel(&dialog);
    bodyLabel->setTextFormat(Qt::RichText);
    bodyLabel->setWordWrap(true);
    bodyLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    bodyLabel->setStyleSheet(
        "color: #c8d8e8; font-size: 12px; background: transparent;");

    mainLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(bodyLabel, 1);

    QHBoxLayout *navRow = new QHBoxLayout;

    QString arrowStyle =
        "QPushButton { background: transparent; color: #888; border: none; font-size: 18px; padding: 0 6px; }"
        "QPushButton:hover { color: #ffd700; }"
        "QPushButton:disabled { color: #333; }";

    QPushButton *prevBtn = new QPushButton("<<", &dialog);
    prevBtn->setFixedSize(32, 28);
    prevBtn->setStyleSheet(arrowStyle);

    QPushButton *nextBtn = new QPushButton(">>", &dialog);
    nextBtn->setFixedSize(32, 28);
    nextBtn->setStyleSheet(arrowStyle);

    QLabel *dotsLabel = new QLabel(&dialog);
    dotsLabel->setAlignment(Qt::AlignCenter);
    dotsLabel->setStyleSheet("background: transparent; color: #ffd700; font-size: 11px;");

    navRow->addWidget(prevBtn);
    navRow->addStretch();
    navRow->addWidget(dotsLabel);
    navRow->addStretch();
    navRow->addWidget(nextBtn);
    mainLayout->addLayout(navRow);

    // page state
    int *currentPage = new int(0);

    auto updatePage = [&]() {
        const HintPage &page = hint.pages[*currentPage];
        subtitleLabel->setText(page.subtitle);
        bodyLabel->setText(page.body);

        // dots indicator
        QString dots;
        for (int i = 0; i < totalPages; i++)
            dots += (i == *currentPage) ? "●" : "○";
        dotsLabel->setText(dots);

        prevBtn->setEnabled(*currentPage > 0);
        nextBtn->setEnabled(*currentPage < totalPages - 1);
    };

    connect(prevBtn, &QPushButton::clicked, [&]() {
        if (*currentPage > 0) { (*currentPage)--; updatePage(); }
    });
    connect(nextBtn, &QPushButton::clicked, [&]() {
        if (*currentPage < totalPages - 1) { (*currentPage)++; updatePage(); }
    });

    updatePage();
    dialog.exec();
    delete currentPage;
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

    gameStarted = false;
    resetPrincessToStart();
    resetDragonsToStart();
}

void MainWindow::createRoutePointBlocks()
{
    for (CodeBlock *block : routePointBlocks) {
        if (block) {
            ui->blockSpaceWidget->getLayout()->removeWidget(block);
            block->deleteLater();
        }
    }
    routePointBlocks.clear();

    for (const QString &pointName : currentLevelRoutePoints) {
        QString blockType = pointName.startsWith("if")    ? "ifStatement"
                            : pointName.startsWith("for(")  ? "forLoop"
                            : (pointName == "{" || pointName == "}") ? "scopeBracket"
                                                                     : "routePoint";
        CodeBlock *pointBlock = new CodeBlock(pointName, blockType);
        ui->blockSpaceWidget->addBlock(pointBlock);
        //pointBlock->hide();

        // connect(pointBlock, &CodeBlock::blockClicked,
        //         this, &MainWindow::on_route_point_block_clicked);

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
    setRoutePointsVisible(true);
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
