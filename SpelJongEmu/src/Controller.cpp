#include "Controller.hpp"
#include "InterruptManager.hpp"
#include "Mmu.hpp"

#include <SFML/Window/Event.hpp>

namespace
{
    const std::uint16_t InputRegister = 0xff00;
    const float Deadzone = 20.f;

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
}

Controller::Controller(InterruptManager& im, Mmu& mmu)
    : m_inputMask		(0xff),
    m_controllerMask    (0xff),
	m_lastMask			(0xff),
	m_interruptManager	(im),
    m_mmu				(mmu)
{

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
            case sf::Keyboard::Enter:
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
        case sf::Keyboard::Enter:
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
    auto oldVal = val;
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

    if (/*m_inputMask != m_lastMask*/val != oldVal)
    {
        m_interruptManager.requestInterrupt(Interrupt::Type::P10_13);
    }
    m_lastMask = m_inputMask;
}