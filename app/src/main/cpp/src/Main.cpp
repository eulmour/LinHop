#include "Main.hpp"
#include "MainScene.hpp"

class Application : public Game {

public:
    std::unique_ptr<EngineConfig> config() override {
        auto& builder = (new EngineConfig())
            ->windowConfig(Window::Config()
                .title("LinHop")
                .innerSize(450, 800)
                .resizeable(false)
                .vsync(true)
            );

        return std::unique_ptr<EngineConfig>(&builder);
    }

    void init(Engine& e) override {
        try {
			e.pushScene(std::make_unique<MainScene>(e));
        } catch (std::exception& exception) {
            e.log() << exception.what() << "\n";
            e.show_log();
        }
    }
};

/**
 * This is the main entry point
 */
ENGINE_ENTRY(Application)
