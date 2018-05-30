#include <SFML/Graphics.hpp>

#include "Speljongen.hpp"
#include "nfd/nfd.h"

#ifdef __LINUX__
#include <X11.h>
#endif

int main()
{   
    Speljongen gameboy;
    bool run = false;

    sf::RenderWindow window;
    window.create({ 800, 600 }, "Speljongen");
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event evt;
        while (window.pollEvent(evt))
        {
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
                    //window.setVerticalSyncEnabled(run);
                    run = !run;
                    if (run)
                    {
                        gameboy.start();
                    }
                    else
                    {
                        gameboy.stop();
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

        window.clear(sf::Color::Blue);
        window.draw(gameboy);
        window.display();

        window.setTitle("Speljongen " + std::to_string(1.f / clock.restart().asSeconds()));
    }

    return 0;
}