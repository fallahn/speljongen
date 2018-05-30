#include <SFML/Graphics.hpp>

#include "Speljongen.hpp"
#include "nfd/nfd.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#ifdef __LINUX__
#include <X11.h>
#endif

int main()
{   
    Speljongen gameboy;
    bool run = false;

    sf::RenderWindow window;
    window.create({ 800, 600 }, "Speljongen: STOPPED");
    window.setVerticalSyncEnabled(true);

    ImGui::SFML::Init(window);

    sf::Clock imguiClock;
    while (window.isOpen())
    {
        sf::Event evt;
        while (window.pollEvent(evt))
        {
            ImGui::SFML::ProcessEvent(evt);

            if (evt.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (evt.type == sf::Event::KeyReleased)
            { 
                switch (evt.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::LControl:
                    run = !run;
                    if (run)
                    {
                        gameboy.start();
                        window.setTitle("Speljongen: STARTED");
                    }
                    else
                    {
                        window.setTitle("Speljongen: STOPPING...");
                        gameboy.stop();
                        window.setTitle("Speljongen: STOPPED");
                    }
                    break;
                default: break;
                }
            }
            else if (evt.type == sf::Event::KeyPressed)
            {
                switch (evt.key.code)
                {
                default: break;
                case sf::Keyboard::Space:
                    if (!run)
                    {
                        gameboy.step();
                    }
                    break;
                }
            }
            else if (evt.type == sf::Event::MouseButtonReleased)
            {
                if (evt.mouseButton.button == sf::Mouse::Right)
                {
                    gameboy.stop();
                    gameboy.reset();
                    run = false;
                    
                    //TODO check also inside window
                    nfdchar_t *outPath = nullptr;
                    nfdresult_t result = NFD_OpenDialog("gb", nullptr, &outPath);
                    if (result == NFD_OKAY)
                    {
                        gameboy.load(outPath);
                        free(outPath);
                    }
                }
            }
        }

        ImGui::SFML::Update(window, imguiClock.restart());

        gameboy.doImgui();

        window.clear(sf::Color::Blue);
        window.draw(gameboy);
        gameboy.lockDisplay();
        ImGui::SFML::Render(window);
        gameboy.freeDisplay();
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}