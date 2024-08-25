import s2f.core.simdata.mysql.SimSchemaMySQL;
import querier.SimSchemaQuerier;

network Unconnected
{
    // parameters:
    //    string class;
    submodules:
    //    simschema: <class>;
        simschema: SimSchemaMySQL;
        querier: SimSchemaQuerier;
}