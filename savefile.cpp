#include "savefile.h"


#include "QFile"
#include "QDebug"
#include "QJsonArray"

QString filename = "";

SaveFile::SaveFile() {
    latestUnlockedIndex = 1; // level 1 is always unlocked
}





void SaveFile::save(){

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing";
        return;
    }

    QJsonDocument doc(toJSON());

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

void SaveFile::load() {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "No save detected!";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    // Parse JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }

    QJsonObject obj = doc.object();

    latestUnlockedIndex = obj["unlockedLevels"].toInt();
}

QJsonObject SaveFile::toJSON() const {
    QJsonArray scoreArray;
    for (const auto &score : scores) {
        scoreArray.append(score);
    }

    QJsonObject obj;

    obj["unlockedLevels"] = latestUnlockedIndex;
    obj["scores"] = scoreArray;

    return obj;
}

int SaveFile::getLatestUnlocked(){
    return latestUnlockedIndex;
}

bool SaveFile::isLevelUnlocked(int levelIndex){
    return (levelIndex <= latestUnlockedIndex);
}

void SaveFile::updateUnlockedLevels(int levelIndex){
    latestUnlockedIndex = levelIndex;
}

void SaveFile::updateScore(int levelIndex, int score){
    scores[levelIndex] = score;
}

int SaveFile::getScore(int levelIndex){
    return scores[levelIndex];
}
