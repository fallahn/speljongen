#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window;
    window.create({ 800, 600 }, "SpelJong");
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

        window.clear(sf::Color::Blue);
        window.display();
    }

    return 0;
}