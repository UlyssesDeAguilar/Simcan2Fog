package s2f.UserModel.Edge;

import inet.node.inet.StandardHost;

module EdgeUserModel extends StandardHost
{
    parameters:
        @display("i=user");
        
        // Set the driver as "commander" of the module
        numApps = 1;
        app[0].typename = "EdgeUserModelDriver";
}