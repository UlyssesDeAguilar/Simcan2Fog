package simcan2.Core.SimData;
import simcan2.Core.SimSchema.SimSchema;
import simcan2.Core.DataManager.DataManager;

// This module encapsulates and collects data sources for simulation.
module SimData
{
    parameters:
        @display("i=block/cogwheel");
    submodules:
        manager: DataManager {
            @display("p=189,123");
        }
        simSchema: <> like SimSchema {
        // Maybe in the future add a topology reader for resource detection ?
            @display("p=52,121");
        }
}
