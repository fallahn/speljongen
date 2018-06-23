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
#if defined __LINUX__ && defined USE_THREADING
    XInitThreads();
#endif    

    bool fullScreen = false;

    sf::RenderWindow window;
    window.create({ 800, 600 }, "Speljongen", sf::Style::Close | sf::Style::Titlebar);
    window.setVerticalSyncEnabled(true);

    ImGui::SFML::Init(window);

    Speljongen gameboy;

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
                case sf::Keyboard::F:
                    fullScreen = !fullScreen;
                    gameboy.toggleFullscreen();
                    if (fullScreen)
                    {
                        auto mode = sf::VideoMode::getDesktopMode();
                        window.create(mode, "Speljongen", sf::Style::Fullscreen);
                    }
                    else
                    {
                        window.create({ 800, 600 }, "Speljongen", sf::Style::Close | sf::Style::Titlebar);
                    }
                    break;
                default: break;
                }
            }
            gameboy.handleEvent(evt);
        }

#ifndef USE_THREADING
        gameboy.update();
#else
        gameboy.updateDebugger();
#endif
        
        if (fullScreen)
        {
            window.clear();
            window.draw(gameboy);
            window.display();
        }
        else
        {
            ImGui::SFML::Update(window, imguiClock.restart());
            gameboy.doImgui();

            window.clear(sf::Color(100, 149, 237));
            ImGui::SFML::Render(window);
            window.display();
        }
    }

    ImGui::SFML::Shutdown();
    return 0;
}