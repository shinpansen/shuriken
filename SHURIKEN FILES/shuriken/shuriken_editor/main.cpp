#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "effects/effects.hpp"
#include "tools/tools.hpp"
#include "tools/files.hpp"
#include "environment/maps.hpp"

using namespace sf;
using namespace std;

int main()
{
    //Window
    RenderWindow window(VideoMode(840, 576, 32), "shuriken editor");//, Style::Fullscreen);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    //View
    View view;
    view.setSize(840,576);
    view.setCenter(420,288);

    //Sprite
    Texture texture, treeTexture;
    texture.loadFromFile("resources//tiles/layer1.png");
    treeTexture.loadFromFile("resources//environment/trees.png");

    //Tile
    Sprite tile;
    tile.setTexture(texture);
    tile.setOrigin(420,288);

    //Sprite + Sprite list
    Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect(IntRect(0,0,32,32));
    sprite.setOrigin(0,0);

    //Map
    effects effectBox;
    b2World* world = new b2World(b2Vec2(0.f, 50.f), false);
    maps maps(world, &effectBox);

    //Trees
    tree treeBase(Vector2f(0,0), &treeTexture, 0, 0, world, &effectBox);

    //Collisions - ladders
    files files;
    vector<RectangleShape*> shapes;
    vector<int> collisionType;
    vector<RectangleShape*> ladderShapes;
    tools::loadShapes(files.collisionFile, &shapes, &collisionType);
    tools::loadShapes(files.ladderFile, &ladderShapes);

    //Mode - Texts
    int mode = 0;
    bool directionPressed = false;
    Font font;
    font.loadFromFile("resources//fonts//rainyhearts.ttf");
    Text text;
    text.setFont(font);
    text.setCharacterSize(40);
    text.setStyle(Text::Bold);
    text.setColor(Color(255,217,0));
    text.setString("LAYER0");

    //IntRect
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    int x1_2 = 0, x2_2 = 0, y1_2 = 0, y2_2 = 0;
    bool mousePressed = false;
    bool keyPressed = false;
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();

        window.clear(Color(80,80,80));

        //Delete sprite
        if(Mouse::isButtonPressed(Mouse::Right))
        {
            if(mode == 0)
                tools::eraseSprite(&maps._layer0Tiles, &view, Mouse::getPosition(window));
            else if(mode == 1)
                tools::eraseSprite(&maps._layer1Tiles, &view, Mouse::getPosition(window));
            else if(mode == 2 || mode == 3)
                tools::eraseCollision(&shapes, &view, Mouse::getPosition(window), &collisionType);
            else if(mode == 4)
                tools::eraseCollision(&ladderShapes, &view, Mouse::getPosition(window));
            else if(mode == 5)
                tools::eraseTree(&maps._treeList, &view, Mouse::getPosition(window));
        }

        //Tile default
        if(Keyboard::isKeyPressed(Keyboard::Num1) && (mode == 0 || mode == 1))
        {
            window.setMouseCursorVisible(true);
            tools::selectLayer(&sprite, &tile, &window, &view, &mousePressed, &x1, &x2, &y1, &y2);
        }
        else //Display sprites
        {
            //Show Grid
            for(int i=0 ; i<=840; i+=16)
            {
                Vertex line1[] = { Vertex(Vector2f(i, 0)), Vertex(Vector2f(i, 10000)) };
                window.draw(line1, 2, sf::Lines);
                if(i <= 576)
                {
                    Vertex line2[] = { Vertex(Vector2f(0, i)), Vertex(Vector2f(10000, i)) };
                    window.draw(line2, 2, sf::Lines);
                }
            }

            //Draw sprites
            maps.RenderMap(&window, mode);

            //View
            if(!Mouse::isButtonPressed(Mouse::Left))
                tools::moveView(&window, &view, Mouse::getPosition(window));

            if(mode == 0 || mode == 1)
            {
                //Add Sprite
                window.setMouseCursorVisible(true);
                tools::drawSprite(&window, &sprite, &view, Mouse::getPosition(window));
                tools::addSprite(mode == 0 ? &maps._layer0Tiles : &maps._layer1Tiles, &sprite, &texture, &mousePressed);
            }
            else if(mode == 2)
            {
                //Draw collisions masks
                for(int i=0 ; i<shapes.size() ; i++)
                    window.draw(*shapes[i]);
                tools::selectCollision(&shapes, &window, &view, &mousePressed, &x1_2, &x2_2, &y1_2, &y2_2, 1, &collisionType);
                tools::drawCursor(&window, &view, Color(255,0,0,150));
            }
            else if(mode == 3)
            {
                //Draw collisions masks
                for(int i=0 ; i<shapes.size() ; i++)
                    window.draw(*shapes[i]);
                tools::selectCollision(&shapes, &window, &view, &mousePressed, &x1_2, &x2_2, &y1_2, &y2_2, 2, &collisionType);
                tools::drawCursor(&window, &view, Color(255,0,0,150));
            }
            else if(mode == 4)
            {
                //Draw ladders masks
                for(int i=0 ; i<ladderShapes.size() ; i++)
                    window.draw(*ladderShapes[i]);
                tools::selectCollision(&ladderShapes, &window, &view, &mousePressed, &x1_2, &x2_2, &y1_2, &y2_2, 42);
                tools::drawCursor(&window, &view, Color(255,0,0,150));
            }
            else if (mode == 5)
            {
                tools::changeKindOfTree(&treeBase, &directionPressed);
                Vector2f pos = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
                pos.x += view.getCenter().x - 400;
                pos.y += view.getCenter().y - 300;
                window.setMouseCursorVisible(true);
                treeBase.SetPosition(pos);
                treeBase.DrawTree(&window);
                tools::addTree(&maps._treeList, pos, &treeTexture, treeBase.GetColorID(), treeBase.GetKindOfTree(), world, &effectBox, &mousePressed);
            }
        }

        //Mode
        tools::manageMode(&directionPressed, &mode, &text, &window);

        //Save map
        if(Keyboard::isKeyPressed(Keyboard::S) && !keyPressed)
        {
            keyPressed = true;
            tools::saveMap(maps._layer0Tiles, maps._layer1Tiles, shapes, ladderShapes, maps._treeList, collisionType);
        }
        else if(!Keyboard::isKeyPressed(Keyboard::S))
            keyPressed = false;

        window.display();
    }

    return EXIT_SUCCESS;
}
