package s2f.tests.networks;

import s2f.core.simdata.SimData;
import s2f.tests.modules.querier.DataManagerQuerier;

network Unconnected
{
    submodules:
        simData: SimData;
        querier: DataManagerQuerier;
}