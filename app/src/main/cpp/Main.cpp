#include "Main.hpp"
#include "game/MainScene.hpp"
//#include "game/SettingsScene.hpp"

class Application : public SpigeApplication {

public:
    std::unique_ptr<EngineConfig> config() override {
        auto& builder = (new EngineConfig())
            ->windowTitle("LinHop");
//            .windowResizeable(false)
//            .windowMaximized(false)
//            .windowVsync(true);

        return std::unique_ptr<EngineConfig>(&builder);
    }

    void init(Engine& e) override {
        e.setScene(new MainScene(e));
    }
};

/**
 * This is the main entry point
 */
SPIGE_ENTRY(Application)
