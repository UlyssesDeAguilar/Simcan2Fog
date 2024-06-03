package simcan2.UserModel.Edge;

import inet.node.inet.WirelessHost;

module EdgeUserModel extends WirelessHost
{
    parameters:
        @display("i=user");
        
        // Set the driver as "commander" of the module
        numApps = 1;
        app[0].typename = "EdgeUserModelDriver";
}