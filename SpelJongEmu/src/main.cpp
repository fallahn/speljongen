#include <SFML/Graphics.hpp>

#include "Speljongen.hpp"

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
                    run = !run;
                    break;
                case sf::Keyboard::Space:
                    if (!run)
                    {
                        gameboy.tick();
                    }
                    break;
                default: break;
                }
            }
        }

        if(run) gameboy.tick();

        window.clear(sf::Color::Blue);
        window.draw(gameboy);
        window.display();
    }

    return 0;
}