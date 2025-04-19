package s2f.management.managers;

simple NodePool
{
    parameters:
        @class(NodePool);
        @display("i=block/table2,,0");
        int estimatedNumberOfNodes = default(-1);
}
