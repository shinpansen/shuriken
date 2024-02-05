#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <Box2D/Box2D.h>

#include "tools/Box2DTools.hpp"
#include "tools/input.hpp"
#include "tools/xmlReader.hpp"
#include "tools/settings.hpp"
#include "effects/effects.hpp"
#include "effects/projectile.hpp"
#include "effects/sound.hpp"
#include "characters/hero.hpp"
#include "environment/maps.hpp"
#include "UI/menuList.hpp"
#include "UI/mainGame.hpp"

using namespace sf;
using namespace std;

static const float SCALE = 30.f;
static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

void LoadScores(string** scoresString)
{
    try
    {
        for(int i=0 ; i<20 ; i++)
        {
            //Sending request
            sf::Http http("http://shinpansen.esy.es/");
            sf::Http::Request request("/get_scores.php", sf::Http::Request::Post);
            std::ostringstream stream;
            stream << "i=" << i;
            request.setBody(stream.str());
            sf::Http::Response response = http.sendRequest(request, Time::Zero);

            //If error connection
            if(response.getStatus() != 200)
                (*scoresString)[i] = "error";
            else
                (*scoresString)[i] = response.getBody() != "" ? response.getBody() : ";";
        }
    }
    catch(exception e)
    {
        cout << e.what() << endl;
        throw e;
        return;
    }
}

int main(int argc, char **argv)
{
    //Score with parallel thread
    string* scoresString = new string[20];
    for(int i=0 ; i<20 ; i++) { scoresString[i] = "..."; }
    Thread thread(&LoadScores, &scoresString);
    thread.launch();

    //Window
    settings settings;
    Vector2i screenSize;
    if(settings.fullscreen)
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(NATIVE_WIDTH*settings.screenRatio,NATIVE_HEIGHT*settings.screenRatio);
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    RenderWindow window(VideoMode(screenSize.x, screenSize.y, 32), "SHURIKEN!",
    settings.fullscreen ? Style::Fullscreen : Style::Close, ctxSettings);

    //View
    FloatRect rect;
    if(NATIVE_HEIGHT*screenSize.x/screenSize.y >= NATIVE_WIDTH)
        rect = FloatRect(0,0,NATIVE_HEIGHT*screenSize.x/screenSize.y,NATIVE_HEIGHT);
    else
        rect = FloatRect(0,0,NATIVE_WIDTH,NATIVE_WIDTH*screenSize.y/screenSize.x);
    View view(FloatRect(0,0,NATIVE_WIDTH,NATIVE_HEIGHT)); //280x192 native Apple II resolution
    view.setCenter(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    RectangleShape shapeLeft(Vector2f(500,NATIVE_HEIGHT));
    RectangleShape shapeRight(Vector2f(500,NATIVE_HEIGHT));
    RectangleShape shapeTop(Vector2f(NATIVE_WIDTH,200));
    RectangleShape shapeBottom(Vector2f(NATIVE_WIDTH,200));
    shapeLeft.setFillColor(Color(0,0,0));
    shapeRight.setFillColor(Color(0,0,0));
    shapeTop.setFillColor(Color(0,0,0));
    shapeBottom.setFillColor(Color(0,0,0));
    shapeLeft.setPosition(-500,0);
    shapeRight.setPosition(NATIVE_WIDTH,0);
    shapeTop.setPosition(0,-200);
    shapeBottom.setPosition(0,NATIVE_HEIGHT);
    window.setView(view);

    //THE GAME
    mainGame theGame(&window, &view, &settings, &thread, &scoresString);
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
            window.setView(view);

            //FPS - Adapting game speed to fps
            Time frameTime = clock.restart();
            deltaTime = 60*frameTime.asSeconds(); //60fps native speed - Not used

            //THE GAME CAN GO ON !
            theGame.PauseMusic(false);
            theGame.GoOn(key, keyPressed, deltaTime);

            //Window properties
            /*window.draw(shapeLeft);
            window.draw(shapeRight);
            window.draw(shapeTop);
            window.draw(shapeBottom);*/
            window.setFramerateLimit(60);
            window.setVerticalSyncEnabled(true);
            window.display();

            //{Changing screen ratio
            if(Keyboard::isKeyPressed(Keyboard::F1))
                window.create(VideoMode(NATIVE_WIDTH, NATIVE_HEIGHT, 32), "SHURIKEN!", Style::Close, ctxSettings);
            else if(Keyboard::isKeyPressed(Keyboard::F2))
                window.create(VideoMode(NATIVE_WIDTH*2, NATIVE_HEIGHT*2, 32), "SHURIKEN!", Style::Close, ctxSettings);
            else if(Keyboard::isKeyPressed(Keyboard::F3))
                window.create(VideoMode(NATIVE_WIDTH*3, NATIVE_HEIGHT*3, 32), "SHURIKEN!", Style::Close, ctxSettings);
            else if(Keyboard::isKeyPressed(Keyboard::F4))
                window.create(VideoMode(NATIVE_WIDTH*4, NATIVE_HEIGHT*4, 32), "SHURIKEN!", Style::Close, ctxSettings);
            else if(Keyboard::isKeyPressed(Keyboard::F12))
                window.create(VideoMode(NATIVE_WIDTH, NATIVE_HEIGHT, 32), "SHURIKEN!", Style::Fullscreen, ctxSettings);

            //Saving new screen ratio
            int screenRatioCurrent = settings.screenRatio;
            settings.screenRatio = (int)(window.getSize().x/NATIVE_WIDTH);
            if(screenRatioCurrent != settings.screenRatio)
                settings.Save();
            //}
        }
        else
            theGame.PauseMusic(true);
    }

    return EXIT_SUCCESS;
}
