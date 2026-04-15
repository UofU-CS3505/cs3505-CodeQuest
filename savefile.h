#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <vector>
#include <QJsonObject>

using std::vector;

class SaveFile
{
private:
    vector<int> scores;
    int latestUnlockedIndex;

public:
    SaveFile();

    void save();
    void load();

    void loadJSON() const;
    QJsonObject toJSON() const;

    int getLatestUnlocked();
    bool isLevelUnlocked(int levelIndex);
    void updateUnlockedLevels(int levelIndex);
    void updateScore(int levelIndex, int score);
    int getScore(int levelIndex);
};

#endif // SAVEFILE_H
