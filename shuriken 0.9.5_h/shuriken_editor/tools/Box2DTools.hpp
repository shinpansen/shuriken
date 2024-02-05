#ifndef Box2DTools_HPP
#define Box2DTools_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

using namespace std;
using namespace sf;

class Box2DTools
{
public:
    Box2DTools();
    static b2Body* CreateRectangleBox(b2World* world, Vector2i pos, int width, int height, float friction, float density, float angle, int category);
    static bool TestCollisionPoint(b2World* world, b2Vec2 point, bool all, short* collisionType);
    static int TestHookPoint(b2World* world, b2Vec2 point, Vector2f* collisionPos);
    static void ManageOneWayPlatforms(b2World* world);
    static void DrawCollisionMask(b2World* world, b2Body* defaultBody, RenderWindow* window);
    static uint16 Entity(int i);
    static Vector2f GetAABBSize(b2Fixture* fixture);
};
#endif
