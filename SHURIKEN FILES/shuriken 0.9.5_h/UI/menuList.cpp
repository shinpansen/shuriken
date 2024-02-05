#include "menuList.hpp"

using namespace std;
using namespace sf;

menuList::menuList()
{
}

menuList::menuList(vector<string> items, Vector2f pos, input* input, sound* soundBox, ALIGNEMENT align, bool clickMode)
{
    //Objects - variables
    _deltaTime = 1;
    _pos = pos;
    _items = items;
    _input = input;
    _selectedItem = 0;
    _focusedItem = 0;
    _maxItemIndex = items.size()-1;
    _keyPressed = false;
    _clickMode = clickMode;
    _soundBox = soundBox;

    //Texts objects
    FloatRect textRect;
    _maxTextWidth = 0;
    for(int i=0 ; i<_items.size() ; i++)
    {
        _texts.push_back(new pixelFont());
        _texts[_texts.size()-1]->SetString(_items[i]);
        textRect = _texts[_texts.size()-1]->GetLocalBounds();
        if(textRect.width > _maxTextWidth)
            _maxTextWidth = textRect.width;
    }

    //Texts positions
    int topPos = pos.y;
    for(int i=0 ; i<_items.size() ; i++)
        _texts[i]->SetPosition(pos.x, topPos+2+(i*(textRect.height+2)), align);

    //Menu animation
    ResetAnimation();
}

void menuList::ResetAnimation(bool resetItemPos)
{
    if(resetItemPos)
        _selectedItem = 0;
    PerformSelectedItem(true);
}

void menuList::PerformInputs(RenderWindow* window)
{
    //Keyboard
    if(!_input->GetMouseController())
    {
        // Up and down
        int direction = 0;
        if((_input->isKeyPressed("UP") || _input->isKeyPressed("DOWN")) && !_keyPressed)
        {
            direction = _input->isKeyPressed("DOWN") ? 1 : -1;
            MoveSelection(direction);
            _soundBox->PlaySound(SND_BLIP, false);
            _keyPressed = true;
        }
        else if(!_input->isKeyPressed("UP") && !_input->isKeyPressed("DOWN"))
            _keyPressed = false;

        //Skipping separators
        if(IsASeparator(_selectedItem))
            MoveSelection(direction);
    }
    else //Mouse
    {
        _selectedItem = _clickMode ? _selectedItem : -1;
        _focusedItem = -1;
        Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        for(int i=0 ; i<=_maxItemIndex ; i++)
        {
            FloatRect textRect = _texts[i]->GetGlobalBounds();
            _texts[i]->SetColor(Color(255,255,255));
            if(textRect.contains(mousePos) && !IsASeparator(i))
            {
                _focusedItem = i;
                if((_clickMode && Mouse::isButtonPressed(Mouse::Left)) || !_clickMode)
                {
                    if(_selectedItem != i && _clickMode)
                        _soundBox->PlaySound(SND_BLIP, false);
                    _selectedItem = i;
                }
                break;
            }
        }

        if(IsASeparator(_selectedItem) && !_clickMode)
            _selectedItem = -1;
    }
}

int menuList::GetSelectedItem()
{
    return _selectedItem;
}

string menuList::GetSelectedText()
{
    if(_selectedItem >= 0 && _selectedItem < _texts.size())
        return _texts[_selectedItem]->GetString();
    else
        return "";
}

void menuList::MoveSelection(int direction)
{
    _selectedItem += direction;
    if(_selectedItem < 0)
        _selectedItem = _maxItemIndex;
    else if(_selectedItem > _maxItemIndex)
        _selectedItem = 0;
}

void menuList::FocusText(string text)
{
    for(int i=0 ; i<_texts.size() ; i++)
    {
        if(_texts[i]->GetString() == text)
        {
            _selectedItem = i;
            PerformSelectedItem();
            break;
        }
    }
}

void menuList::FocusElement(int i)
{
    if(i >= 0 && i < _texts.size())
    {
        _selectedItem = i;
        PerformSelectedItem();
    }
}

bool menuList::mouseOnSelectedText(RenderWindow* window)
{
    if(_selectedItem < 0)
        return false;

    Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    FloatRect textRect = _texts[_selectedItem]->GetGlobalBounds();
    if(textRect.contains(mousePos))
        return true;
    else
        return false;
}

bool menuList::IsASeparator(int i)
{
    if(i < 0 || i >= _texts.size())
        return false;

    string text = _texts[i]->GetString();
    for(int i=0 ; i<text.length() ; i++)
        if(text[i] != '-')
            return false;
    return true;
}

void menuList::PerformSelectedItem(bool forceFocus)
{
    for(int i=0 ; i<_texts.size() ; i++)
    {
        if(i > _maxItemIndex)
            return;

        if(i == _selectedItem)
            _texts[i]->Highlight(Color(234,93,240), Color(133,59,81));
        else if(i == _focusedItem && _input->GetMouseController())
             _texts[i]->SetColor(Color(246,185,202));
        else
            _texts[i]->SetColor(Color(255,255,255));
    }
}

void menuList::Display(RenderWindow* window, float deltaTime, bool step)
{
    _deltaTime = deltaTime;

    if(step)
    {
        PerformInputs(window);
        PerformSelectedItem();
    }

    for(int i=0 ; i<_texts.size() ; i++)
        _texts[i]->Display(window);
}
