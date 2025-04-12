namespace users;

enum State
{
    PENDING = 0;
    UP = 1;
    DOWN = 2;
};

class Vm
{
    string name;
    string type;
    State state = PENDING;
};

class App
{
    string name;
    string type;
    Vm* platform = nullptr;
    State state = PENDING;
};