#include <SFML/Graphics.hpp>

#include "Speljongen.hpp"

#ifdef __LINUX__
#include <X11.h>
#endif

int main()
{   
    Speljongen gameboy;

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
            else if (evt.type == sf::Event::KeyReleased
                && evt.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }
        }

        gameboy.tick();

        window.clear(sf::Color::Blue);
        window.draw(gameboy);
        window.display();
    }

    return 0;
}