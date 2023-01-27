#include "Main.hpp"
#include "MainScene.hpp"

class Application : public wuh::Game {

public:
    std::unique_ptr<wuh::EngineConfig> config() override {
        auto& builder = (new wuh::EngineConfig())
            ->window(wuh::Window::Config()
                .title("LinHop")
                .size(450, 800)
                .resizeable(false)
                .vsync(true)
            );

        return std::unique_ptr<wuh::EngineConfig>(&builder);
    }

    void init(wuh::Engine& e) override {
        try {
			e.push_activity(std::make_unique<MainScene>(e));
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
