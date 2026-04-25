/**
 * @file saveFile.h
 * @brief Header file for the SaveFile class.
 * @author BitCat
 *
 * SaveFile manages player progress and score data for the game.
 * It stores unlocked levels and level scores using JSON data
 * written to and loaded from a text file.
 */

#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <vector>
#include <QJsonObject>

using std::vector;
/**
 * SaveFile class, which handles save data. It saves the file
 * in the form of a JSON, which is stored in a .txt file.
 * @brief The SaveFile class
 */
class SaveFile
{
private:
    /**
     * The vector containing all of the scores.
     * @brief scores
     */
    vector<int> scores;
    /**
     * The latest unlocked level that the player can access.
     * @brief latestUnlockedIndex
     */
    int latestUnlockedIndex;

public:
    SaveFile();

    /**
     * Saves the SaveFile to 'save.txt'. If there is no Savefile, it will create a new one.
     * @brief save Saves the SaveFile to a txt.
     */
    void save();
    /**
     * Loads the savefile from an existing save. If there is no existing save, then it will create a new one.
     * @brief load loads the saveFile
     */
    void load();

    /**
     * Converts this to a JSON object.
     * @brief toJSON JSON conversion
     * @return the savefile as a JSON.
     */
    QJsonObject toJSON() const;

    /**
     * Gets the latest unlocked level.
     * @brief getLatestUnlocked
     * @return the latest unlocked level
     */
    int getLatestUnlocked();
    /**
     * Checks whether the specified level is unlocked.
     * @brief isLevelUnlocked
     * @param levelIndex the specified level
     * @return whether the level is unlocked yet
     */
    bool isLevelUnlocked(int levelIndex);
    /**
     * Updates which levels are unlocked.
     * @brief updateUnlockedLevels
     * @param levelIndex the new level index that is unlocked
     */
    void updateUnlockedLevels(int levelIndex);
    /**
     * Updates the score of a specified level.
     * @brief updateScore
     * @param levelIndex Which level to update the score for
     * @param score The score
     */
    void updateScore(int levelIndex, int score);
    /**
     * Gets the score of a specified level.
     * @brief getScore
     * @param levelIndex the level
     * @return the score
     */
    int getScore(int levelIndex);
    /**
     * Resets all scores.
     * @brief resetScores
     */
    void resetScores();
};

#endif // SAVEFILE_H
