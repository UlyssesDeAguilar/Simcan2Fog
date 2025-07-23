# Running simulations
The objective of this section is to introduce the reader in how to initiate a simulation. 

## Fundamentals
Simcan2Fog, being an OMNeT++ proyect, allows for two ways of executing it's simulations:
1. Through the GUI (_also referred as Qtenv_): Renders the simulation modules and animates the message flow.
2. Through the CLI (_also referred as Cmdenv_): This mode is recommended for speed and going straight for simulation results.

The current prepared scenarios for simulation are:


## Running simulations through the GUI
### Using the OMNeT++ IDE
The easiest way to run in this mode is through the integrated OMNeT++ IDE.
1. Quickstart the IDE either by typing on a terminal ```omnetpp``` or by finding it's icon on the launchpad.
2. Once the simulator starts up, navigate to the proyect ```Simcan2Fog```.
3. Open a folder wich contains the simulation you want to run.  
    _The current prepared scenarios for simulation are:_
    * ```simulations/EdgeScenario```
    * ```simulations/EdgeMediumScenario```
    * ```simulations/EdgeBigScenario```
    * ```showcases/iot```
    * ```showcases/proxy```
    * ```showcases/dc```
4. Click upon the ```omnetpp.ini``` file
5. Press the play button located in the upper tool belt.  
    _Note: the hover text of the button is **Run**_
6. If it's the first time the simulation is launched, the IDE will notify you that a new "Launch Configuration" was created. Click on **OK**
7. The IDE may prompt you to switch to release configuration. It's recommended to hit **OK** so the performance will be the best one.
8. Once the GUI starts up it will prompt you to select the proper configuration. Select the one that suits you best.
9. The grafical interface will render the modules and channels. All that is left to start the simulation is to hit the play button.
10. At any time you can stop the simulation. In order to gather the results up to this point it's necessary to click on the "finish flag" (technically _Conclude simulation_ button) in order to let the modules flush their statistics to the result files.

### Using the CLI
Just navigate to the corresponding folder and run the command ```s2f```  
_Note: The simulator and INET must both be compiled beforehand_

## Running simulations through the CLI
### The ```s2f``` command
Navigate to the simulation folder, over there you'll just need to run the command ```s2f``` like in this example
```
$ s2f -u Cmdenv -c targetDc
```
The ```-c``` option allows you to select the configuration to be run. If you're unsure of how many configurations are available run ```s2f -a``` which will list all available options. An example:
```
$ s2f -a
OMNeT++ Discrete Event Simulation  (C) 1992-2024 Andras Varga, OpenSim Ltd.
Version: 6.1, build: 241008-f7568267cd, edition: Academic Public License -- NOT FOR COMMERCIAL USE
See the license for distribution terms and warranty disclaimer

Setting up Qtenv...

Config General: 1
Config targetDc: 1
Config targetFog: 1

End.
```
_Note: Watch out for the General configuration as it's generally used as an "abstract" parent configuration. That could imply that there's missing parametrization that will cause the simulation to fail. On the GUI it's not available to be selected since it's marked with the_ ```abstract=true```_property_.

## The ```run-sim``` command
This command takes the ```s2f``` script to a next level. As before, navigate to the proper simulation folder and run ```run-sim```. It will prompt you to select the proper configuration. An example:
```
$ run-sim 
The General configuration has been ommited by default, to enable it run the script with --enable-general-conf flag

Select an option:
1. targetDc
2. targetFog
Enter your choice: 1
Processing targetDc...
OMNeT++ Discrete Event Simulation  (C) 1992-2024 Andras Varga, OpenSim Ltd.
Version: 6.1, build: 241008-f7568267cd, edition: Academic Public License -- NOT FOR COMMERCIAL USE
See the license for distribution terms and warranty disclaimer

Setting up Cmdenv...

Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/simulations:  8
Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/src:  104
Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/tests/modules:  7
Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/tests/networks:  4
Loading NED files from /home/ulysses/omnetpp-6.1/Simcan2Fog/showcases:  4
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/examples:  185
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/showcases:  76
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/src:  1187
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/tests/validation:  5
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/tests/networks:  6
Loading NED files from /home/ulysses/omnetpp-6.1/inet4.5/tutorials:  20

Preparing for running configuration targetDc, run #0...
Assigned runID=targetDc-0-20250723-21:54:09-9555
Setting up network "SimpleFogNetwork"...
Initializing...

Running simulation...
** Event #0   t=0   Elapsed: 1.1e-05s (0m 00s)  0% completed  (0% total)
     Speed:     ev/sec=0   simsec/sec=0   ev/simsec=0
     Messages:  created: 1834   present: 1834   in FES: 76
** Event #25435   t=3600   Elapsed: 0.099439s (0m 00s)  100% completed  (100% total)
     Speed:     ev/sec=255803   simsec/sec=36205.6   ev/simsec=7.06528
     Messages:  created: 15176   present: 1918   in FES: 6

<!> Simulation time limit reached -- at t=3600s, event #25435

Calling finish() at end of Run #0...
[INFO]	SimpleFogNetwork.cloudProvider.tcp: finishing with 2 connections open.
[INFO]	SimpleFogNetwork.messageQueue.tcp: finishing with 10 connections open.
[INFO]	SimpleFogNetwork.netStack.tcp: finishing with 2 connections open.
[INFO]	Dispatching message to service, protocol = udp(63), servicePrimitive = 1, inGate = (omnetpp::cGate)in[3] <-- dnsResolver.socketOut, outGate = (omnetpp::cGate)out[0] --> udp.appIn, message = (Request)close.
[INFO]	SimpleFogNetwork.dns.tcp: finishing with 1 connections open.
[INFO]	SimpleFogNetwork.dc.stack.tcp: finishing with 4 connections open.
[INFO]	Dispatching message to service, protocol = udp(63), servicePrimitive = 1, inGate = (omnetpp::cGate)in[3] <-- dnsResolver.socketOut, outGate = (omnetpp::cGate)out[0] --> udp.appIn, message = (Request)close.
[INFO]	core[0]: utilization time: 3599 proportion 0.999722
[INFO]	core[1]: utilization time: 0 proportion 0
[INFO]	core[2]: utilization time: 0 proportion 0
[INFO]	core[3]: utilization time: 0 proportion 0
[INFO]	core[4]: utilization time: 0 proportion 0

... Ommited output...

[INFO]	core[7]: utilization time: 0 proportion 0
[INFO]	SimpleFogNetwork.sensor.tcp: finishing with 0 connections open.
[INFO]	SimpleFogNetwork.controller.tcp: finishing with 1 connections open.
[INFO]	core[0]: utilization time: 0 proportion 0
[INFO]	core[1]: utilization time: 0 proportion 0
[INFO]	core[2]: utilization time: 0 proportion 0
[INFO]	core[3]: utilization time: 0 proportion 0
[INFO]	Execution results:
[INFO]	 + Total simulation time (simulated):3599 seconds 
[INFO]	 + Total execution time (real):0 seconds
[INFO]	Dispatching message to service, protocol = udp(63), servicePrimitive = 1, inGate = (omnetpp::cGate)in[3] <-- apps.socketOut[0], outGate = (omnetpp::cGate)out[0] --> udp.appIn, message = (Request)destroy.
[INFO]	Dispatching message to service, protocol = tcp(58), servicePrimitive = 1, inGate = (omnetpp::cGate)in[3] <-- apps.socketOut[0], outGate = (omnetpp::cGate)out[1] --> tcp.appIn, message = (Request)DESTROY.
[INFO]	Dispatching message to service, protocol = udp(63), servicePrimitive = 1, inGate = (omnetpp::cGate)in[8] <-- resolver.socketOut, outGate = (omnetpp::cGate)out[0] --> udp.appIn, message = (Request)close.
[INFO]	SimpleFogNetwork.actuator[0].tcp: finishing with 0 connections open.
[INFO]	SimpleFogNetwork.actuator[0].app[0]: received 59 packets

End.
Done! check the results/ folder and run the script in analysis/ folder to extract the results! 
``` 