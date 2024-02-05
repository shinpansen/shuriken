#ifndef menuList_HPP
#define menuList_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../tools/xmlReader.hpp"
#include "../tools/input.hpp"
#include "../tools/files.hpp"
#include "../tools/pixelFont.hpp"
#include "../effects/sound.hpp"

using namespace std;
using namespace sf;

class menuList
{
public:
    menuList();
    menuList(vector<string> items, Vector2f pos, input* input, sound* soundBox,
             ALIGNEMENT align = ALIGNEMENT_CENTER, bool clickMode = false);
    void ResetAnimation(bool resetItemPos = true);
    void PerformInputs(RenderWindow* window);
    int GetSelectedItem();
    string GetSelectedText();
    void MoveSelection(int direction);
    void FocusText(string text);
    void FocusElement(int i);
    bool mouseOnSelectedText(RenderWindow* window);
    bool IsASeparator(int i);
    void PerformSelectedItem(bool forceFocus = false);
    void Display(RenderWindow* window, float deltaTime, bool step = true);
    bool _keyPressed;
protected:
    //Variables
    Vector2f _pos;
    vector<string> _items;
    float _deltaTime;
    int _selectedItem;
    int _focusedItem;
    int _maxItemIndex;
    int _maxTextWidth;
    bool _clickMode;

    //Objects
    input* _input;
    files _files;
    sound* _soundBox;

    //Graphics
    vector<pixelFont*> _texts;
};
#endif
