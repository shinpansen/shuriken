#include "tools.hpp"

using namespace std;
using namespace sf;

static const int MIDDLE_WIDTH = 140;
static const int MIDDLE_HEIGHT = 96;
static const int TILE = 10;

tools::tools()
{
}

void tools::drawSprite(RenderWindow* window, Sprite* sprite, View* view, Vector2i pos)
{
    int x = ((view->getCenter().x  - ((int)view->getCenter().x % TILE)) - MIDDLE_WIDTH) + (pos.x - (pos.x % TILE));
    int y = ((view->getCenter().y  - ((int)view->getCenter().y % TILE)) - MIDDLE_HEIGHT) + (pos.y - (pos.y % TILE));
    sprite->setPosition(x, y-4); // TODO : AJUSTER !!!
    window->draw(*sprite);
}

void tools::moveView(RenderWindow* window, View* view, Vector2i pos)
{
    //return;
    if(pos.x < 5)
        view->setCenter(view->getCenter().x - 1, view->getCenter().y);
    else if(pos.x > view->getSize().x - 5)
        view->setCenter(view->getCenter().x + 1, view->getCenter().y);

    if(pos.y < 5)
        view->setCenter(view->getCenter().x, view->getCenter().y - 1);
    else if(pos.y > view->getSize().y - 5)
        view->setCenter(view->getCenter().x, view->getCenter().y + 1);

    window->setView(*view);
}

void tools::eraseSprite(vector<Sprite*>* sprites, View* view, Vector2i mouse)
{
    Vector2i pos = getRect(mouse);
    pos.x += view->getCenter().x - MIDDLE_WIDTH;
    pos.y += view->getCenter().y - MIDDLE_HEIGHT;
    for(int i=0 ; i<sprites->size() ; i++)
    {
        if(pos.x >= (*sprites)[i]->getPosition().x &&
           pos.x <= (*sprites)[i]->getPosition().x + (*sprites)[i]->getTextureRect().width &&
           pos.y >= (*sprites)[i]->getPosition().y &&
           pos.y <= (*sprites)[i]->getPosition().y + (*sprites)[i]->getTextureRect().height)
           {
               delete (*sprites)[i];
               (*sprites).erase((*sprites).begin()+i);
           }
    }
}

void tools::eraseCollision(vector<RectangleShape*>* shapes, View* view, Vector2i mouse, vector<int>* collisionType)
{
    Vector2i pos = getRect(mouse);
    pos.x += view->getCenter().x - MIDDLE_WIDTH;
    pos.y += view->getCenter().y - MIDDLE_HEIGHT;
    for(int i=0 ; i<shapes->size() ; i++)
    {
        if(pos.x >= (*shapes)[i]->getPosition().x &&
           pos.x <= (*shapes)[i]->getPosition().x + (*shapes)[i]->getSize().x &&
           pos.y >= (*shapes)[i]->getPosition().y &&
           pos.y <= (*shapes)[i]->getPosition().y + (*shapes)[i]->getSize().y)
           {
               delete (*shapes)[i];
               (*shapes).erase((*shapes).begin()+i);
               if(collisionType != NULL)
                    (*collisionType).erase((*collisionType).begin()+i);
           }
    }
}

void tools::eraseTree(vector<tree*>* treeList, View* view, Vector2i mouse)
{
    Vector2i pos = mouse;
    pos.x += view->getCenter().x - MIDDLE_WIDTH;
    pos.y += view->getCenter().y - MIDDLE_HEIGHT;
    for(int i=0 ; i<treeList->size() ; i++)
    {
        if(pos.x >= (*treeList)[i]->GetPosition().x - 58 &&
           pos.x <= (*treeList)[i]->GetPosition().x + 58 &&
           pos.y >= (*treeList)[i]->GetPosition().y - 138 &&
           pos.y <= (*treeList)[i]->GetPosition().y + 12)
           {
                delete (*treeList)[i];
                (*treeList).erase((*treeList).begin()+i);
           }
    }
}

void tools::selectLayer(Sprite* sprite, Sprite* tile, RenderWindow* window, View* view,
                      bool* mousePressed, int* x1, int* x2, int* y1, int* y2)
{
    tile->setPosition(view->getCenter());
    window->draw(*tile);
    if(Mouse::isButtonPressed(Mouse::Left) && !*mousePressed)
    {
        *x1 = getRect(Mouse::getPosition(*window)).x;
        *y1 = getRect(Mouse::getPosition(*window)).y;
        *mousePressed = true;
    }
    else if(Mouse::isButtonPressed(Mouse::Left))
    {
        *x2 = getRect(Mouse::getPosition(*window)).x;
        *y2 = getRect(Mouse::getPosition(*window)).y;
        if(*x1 == *x2) { *x2 += TILE; }
        if(*y1 == *y2) { *y2 += TILE; }
    }
    else
        *mousePressed = false;

    IntRect rect = calcRect(*x1, *x2, *y1, *y2);
    RectangleShape shape = getShape(&rect, view, *x1, *x2, *y1, *y2);

    window->draw(shape);
    sprite->setTextureRect(rect);
}

void tools::selectCollision(vector<RectangleShape*>* shapes, RenderWindow* window,  View* view,
                            bool* mousePressed, int* x1, int* x2, int* y1, int* y2, int type, vector<int>* collisionType)
{
    if(Mouse::isButtonPressed(Mouse::Left) && !*mousePressed)
    {
        *x1 = getRect(Mouse::getPosition(*window)).x;
        *y1 = getRect(Mouse::getPosition(*window)).y;
        *mousePressed = true;
    }
    else if(Mouse::isButtonPressed(Mouse::Left))
    {
        *x2 = getRect(Mouse::getPosition(*window)).x;
        *y2 = getRect(Mouse::getPosition(*window)).y;
        if(*x1 == *x2) { *x2 += TILE; }
        if(*y1 == *y2) { *y2 += TILE; }

        //Draw current shape
        IntRect rect = calcRect(*x1+view->getCenter().x-MIDDLE_WIDTH, *x2+view->getCenter().x-MIDDLE_WIDTH,
                                *y1+view->getCenter().y-MIDDLE_HEIGHT, *y2+view->getCenter().y-MIDDLE_HEIGHT);
        RectangleShape shape;
        Vector2i pos = getRect(Vector2i(*x1+view->getCenter().x-MIDDLE_WIDTH, *y1+view->getCenter().y-MIDDLE_HEIGHT));
        shape.setSize(Vector2f(rect.width, rect.height));
        shape.setPosition(pos.x, pos.y);
        shape.setFillColor(Color(255,0,0,150));
        window->draw(shape);
    }
    else
    {
        //Add shape
        if(*mousePressed)
        {
            IntRect rect = calcRect(*x1+view->getCenter().x-MIDDLE_WIDTH, *x2+view->getCenter().x-MIDDLE_WIDTH,
                            *y1+view->getCenter().y-MIDDLE_HEIGHT, *y2+view->getCenter().y-MIDDLE_HEIGHT);
            Vector2i pos = getRect(Vector2i(*x1+view->getCenter().x-MIDDLE_WIDTH, *y1+view->getCenter().y-MIDDLE_HEIGHT));
            shapes->push_back(new RectangleShape());
            (*shapes)[shapes->size()-1]->setSize(Vector2f(rect.width, rect.height));
            (*shapes)[shapes->size()-1]->setPosition(pos.x, pos.y);

            (*shapes)[shapes->size()-1]->setOutlineColor(Color(255,0,0));
            (*shapes)[shapes->size()-1]->setOutlineThickness(1);
            if(collisionType != NULL)
            {
                collisionType->push_back(type);
                (*shapes)[shapes->size()-1]->setFillColor(Color(255,255,0,100));
            }
            else
                (*shapes)[shapes->size()-1]->setFillColor(Color(255,0,0,100));

        }
        *mousePressed = false;
    }
}

void tools::addSprite(vector<Sprite*>* sprites, Sprite* sprite, Texture* texture, bool* mousePressed)
{
    if(Mouse::isButtonPressed(Mouse::Left))
    {
        if(!*mousePressed)
        {
            *mousePressed = true;
            sprites->push_back(new Sprite());
            (*sprites)[sprites->size()-1]->setTexture(*texture);
            (*sprites)[sprites->size()-1]->setTextureRect(sprite->getTextureRect());
            (*sprites)[sprites->size()-1]->setOrigin(0,0);
            (*sprites)[sprites->size()-1]->setPosition(sprite->getPosition());
        }
    }
    else
        *mousePressed = false;
}

void tools::addTree(vector<tree*>* treeList, Vector2f pos, Texture* texture, int colorID,
                        int kindOfTree, b2World* world, effects* effectBox, bool* mousePressed)
{
    if(Mouse::isButtonPressed(Mouse::Left))
    {
        if(!*mousePressed)
        {
            *mousePressed = true;
            treeList->push_back(new tree(pos, texture, colorID, kindOfTree, world, effectBox));
        }
    }
    else
        *mousePressed = false;
}

void tools::changeKindOfTree(tree* treeBase, bool* directionPressed)
{
    //Kind of tree
    if(Keyboard::isKeyPressed(Keyboard::Numpad1))
        treeBase->SetKind(treeBase->GetColorID(), 0);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad2))
        treeBase->SetKind(treeBase->GetColorID(), 1);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad3))
        treeBase->SetKind(treeBase->GetColorID(), 2);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad4))
        treeBase->SetKind(treeBase->GetColorID(), 3);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad5))
        treeBase->SetKind(treeBase->GetColorID(), 4);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad6))
        treeBase->SetKind(treeBase->GetColorID(), 5);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad7))
        treeBase->SetKind(treeBase->GetColorID(), 6);
    else if(Keyboard::isKeyPressed(Keyboard::Numpad8))
        treeBase->SetKind(treeBase->GetColorID(), 7);

    //Tree color
    if(!*directionPressed && (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Down)))
    {
        int colorID = treeBase->GetColorID();
        *directionPressed = true;
        colorID += Keyboard::isKeyPressed(Keyboard::Up) ? -1 : 1;
        if(colorID < 0)
            colorID = 12;
        else if(colorID > 12)
            colorID = 0;
        treeBase->SetKind(colorID, treeBase->GetKindOfTree());
    }
    else if(!Keyboard::isKeyPressed(Keyboard::Up) && !Keyboard::isKeyPressed(Keyboard::Down) &&
            !Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
        *directionPressed = false;
}

Vector2i tools::getRect(Vector2i pos)
{
    int x = (pos.x - (pos.x % TILE));
    int y = (pos.y - (pos.y % TILE));
    return Vector2i(x,y);
}

IntRect tools::calcRect(int x1, int x2, int y1, int y2)
{
    int width = max(x1, x2) - min(x1, x2);
    int height = max(y1, y2) - min(y1, y2);
    return IntRect(min(x1, x2), min(y1, y2), width, height);
}

RectangleShape tools::getShape(IntRect* rect, View* view, int x1, int x2, int y1, int y2)
{
    RectangleShape shape;
    shape.setSize(Vector2f(rect->width, rect->height));
    shape.setFillColor(Color(255,255,255,100));
    shape.setOrigin(0,0);
    int topX = view->getCenter().x-MIDDLE_WIDTH;
    int topY = view->getCenter().y-MIDDLE_HEIGHT;
    shape.setPosition(topX + min(x1,x2), topY + min(y1,y2));
    return shape;
}

void tools::manageMode(bool* directionPressed, int* mode, Text* text, RenderWindow* window)
{
    if(!*directionPressed && (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right)))
    {
        *directionPressed = true;
        *mode += Keyboard::isKeyPressed(Keyboard::Left) ? -1 : 1;
        if(*mode < 0)
            *mode = 5;
        else if(*mode > 5)
            *mode = 0;
    }
    else if(!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right) &&
            !Keyboard::isKeyPressed(Keyboard::Up) && !Keyboard::isKeyPressed(Keyboard::Down))
        *directionPressed = false;

    if(*mode == 0)
        text->setString("LAYER0");
    else if(*mode == 1)
        text->setString("LAYER1");
    else if(*mode == 2)
        text->setString("COLLISION");
    else if(*mode == 3)
        text->setString("PLATFORM");
    else if(*mode == 4)
        text->setString("LADDER");
    else
        text->setString("TREE");
    text->setPosition(window->getView().getCenter().x-MIDDLE_WIDTH+10, window->getView().getCenter().y+MIDDLE_HEIGHT-10);
    window->draw(*text);
}

void tools::drawCursor(RenderWindow* window, View* view, Color color)
{
    //Cursor
    window->setMouseCursorVisible(true);// (false);
    RectangleShape shapeCursor;
    Vector2i pos = getRect(Mouse::getPosition(*window));
    Vector2i pos2 = getRect(Vector2i(pos.x+view->getCenter().x-MIDDLE_WIDTH, pos.y+view->getCenter().y-MIDDLE_HEIGHT));
    shapeCursor.setPosition(pos2.x, pos2.y);
    shapeCursor.setSize(Vector2f(TILE,TILE));
    shapeCursor.setFillColor(color);
    window->draw(shapeCursor);
}

void tools::loadShapes(string filePath, vector<RectangleShape*>* shapes, vector<int>* collisionType)
{
    try
    {
        string line;
        ifstream file(filePath.c_str(), ios::in);
        while(getline(file, line))
        {
            vector<float> param = files::readLine(line);
            shapes->push_back(new RectangleShape());
            (*shapes)[shapes->size()-1]->setSize(Vector2f(param[2], param[3]));
            (*shapes)[shapes->size()-1]->setPosition(Vector2f(param[0]-(param[2]/2), param[1]-(param[3]/2)));
            (*shapes)[shapes->size()-1]->setOutlineColor(Color(255,0,0));
            (*shapes)[shapes->size()-1]->setOutlineThickness(1);
            if(param[4] == 1)
                (*shapes)[shapes->size()-1]->setFillColor(Color(255,0,0,100));
            else
                (*shapes)[shapes->size()-1]->setFillColor(Color(255,255,0,100));
            if(collisionType != NULL)
                collisionType->push_back(param[4]);
        }
        file.close();
    }
    catch(exception ex) { cout << ex.what() << endl; }
}

void tools::saveMap(vector<Sprite*> layer0, vector<Sprite*> sprites, vector<RectangleShape*> shapes,
                    vector<RectangleShape*> ladders, vector<tree*> treeList, vector<int> collisionType)
{
    //Layer 0
    files files;
    cout << "Saving layer0...\n";
    ofstream fileLayer0(files.layer0File.c_str(), ios::out | ios::trunc);
    if(fileLayer0)
    {
        for(int i=0 ; i<layer0.size() ; i++)
        {
            Vector2f pos = layer0[i]->getPosition();
            IntRect rect = layer0[i]->getTextureRect();
            fileLayer0 << pos.x << "," << pos.y << ",";
            fileLayer0 << rect.left << "," << rect.top << "," << rect.width << "," << rect.height << "," << endl;
        }
        fileLayer0.close();
        cout << "done !\n";
    }
    else
        cerr << "Unable to open file '" << files.layer0File << "' !" << endl;

    //Layer 1
    cout << "Saving layer1...\n";
    ofstream fileLayer1(files.layer1File.c_str(), ios::out | ios::trunc);
    if(fileLayer1)
    {
        for(int i=0 ; i<sprites.size() ; i++)
        {
            Vector2f pos = sprites[i]->getPosition();
            IntRect rect = sprites[i]->getTextureRect();
            fileLayer1 << pos.x << "," << pos.y << ",";
            fileLayer1 << rect.left << "," << rect.top << "," << rect.width << "," << rect.height << "," << endl;
        }
        fileLayer1.close();
        cout << "done !\n";
    }
    else
        cerr << "Unable to open file '" << files.layer1File << "' !" << endl;

    //Collisions
    cout << "Saving collisions...\n";
    ofstream fileCollision(files.collisionFile.c_str(), ios::out | ios::trunc);
    if(fileCollision)
    {
        for(int i=0 ; i<shapes.size() ; i++)
        {
            Vector2f pos = shapes[i]->getPosition();
            Vector2f shapeSize = shapes[i]->getSize();
            fileCollision << pos.x+shapeSize.x/2 << "," << pos.y+shapeSize.y/2 << ",";
            fileCollision << shapeSize.x << "," << shapeSize.y << "," << collisionType[i] << "," << endl;
        }
        fileCollision.close();
        cout << "done !\n";
    }
    else
        cerr << "Unable to open file '" << files.collisionFile << "' !" << endl;

    //Ladders
    cout << "Saving ladders...\n";
    ofstream fileLadder(files.ladderFile.c_str(), ios::out | ios::trunc);
    if(fileLadder)
    {
        for(int i=0 ; i<ladders.size() ; i++)
        {
            Vector2f pos = ladders[i]->getPosition();
            Vector2f shapeSize = ladders[i]->getSize();
            fileLadder << pos.x+shapeSize.x/2 << "," << pos.y+shapeSize.y/2 << ",";
            fileLadder << shapeSize.x << "," << shapeSize.y << "," << endl;
        }
        fileLadder.close();
        cout << "done !\n";
    }
    else
        cerr << "Unable to open file '" << files.ladderFile << "' !" << endl;

    //Trees
    cout << "Saving trees...\n";
    ofstream fileTree(files.treeFile.c_str(), ios::out | ios::trunc);
    if(fileTree)
    {
        for(int i=0 ; i<treeList.size() ; i++)
        {
            Vector2f pos = treeList[i]->GetPosition();
            int colorID = treeList[i]->GetColorID();
            int kind = treeList[i]->GetKindOfTree();
            fileTree << pos.x << "," << pos.y << "," << colorID << "," << kind << "," << endl;
        }
        fileTree.close();
        cout << "done !\n";
    }
    else
        cerr << "Unable to open file '" << files.treeFile << "' !" << endl;

    //Copy to game project directory
    /*cout << "Copying files to game project directory...\n";
    copyFile(files.layer1File, "..//shuriken_crossjoin//" + files.layer1File);
    copyFile(files.collisionFile, "..//shuriken_crossjoin//" + files.collisionFile);
    copyFile(files.ladderFile, "..//shuriken_crossjoin//" + files.ladderFile);
    copyFile(files.layer0File, "..//shuriken//" + files.layer0File);
    copyFile(files.layer1File, "..//shuriken//" + files.layer1File);
    copyFile(files.collisionFile, "..//shuriken//" + files.collisionFile);
    copyFile(files.ladderFile, "..//shuriken//" + files.ladderFile);
    copyFile(files.treeFile, "..//shuriken//" + files.treeFile);
    cout << "done !\n";*/
}

void tools::copyFile(string sourcePath, string destinationPath)
{
    try
    {
        ifstream source(sourcePath.c_str(), ios::binary);
        ofstream destination(destinationPath.c_str(), ios::binary);
        destination << source.rdbuf();
        source.close();
        destination.close();
    }
    catch(exception ex) { cout << ex.what() << endl; }
}
