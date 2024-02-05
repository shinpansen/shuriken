#ifndef menuList_HPP
#define menuList_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../tools/xmlReader.hpp"
#include "../tools/input.hpp"
#include "../tools/files.hpp"
#include "../effects/sound.hpp"

using namespace std;
using namespace sf;

class menuList
{
public:
    menuList();
    menuList(vector<string> items, Vector2f pos, input* input, sound* soundBox, string alignment = "CENTER");
    void ResetAnimation(bool resetItemPos = true);
    void PerformInputs(RenderWindow* window);
    int GetSelectedItem();
    string GetSelectedText();
    void FocusElement(int i);
    void FocusText(string text);
    bool mouseOnSelectedText(RenderWindow* window);
    void PerformSelectedItem(bool forceFocus = false);
    void Display(RenderWindow* window, float deltaTime, bool step = true);
protected:
    //Variables
    Vector2f _pos;
    vector<string> _items;
    float _deltaTime;
    int _selectedItem;
    int _maxItemIndex;
    int _maxTextWidth;
    bool _keyPressed;

    //Objects
    input* _input;
    files _files;
    sound* _soundBox;

    //Graphics
    Font _font;
    vector<Text*> _texts;
};
#endif
