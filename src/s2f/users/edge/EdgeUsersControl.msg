namespace users;

enum State
{
    PENDING = 0;
    UP = 1;
    DOWN = 2;
};

class App
{
    string name;
    string type;
    string dependant;
    State state = PENDING;
};

class Vm
{
    string name;
    string type;
    State state = PENDING;
    App app[];
};