#include "menuList.hpp"

using namespace std;
using namespace sf;

menuList::menuList()
{
}

menuList::menuList(vector<string> items, Vector2f pos, input* input, sound* soundBox, string alignment)
{
    //Objects - variables
    _deltaTime = 1;
    _pos = pos;
    _items = items;
    _input = input;
    _selectedItem = 0;
    _maxItemIndex = items.size()-1;
    _keyPressed = false;
    _soundBox = soundBox;

    //Font
    _font.loadFromFile(_files.font2);

    //Texts objects
    FloatRect textRect;
    _maxTextWidth = 0;
    for(int i=0 ; i<_items.size() ; i++)
    {
        _texts.push_back(new Text());
        _texts[_texts.size()-1]->setFont(_font);
        _texts[_texts.size()-1]->setString(_items[i]);
        _texts[_texts.size()-1]->setCharacterSize(16);
        textRect = _texts[_texts.size()-1]->getLocalBounds();
        if(textRect.width > _maxTextWidth)
            _maxTextWidth = textRect.width;

        //Alignment
        if(alignment == "LEFT")
            _texts[_texts.size()-1]->setOrigin(0, textRect.height/2+5);
        else if(alignment == "RIGHT")
            _texts[_texts.size()-1]->setOrigin(textRect.width, textRect.height/2+5);
        else
            _texts[_texts.size()-1]->setOrigin(textRect.width/2, textRect.height/2+5);
    }

    //Texts positions
    int topPos = pos.y;
    for(int i=0 ; i<_items.size() ; i++)
    {
        if(alignment == "LEFT")
            _texts[i]->setPosition(pos.x - _maxTextWidth/2, topPos+23+(i*(textRect.height+10)));
        else if(alignment == "RIGHT")
            _texts[i]->setPosition(pos.x + _maxTextWidth/2, topPos+23+(i*(textRect.height+10)));
        else
            _texts[i]->setPosition(pos.x, topPos+23+(i*(textRect.height+10)));
    }

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
        if((_input->isKeyPressed("UP") || _input->isKeyPressed("DOWN")) && !_keyPressed)
        {
            _selectedItem += _input->isKeyPressed("DOWN") ? 1 : -1;
            _soundBox->PlaySound(SND_BLIP, false);
            _keyPressed = true;
            if(_selectedItem >= 0 && _selectedItem <= _maxItemIndex)
                _texts[_selectedItem]->setScale(1.15, 1.15);
            else
                _texts[_selectedItem < 0 ? _maxItemIndex : 0]->setScale(1.15, 1.15);
        }
        else if(!_input->isKeyPressed("UP") && !_input->isKeyPressed("DOWN"))
            _keyPressed = false;

        //Loop
        if(_selectedItem < 0)
            _selectedItem = _maxItemIndex;
        else if(_selectedItem > _maxItemIndex)
            _selectedItem = 0;
    }
    else
    {
        //Mouse
        _selectedItem = -1;
        Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        for(int i=0 ; i<=_maxItemIndex ; i++)
        {
            FloatRect textRect = _texts[i]->getGlobalBounds();
            if(textRect.contains(mousePos))
            {
               _selectedItem = i;
               break;
            }
        }
    }
}

int menuList::GetSelectedItem()
{
    return _selectedItem;
}

string menuList::GetSelectedText()
{
    if(_selectedItem >= 0 && _selectedItem < _texts.size())
        return _texts[_selectedItem]->getString();
    else
        return "";
}

void menuList::FocusText(string text)
{
    for(int i=0 ; i<_texts.size() ; i++)
    {
        if(_texts[i]->getString() == text)
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
    FloatRect textRect = _texts[_selectedItem]->getGlobalBounds();
    if(textRect.contains(mousePos))
        return true;
    else
        return false;
}

void menuList::PerformSelectedItem(bool forceFocus)
{
    for(int i=0 ; i<_texts.size() ; i++)
    {
        if(i > _maxItemIndex)
            return;

        _texts[i]->setColor(i == _selectedItem ? Color(235,125,35) : Color(255,255,255));
        if (i != _selectedItem)
            _texts[i]->setScale(1,1);
    }

    if(forceFocus && _selectedItem >= 0)
        _texts[_selectedItem]->setScale(1.15,1.15);
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
        window->draw(*_texts[i]);
}
