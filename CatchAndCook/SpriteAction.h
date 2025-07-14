#pragma once

#include "Sprite.h"

class ActionFunc
{
public:
    static void OnClickAction(KeyCode key, Sprite* sprite);
    static void OnDragAction(KeyCode key, Sprite* sprite);
    static void DisableMouseAction(KeyCode key, Sprite* sprite);
    static void EnableDisableKeyAction(KeyCode key, Sprite* sprite);
};



class ActionCommand
{
public:
    virtual ~ActionCommand() {}
    virtual void Execute(Sprite* sprite) = 0;
};


class ClickAction : public ActionCommand
{
public:
    ClickAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
        ActionFunc::OnClickAction(_key, sprite);
    }

private:
    KeyCode _key;
};


class DragAction : public ActionCommand
{
public:
    DragAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
        ActionFunc::OnDragAction(_key, sprite);
    }

private:
    KeyCode _key;
};

class DisableMouseAction : public ActionCommand
{
public:
    DisableMouseAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
        ActionFunc::DisableMouseAction(_key, sprite);
    }

private:
    KeyCode _key;
};

class EnableDisableKeyAction : public ActionCommand
{
public:
    EnableDisableKeyAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
        ActionFunc::EnableDisableKeyAction(_key, sprite);
    }

private:
    KeyCode _key;
};


