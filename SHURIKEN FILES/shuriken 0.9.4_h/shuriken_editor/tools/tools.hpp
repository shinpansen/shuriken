#ifndef tools_HPP
#define tools_HPP

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include "files.hpp"
#include "../environment/tree.hpp"

using namespace std;
using namespace sf;

class tools
{
public:
    tools();
    static void drawSprite(RenderWindow* window, Sprite* sprite, View* view, Vector2i pos);
    static void moveView(RenderWindow* window, View* view, Vector2i pos);
    static void eraseSprite(vector<Sprite*>* sprites, View* view, Vector2i mouse);
    static void eraseCollision(vector<RectangleShape*>* shapes, View* view, Vector2i mouse, vector<int>* collisionType = NULL);
    static void eraseTree(vector<tree*>* treeList, View* view, Vector2i mouse);
    static void selectLayer(Sprite* sprite, Sprite* tile, RenderWindow* window,  View* view,
                          bool* mousePressed, int* x1, int* x2, int* y1, int* y2);
    static void selectCollision(vector<RectangleShape*>* shapes, RenderWindow* window,  View* view,
                                bool* mousePressed, int* x1, int* x2, int* y1, int* y2,
                                int type, vector<int>* collisionType = NULL);
    static void addSprite(vector<Sprite*>* sprites, Sprite* sprite, Texture* texture, bool* mousePressed);
    static void addTree(vector<tree*>* treeList, Vector2f pos, Texture* texture, int colorID,
                        int kindOfTree, b2World* world, effects* effectBox, bool* mousePressed);
    static void changeKindOfTree(tree* treeBase, bool* directionPressed);
    static Vector2i getRect(Vector2i pos);
    static IntRect calcRect(int x1, int x2, int y1, int y2);
    static RectangleShape getShape(IntRect* rect, View* view, int x1, int x2, int y1, int y2);
    static void manageMode(bool* directionPressed, int* mode, Text* text, RenderWindow* window);
    static void drawCursor(RenderWindow* window, View* view, Color color);
    static void loadShapes(string filePath, vector<RectangleShape*>* shapes, vector<int>* collisionType = NULL);
    static void saveMap(vector<Sprite*> layer0, vector<Sprite*> sprites, vector<RectangleShape*> shapes,
                        vector<RectangleShape*> ladders, vector<tree*> treeList, vector<int> collisionType);
    static void copyFile(string sourcePath, string destinationPath);
};
#endif
