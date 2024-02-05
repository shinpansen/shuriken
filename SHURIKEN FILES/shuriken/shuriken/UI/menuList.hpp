#ifndef menuList_HPP
#define menuList_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../tools/xmlReader.hpp"
#include "../tools/input.hpp"
#include "../tools/files.hpp"

using namespace std;
using namespace sf;

class menuList
{
public:
    menuList();
    menuList(vector<string> items, Vector2f pos, input* input, string alignment = "CENTER");
    void ResetAnimation(bool resetItemPos = true);
    void PerformInputs(RenderWindow* window);
    int GetSelectedItem();
    string GetSelectedText();
    void FocusText(string text);
    bool mouseOnSelectedText(RenderWindow* window);
    void PerformSelectedItem(bool forceFocus = false);
    void Display(RenderWindow* window, float deltaTime, bool step = true);
protected:
    //Objects - variables
    Vector2f _pos;
    vector<string> _items;
    input* _input;
    files _files;
    float _deltaTime;
    int _selectedItem;
    int _maxItemIndex;
    int _maxTextWidth;
    bool _keyPressed;

    //Graphics
    Font _font;
    vector<Text*> _texts;
};
#endif
