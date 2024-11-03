package s2f.core.simdata;
import s2f.core.simdata.engine.DataEngine;

// This module encapsulates and collects data sources for simulation.
module SimData
{
    parameters:
        @display("i=block/cogwheel");
    submodules:
        manager: DataManager {
            @display("p=47,93;i=block/rxtx");
        }

        repository: DataRepository {

            @display("p=135,132;i=block/buffer2");
        }
        engine: <default("JsonDataEngine")> like DataEngine {
            @display("p=135,42;i=block/cogwheel");
        }
}
