"""
config module - This module is responsible for gathering the simulation parameters across all scripts so that they are easily configurable. 
"""

import ast
import os
import graphs

from datetime import datetime
import sys

from numpy import append




def cluster_script(cluster):

    """
    cluster_script: according to the given cluster, returns the appropiate script for each queue manager, which can be added manually.

    !!! WARNING !!! don't change the 'submit_command' key or else the code will not work, its value can be changed normally.
    """
    galgo="simcan_galgo.pbs"
    galgo2="simcan_galgo2.pbs"
    cirrus="simcan_cirrus.pbs"
    return vars()[cluster]



def cluster_commands(cluster, num_simulations, script, launch_task):
    """
    cluster_commands: according to the given cluster and number of simulations or task number, returns the appropiate command for each queue manager, which can be added manually by modifying the cluster dictionary

    !!! WARNING !!! don't change the 'submit_command' key or else the code will not work, its value can be changed normally.
    """
    try:
        now = datetime.now()
        date_time = now.strftime("%Y%m%d%H%M%S")
        

        if (script == "launch"):  #if it's a launch process, then the launch_task is not used 

            galgo = {'submit_command' : f"qsub -J 0-{num_simulations-1} -v ", 'script': script, 'scriptspath' : os.getcwd(),  'timestamp' : date_time, 'num_simulations' : num_simulations}
            cirrus = {'submit_command' : f"sbatch --parsable --array=0-{num_simulations-1} --export=ALL,", 'script': script, 'scriptspath' : os.getcwd(),  'timestamp' : date_time, 'num_simulations' : num_simulations}

            return vars()[cluster]


        if (script=="plot"):  # If it's a plot process, then launch_task is used so the pain process can be executed right after the launch process

            galgo={'submit_command' : f"qsub -W depend=afterok:{launch_task} -v " , 'script': script, 'scriptspath' : os.getcwd(),  'timestamp' : date_time, 'num_simulations' : num_simulations}
            cirrus={'submit_command' : f"sbatch --dependency=afterok:{launch_task} --export=ALL," , 'script': script, 'scriptspath' : os.getcwd(),  'timestamp' : date_time, 'num_simulations' : num_simulations}

            return vars()[cluster]
    except:
        print("Wrong parameters at cluster_commands")

"""
output_folder: This is the folder where all the data of the simulations will be saved. This folder is located in the simulation_path you enter when executing submit.py.
It can be modified by changing the string.

"""

output_folder='exp_cost'


def raw_output_folder(name):
    """
    raw_output_folder: according to the given simulation name, returns the output folder where all raw results will be saved. This path can be modified by changing the returned string. The path must end with a /

    """
    return f'{output_folder}/{name}/raw/' 




def processed_output_folder(name):
    """
    output_folder: according to the given simulation name, returns the output folder where all processed files will be saved. This path can be modified by changing the returned string. The path must end with a /

    """
    return f'{output_folder}/{name}/dat/'

def results_folder(name):
    """
    results_folder: according to the given simulation name, returns the  folder where all results will be saved. This path can be modified by changing the returned string. The path must end with a /

    """
    return f"results/{name}/"


"""
output_file: according to the given simulation, returns the name of the output file where the processed results will be saved. It's located in the path defined by processed_output_folder

"""
simulation_output_file=f'test_'



def command_run(name, current_sim, cluster):
    """
    command_run: according to the given simulation name and current simulation, returns the run command that will be used to execute the simulation.

    """
    galgo = f"./run omnetpp.ini -u Cmdenv -c {name} -r {current_sim}"
    cirrus = f"singularity exec /lustre/home/dc010/abbc/omnetpp.sif /lustre/home/dc010/abbc/Simcan2CloudPruebas/simulations/ScenarioTest/run_cirrus omnetpp.ini -c {name} -r {current_sim}"
    return vars()[cluster]


"""
output_graphs_folder: All resulting graphs will be stored starting from this folder.

"""

output_graphs_folder=f'{output_folder}/results'


def output_plot_graphs_folder(name):
    return f'plot/{name}/'


    
def output_graphs(graph,graph_output_file,data,label,**params):
    """
    output_graphs: Receives parameters from the launch.py scripts and launches the graphs selected in the JSON file.

    """
    graph_to_call = getattr(graphs, graph)
    graph_to_call(data,f'title_{graph}_{label}', graph_output_file,**params)

def plot_graphs(graph,output_file,df_table,**params):
    """
    output_graphs_folder: Receives parameters from the plot.py scripts and launches the graphs selected in the JSON file.

    """
    graph_to_call = getattr(graphs, graph)
    graph_to_call(df_table,output_file,**params)





"""
targets: The launch.py script will process the result files of the simulations targeting the atributes listed here. The results will be saved in a different file for each target

"""
targets = { "init":"#___ini#", 
            "success":"#Success ", 
            "timeout": "#Timeout ", 
            "rentTimeout":"#RentTimeout ",
            "unprovided": "#Unprovided " , 
            "cost": "#Cost ",
            "rentTimeoutCost": "#RentTimeoutCost ", 
            "compensation": "#Compensation ", 
            "cpuTime":"#___cpuTime#",
            "machinesInUse": "#___machinesInUse#",
            "activeMachines": "#___activeMachines#",
            "activeOrInUseMachines": "#___activeOrInUseMachines#",
            "machinesInUseForecast": "#___machinesInUseForecast#"
            }


"""
The following lists represent the columns that will be used when plot.py draws the graphs. These can be selected in the JSON file
"""



cost = ["PProvi", "PRProvi", "RProvi", "RegUnpro", "PriUnpro", "CostOffer", "RentTimeouts","TotalCost"]
no_cost = ["MaxSub", "MeanSub", "NUsers"]
base=["n", "Sim","TotalUnpro","AcceptOffer","UsersAcceptOffer","NoSub", "Sub"]
col_3d=["n", "Sim","TotalCost"]
"""
filter: The launch.py script will use this string to process the raw data generated by the simulator.

"""


filter= "#___"

"""
separator: The launch.py script will separate the processed data using the string declared here.

"""

separator = ','









