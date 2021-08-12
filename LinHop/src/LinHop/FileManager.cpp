#include "FileManager.h"
#include <string>
#include <fstream>

GameData LoadGameData()
{
    GameData gameData =
    {
        .maxScoreClassic = 0L,
        .maxScoreHidden = 0L,
        .fxEnabled = 1L,
        .unlockResizing = 0L,
        .musicVolumeFloat = 0.5f
    };

    std::ifstream file("LinHopSaved.ini");

    if (file.is_open())
    {
        unsigned char i = 0;
        for (std::string line; std::getline(file, line);)
		{
            gameData.param[i] = std::stol(line);
            ++i;
		}
    }

    file.close();

    return gameData;
}

void SaveGameData(GameData& gameData)
{
    std::ofstream file("LinHopSaved.ini", std::ofstream::out);

    for (long& param : gameData.param)
    {
        file << std::to_string(param) << std::endl;
    }

    file.close();
}