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
#ifdef __LINUX__
    XInitThreads();
#endif    

    Speljongen gameboy;

    sf::RenderWindow window;
    window.create({ 800, 600 }, "Speljongen", sf::Style::Close | sf::Style::Titlebar);
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
                default: break;
                }
            }
            gameboy.handleEvent(evt);
        }

#ifndef USE_THREADING

        gameboy.update();
#endif

        ImGui::SFML::Update(window, imguiClock.restart());

        gameboy.doImgui();

        window.clear(sf::Color(100, 149, 237));
        //gameboy.lockDisplay(); //hm. kills perf
        ImGui::SFML::Render(window);
        //gameboy.freeDisplay();
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}