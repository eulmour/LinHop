#include "Main.hpp"
#include "game/MainScene.hpp"

class Application : public SpigeApplication {

public:
    std::unique_ptr<EngineConfig> config() override {
        auto& builder = (new EngineConfig())
            ->windowConfig(Window::Config()
                .title("LinHop")
                .innerSize(480, 800)
                .resizeable(false)
                .vsync(true)
            );

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
