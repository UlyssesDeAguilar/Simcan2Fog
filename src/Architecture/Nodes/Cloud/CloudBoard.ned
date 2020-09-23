package simcan2.Architecture.Nodes.Cloud;

import simcan2.Architecture.Nodes.Blade;
import simcan2.Architecture.Network.Switch.Switch;
import simcan2.Architecture.Network.DataCenterNetwork.*;

//
// Node Board that contains "numBlades" blades and one switch
//
module CloudBoard{
    
    parameters:
        int numBlades;
        string channelType;
        
    gates:
        input in;
        output out;   

    submodules:
        
        blade[numBlades]: CloudBlade;		// Blades
        
        switch: Switch{				// Switch
            parameters:
                type = "board";
                
        	gates:
        		in[numBlades+1];	
      	   		out[numBlades+1];           
        }
        
    connections:  
            
        // Connections between the blades and the switch          
    	for i=0..numBlades-1 {
		   blade[i].in <-- <channelType> like IDataCenterChannel  <-- switch.out++;
		   blade[i].out --> <channelType> like IDataCenterChannel  --> switch.in++;
		}
		
		// Connections between the board and the switch
       	in --> switch.in++;
       	out <-- switch.out++; 
}

