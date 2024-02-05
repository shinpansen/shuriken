#include "maps.hpp"

using namespace std;
using namespace sf;

maps::maps()
{
}

maps::maps(b2World* world, effects* effectBox)
{
    _world = world;
    _effectBox = effectBox;

    //Resources
    if (!_layer1.loadFromFile(_files.layer1))
    {
        cout << "Unable to find map texture. Stopping execution." << endl;
        exit(1);
    }
    if(!_treeTexture.loadFromFile(_files.trees))
    {
        cout << "Unable to find tree texture. Stopping execution." << endl;
        exit(1);
    }

    CreateLayers();

    //Collisions
    CreateCollisions();
}

void maps::CreateLayers()
{
    try
    {
        //Layer 0
        string line;
        ifstream layerMap0(_files.layer0File.c_str(), ios::in);
        while(getline(layerMap0, line))
        {
            vector<float> param = files::readLine(line);
            _layer0Tiles.push_back(new Sprite());
            int i = _layer0Tiles.size() - 1;
            _layer0Tiles[i]->setTexture(_layer1);
            _layer0Tiles[i]->setPosition(param[0],param[1]);
            _layer0Tiles[i]->setTextureRect(IntRect(param[2],param[3],param[4],param[5]));
        }
        layerMap0.close();

        //Layer 1
        ifstream layerMap1(_files.layer1File.c_str(), ios::in);
        while(getline(layerMap1, line))
        {
            vector<float> param = files::readLine(line);
            _layer1Tiles.push_back(new Sprite());
            int i = _layer1Tiles.size() - 1;
            _layer1Tiles[i]->setTexture(_layer1);
            _layer1Tiles[i]->setPosition(param[0],param[1]);
            _layer1Tiles[i]->setTextureRect(IntRect(param[2],param[3],param[4],param[5]));
        }
        layerMap1.close();

        //Trees
        ifstream layerTrees(_files.treeFile.c_str(), ios::in);
        while(getline(layerTrees, line))
        {
            vector<float> param = files::readLine(line);
            _treeList.push_back(new tree(Vector2f(param[0],param[1]), &_treeTexture, param[2], param[3], _world, _effectBox));
        }
        layerTrees.close();
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void maps::CreateCollisions()
{
    /*Box2DTools::CreateRectangleBox(_world, Vector2i(54, 197), 100, 5, 0.1, 0, 0, 2);
    Box2DTools::CreateRectangleBox(_world, Vector2i(64, 257), 180, 5, 0.1, 0, 0, 3);
    Box2DTools::CreateRectangleBox(_world, Vector2i(414, 237), 620, 5, 0.1, 0, 0, 1);
    Box2DTools::CreateRectangleBox(_world, Vector2i(0, 350), 800, 6, 0.1, 0, 0, 1);*/
}

void maps::RenderMap(RenderWindow* window, int mode)
{
    for(int i=0 ; i<_layer0Tiles.size() ; i++)
        window->draw(*_layer0Tiles[i]);

    for(int i=0 ; i<_treeList.size() ; i++)
        _treeList[i]->DrawTree(window);

    for(int i=0 ; i<_layer1Tiles.size() ; i++)
    {
        if(mode == 0)
            _layer1Tiles[i]->setColor(Color(255,255,255,125));
        else
            _layer1Tiles[i]->setColor(Color(255,255,255,255));
        window->draw(*_layer1Tiles[i]);
    }
}
