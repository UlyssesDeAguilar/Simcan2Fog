package s2f.architecture.net.stack.dispatcher;
import inet.common.MessageDispatcher;

simple ToggableDispatcher
{
    parameters:
        @display("b=100,5,,,,1;");
    gates:
        input in[];
        output out[];
}