import s2f.core.simdata.SimData;
import querier.DataManagerQuerier;

network Unconnected
{
    submodules:
        simData: SimData;
        querier: DataManagerQuerier;
}