#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "tools/Box2DTools.hpp"
#include "tools/input.hpp"
#include "tools/xmlReader.hpp"
#include "tools/settings.hpp"
#include "effects/effects.hpp"
#include "effects/spells.hpp"
#include "effects/sound.hpp"
#include "characters/hero.hpp"
#include "environment/maps.hpp"
#include "environment/tree.hpp"
#include "objects/ladder.hpp"
#include "UI/menuList.hpp"
#include "UI/mainGame.hpp"

using namespace sf;
using namespace std;

static const float SCALE = 30.f;

int main(int argc, char **argv)
{
    //Window
    settings settings;
    Vector2i screenSize;
    if(settings._fullscreen == "ON")
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(840,576);
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    RenderWindow window(VideoMode(screenSize.x, screenSize.y, 32), "SHURIKEN!",
    settings._fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

    //View
    FloatRect rect;
    if(576.0*screenSize.x/screenSize.y >= 840)
        rect = FloatRect(0,0,576.0*screenSize.x/screenSize.y,576);
    else
        rect = FloatRect(0,0,840,840*screenSize.y/screenSize.x);
    View view(rect); //280x192 native Apple II resolution : upscaled to 840x576
    view.setCenter(420,288);
    RectangleShape shapeLeft(Vector2f(500,576));
    RectangleShape shapeRight(Vector2f(500,576));
    RectangleShape shapeTop(Vector2f(840,200));
    RectangleShape shapeBottom(Vector2f(840,200));
    shapeLeft.setFillColor(Color(0,0,0));
    shapeRight.setFillColor(Color(0,0,0));
    shapeTop.setFillColor(Color(0,0,0));
    shapeBottom.setFillColor(Color(0,0,0));
    shapeLeft.setPosition(-500,0);
    shapeRight.setPosition(840,0);
    shapeTop.setPosition(0,-200);
    shapeBottom.setPosition(0,576);
    window.setView(view);

    //THE GAME
    mainGame theGame(&window, &settings);
    Clock clock;
    float deltaTime;
    Keyboard::Key key;
    bool keyPressed;
    bool gamePaused = false;

    //Main loop
    static Event event;
    while (window.isOpen())
    {
        keyPressed = false;
        while (window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                   window.close();
                   break;
                case sf::Event::LostFocus:
                    gamePaused = true;
                    break;
                case sf::Event::GainedFocus:
                    gamePaused = false;
                    break;
                case sf::Event::KeyPressed:
                    key = event.key.code;
                    keyPressed = true;
                    break;
            }
        }

        if(!gamePaused)
        {
            window.clear(Color(0,0,0));

            //FPS - Adapting game speed to fps
            Time frameTime = clock.restart();
            deltaTime = 60*frameTime.asSeconds(); //60fps native speed

            //THE GAME CAN GO ON !
            theGame.GoOn(key, keyPressed, deltaTime);

            //Window properties
            window.draw(shapeLeft);
            window.draw(shapeRight);
            window.draw(shapeTop);
            window.draw(shapeBottom);
            window.setFramerateLimit(60);
            window.setVerticalSyncEnabled(true);
            window.display();
        }
        else
        {
            if(!theGame.IsMusicPaused())
                theGame.PauseMusic(true);
        }
    }

    return EXIT_SUCCESS;
}
