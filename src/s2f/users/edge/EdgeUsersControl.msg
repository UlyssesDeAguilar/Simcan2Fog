namespace s2f::users;

//
// Generic state for both apps and vms
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-05-03
// @version 1.0
enum State
{
    PENDING = 0;
    UP = 1;
    DOWN = 2;
};


//
// Small control class for managing an application state
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-05-03
// @version 1.0
class App
{
    string name; // Name of the instance of the application
    string type; // Parameterized instance name of the model of the application
    string dependant; // Reference for other applications that depend on this application
    State state = PENDING; // The current application state
};

//
// Small control class for managing a VM state
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-05-03
// @version 1.0
class Vm
{
    string name; // Name of the instance of the VM
    string type; // Parameterized instance name of the model of the VM
    State state = PENDING; // The current VM state
    App app[]; // Array of app definitions that will be deployed onto this VM
};