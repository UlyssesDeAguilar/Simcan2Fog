package simcan2.Core.SimData;
import simcan2.Core.SimSchema.SimSchema;
import simcan2.Core.DataManager.DataManager;

// This module encapsulates and collects data sources for simulation.
module SimData
{
    submodules:
        manager: DataManager;
        simSchema: <> like SimSchema;
        // Maybe in the future add a topology reader for resource detection ?
}