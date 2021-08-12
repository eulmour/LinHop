#pragma once

union GameData
{
    struct
    {
        long maxScoreClassic;
        long maxScoreHidden;
        long fxEnabled;
        long unlockResizing;
        
        union
        {
            long musicVolume;
            float musicVolumeFloat;
        };
    };

    long param[5];
};

GameData LoadGameData();
void SaveGameData(GameData& gameData);
