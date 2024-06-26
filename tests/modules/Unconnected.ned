import s2f.Core.SimSchema.MySQL.SimSchemaMySQL;
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