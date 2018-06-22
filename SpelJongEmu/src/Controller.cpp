#include "Controller.hpp"
#include "InterruptManager.hpp"
#include "Mmu.hpp"

#include <SFML/Window/Event.hpp>

namespace
{
    const std::uint16_t InputRegister = 0xff00;
    const float Deadzone = 20.f;
    const float ImageWidth = 160.f;
    const float ImageHeight = 128.f;

    enum Control
    {
        Right = 0x01,
        Left = 0x2,
        Up = 0x4,
        Down = 0x8,
        A = 0x10,
        B = 0x20,
        Select = 0x40,
        Start = 0x80
    };

	/*
	           Bit4       Bit5
				|          |
	Bit0 -----Right--------A-------
	            |          |
	Bit1 ------Left--------B-------
	            |          |
	Bit2 -------Up-------Select----
	            |          |
	Bit3 ------Down------Start-----
	
    Bits are normally high and go low when button is pressed

	*/

    struct Quad final
    {
        std::array<sf::Vertex, 4u> vertices = {};
    };
    Quad imageQuad;
    std::array<Quad, 8u> buttonQuads = {};
}

Controller::Controller(InterruptManager& im, Mmu& mmu)
    : m_inputMask		(0xff),
    m_controllerMask    (0xff),
	m_lastMask			(0xff),
	m_interruptManager	(im),
    m_mmu				(mmu)
{
    if (!m_buttonTexture.loadFromFile("assets/controls.png"))
    {
        sf::Image img;
        img.create(static_cast<sf::Uint32>(ImageWidth), static_cast<sf::Uint32>(ImageHeight), sf::Color::Magenta);
        m_buttonTexture.loadFromImage(img);
    }

    imageQuad.vertices[0] = sf::Vertex(sf::Vector2f(), sf::Vector2f());
    imageQuad.vertices[1] = sf::Vertex(sf::Vector2f(ImageWidth, 0.f), sf::Vector2f(ImageWidth, 0.f));
    imageQuad.vertices[2] = sf::Vertex(sf::Vector2f(ImageWidth, ImageHeight), sf::Vector2f(ImageWidth, ImageHeight));
    imageQuad.vertices[3] = sf::Vertex(sf::Vector2f(0.f, ImageHeight), sf::Vector2f(0.f, ImageHeight));


    auto setQuad = [](Quad& quad, sf::FloatRect rect)
    {
        quad.vertices[0] = sf::Vertex(sf::Vector2f(rect.left - ImageWidth, rect.top), sf::Vector2f(rect.left, rect.top));
        quad.vertices[1] = sf::Vertex(sf::Vector2f((rect.left + rect.width) - ImageWidth, rect.top), sf::Vector2f(rect.left + rect.width, rect.top));
        quad.vertices[2] = sf::Vertex(sf::Vector2f((rect.left + rect.width) - ImageWidth, (rect.top + rect.height)), sf::Vector2f(rect.left + rect.width, (rect.top + rect.height)));
        quad.vertices[3] = sf::Vertex(sf::Vector2f((rect.left) - ImageWidth, (rect.top + rect.height)), sf::Vector2f(rect.left, (rect.top + rect.height)));
    };

    setQuad(buttonQuads[0], { 215.f, 54.f, 25.f, 27.f });
    setQuad(buttonQuads[1], { 168.f, 54.f, 25.f, 27.f });
    setQuad(buttonQuads[2], { 191.f, 29.f, 25.f, 27.f });
    setQuad(buttonQuads[3], { 191.f, 79.f, 25.f, 27.f });
    setQuad(buttonQuads[4], { 279.f, 64.f, 35.f, 34.f });
    setQuad(buttonQuads[5], { 243.f, 83.f, 35.f, 34.f });
    setQuad(buttonQuads[6], { 243.f, 20.f, 26.f, 25.f });
    setQuad(buttonQuads[7], { 278.f, 20.f, 26.f, 25.f });

    m_texture.create(static_cast<sf::Uint32>(ImageWidth), static_cast<sf::Uint32>(ImageHeight));
    updateTexture();
}

//public
void Controller::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyReleased)
    {
        switch(evt.key.code)
        {
            default: break;
            case sf::Keyboard::W:
                m_inputMask |= Up;
                break;
            case sf::Keyboard::S:
                m_inputMask |= Down;
                break;
            case sf::Keyboard::A:
                m_inputMask |= Left;
                break;
            case sf::Keyboard::D:
                m_inputMask |= Right;
                break;
            case sf::Keyboard::N:
                m_inputMask |= B;
                break;
            case sf::Keyboard::M:
                m_inputMask |= A;
                break;
            case sf::Keyboard::RShift:
                m_inputMask |= Select;
                break;
            case sf::Keyboard::Return:
                m_inputMask |= Start;
                break;
        }
	}
    else if (evt.type == sf::Event::KeyPressed)
    {
        switch (evt.key.code)
        {
        default: break;
        case sf::Keyboard::W:
            m_inputMask &= ~Up;
            break;
        case sf::Keyboard::S:
            m_inputMask &= ~Down;
            break;
        case sf::Keyboard::A:
            m_inputMask &= ~Left;
            break;
        case sf::Keyboard::D:
            m_inputMask &= ~Right;
            break;
        case sf::Keyboard::N:
            m_inputMask &= ~B;
            break;
        case sf::Keyboard::M:
            m_inputMask &= ~A;
            break;
        case sf::Keyboard::RShift:
            m_inputMask &= ~Select;
            break;
        case sf::Keyboard::Return:
            m_inputMask &= ~Start;
            break;
        }
    }

    else if (evt.type == sf::Event::JoystickButtonReleased)
    {
        switch (evt.joystickButton.button)
        {
        default: break;
        case 0:
            m_inputMask |= B;
            break;
        case 1:
            m_inputMask |= A;
            break;
        case 6:
            m_inputMask |= Select;
            break;
        case 7:
            m_inputMask |= Start;
            break;
        }
    }
    else if (evt.type == sf::Event::JoystickButtonPressed)
    {
        switch (evt.joystickButton.button)
        {
        default: break;
        case 0:
            m_inputMask &= ~B;
            break;
        case 1:
            m_inputMask &= ~A;
            break;
        case 6:
            m_inputMask &= ~Select;
            break;
        case 7:
            m_inputMask &= ~Start;
            break;
        }
    }
}

void Controller::tick()
{
    //handle this mask separately so we don't overwrite
    //keyboard presses with controller non-presses
    std::uint8_t controllerMask = 0xff;
    if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) < -Deadzone)
    {
        controllerMask &= ~Left;
    }
    else if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) > Deadzone)
    {
        controllerMask &= ~Right;
    }

    //hum, on windows the Y axis appears to be inverted
#ifdef _WIN32
    if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) < -Deadzone)
    {
        controllerMask &= ~Down;
}
    else if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) > Deadzone)
    {
        controllerMask &= ~Up;
    }
#else
    if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) < -Deadzone)
    {
        controllerMask &= ~Up;
    }
    else if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) > Deadzone)
    {
        controllerMask &= ~Down;
    }
#endif


    if (controllerMask != m_controllerMask)
    {
        m_inputMask = (m_inputMask & 0xf0) | (controllerMask & 0x0f);
    }
    m_controllerMask = controllerMask;
    


    auto val = m_mmu.getByte(InputRegister);
    if ((val & (1 << 4)) == 0)
    {
		//reading direction
        val |= (m_inputMask & 0x0f);
    }
    else if ((val & (1 << 5)) == 0)
    {
		//reading buttons
        val |= ((m_inputMask & 0xf0) >> 4);
    }
    m_mmu.setByte(InputRegister, val);

    if (m_inputMask != m_lastMask)
    {
        m_interruptManager.requestInterrupt(Interrupt::Type::P10_13);
    }
    m_lastMask = m_inputMask;
}

void Controller::updateTexture()
{
    m_vertices.clear();
    for (auto& v : imageQuad.vertices)
    {
        m_vertices.push_back(v);
    }

    //add any active button highlights
    for (auto i = 0; i < 8; ++i)
    {
        if ((m_inputMask & (1 << i)) == 0)
        {
            const auto& quad = buttonQuads[i];
            for (auto& v : quad.vertices)
            {
                m_vertices.push_back(v);
            }
        }
    }

    m_texture.clear(sf::Color::Red);
    m_texture.draw(m_vertices.data(), m_vertices.size(), sf::Quads, &m_buttonTexture);
    m_texture.display();
}
