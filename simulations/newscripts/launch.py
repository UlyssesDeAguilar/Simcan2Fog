#!/usr/bin/env python3.8
# *_* coding: utf-8 *_*

"""
module launch - Launches simulations, processes data and draws time2d graphic.

This module launches the specified simulation with Simcan2Cloud (if asked) and
stores the raw result, processes the result of a simulation (if asked) and
stores the processed result and finally generates the graphics selected in the JSON file
"""


from contextlib import contextmanager
from pathlib import Path
import asyncio
import os
import os.path
import sys
import ast


from graphs import time2d, user2d, user_time, cpuTime2d, cpuUsage2d
from job_utils import execute_command
from config import *



@contextmanager
def cd(path):
    """
    Generate a context in which the current directory is other.

    Keyword arguments:
        path -- the new current working directory
    """
    oldpwd = os.getcwd()
    os.chdir(os.path.expandvars(os.path.expanduser(path)))
    try:
        yield
    finally:
        os.chdir(oldpwd)

def interpret_constant(c): #Transforms the given string into an integer or float depending of its type
    try:
        if str(int(c)) == c: return int(c)
    except ValueError:
        pass
    try:
        if str(float(c)) == c: return float(c)
    except ValueError:
        return c

def transform_string(arguments):
    for k in arguments: #All variables were converted to string when they were uploaded to the queue manager so here they're converted to their corresponding type     
        if"[" in arguments[k]:
            try:
                arguments[k]= ast.literal_eval(arguments[k])  #Tries to use an evaluate function to transform lists into their corresponding type
            except ValueError:
                arguments[k]=list(arguments[k]) 
        else:
            arguments[k]=interpret_constant(arguments[k])

def main(argc, argv):
    """
    Check whether arguments are valid or not before calling launcher.

    Keyword arguments:
        argc -- the number of arguments
        argv -- the list of arguments
    """


    

    arguments={}
    for i in range(argc):
        arguments = {**arguments , **dict(x.replace("·",",").replace("*",'"').split("=") for x in argv[i].split(";"))} #Creates a dictionary containing all values passed through the queue manager, which used the format "key=value,"
    


    



    transform_string(arguments)


    for i in range(arguments['num_simulations']):
        code = launcher(arguments)


def launcher(vars):
    """
    Call methods when needed to launch the simulations.

    Keyword arguments:
        action -- the action to perform (must be a valid Action)
        current_sim -- integer in [0, num_simulations)
        name -- name of the experiment
        num_vars -- number of variables
        max_vars -- list with the number of possible values of each variable
        file_labels -- path of the labels file
        simulations_path -- path of the simulations folder

    Returns:
        The error code (0 means no error)
    """
    code = 0
    print(f"RUNNING {vars['config_name']}: {vars['current_sim']}")
    
    
    labels=vars["labels"]
    maxCoords= vars['max_coords']
    
    config_name=vars['config_name']
    current_sim=vars['current_sim']
    action= vars['action']

    try:
        with cd(vars['simulations_path']):
            output = None
            ofile = raw_output_folder(config_name) + simulation_output_file+f'{current_sim}.dat' #This is the path and the file name that will be used. Both can be changed in the config script
            if action == "simulate":
                Path(raw_output_folder(config_name)).mkdir(parents=True, exist_ok=True)

                str_cmd = command_run(config_name, current_sim, vars['cluster']) #Obtains the command that will be lauched from config.py

                cmd = execute_command(str_cmd, merge_output=True,
                                      filter=lambda line: filter in line)
                print("Executed command:  ", str_cmd)                                      

                olines, _, _ = asyncio.run(cmd)

                with open(ofile, "w+") as f: #Writes the output of the command launched previously
                    output = []
                    for line in olines:
                        if filter in line:
                            output.append(line)
                            f.write(line + os.linesep)
            elif action == "process": #Reads the output of another test done previously and saves it in output
                with open(ofile, "r") as f:
                    output = f.readlines()

            data = {}
            if action != "plot":
                if output is None or len(output) == 0:
                    print(f"Empty output {ofile}", file=sys.stderr)
                    code = 11
                else:
                    for line in output: 
                        i=0
                        target = None
                        for key in targets.keys(): #Reads the raw data in search for the defined targets
                            if targets[key] in line:
                                line = line.split(targets[key])[1]
                                target=key
                                break


                        if target is not None:
                            if target not in data:
                                data[target] = []
                            data_array = line.strip().split()
                            data[target].append(data_array) #Saves data for each target
                    Path(processed_output_folder(config_name)).mkdir(parents=True, 
                                                       exist_ok=True)
                    for target in data: #For every target defined in config.py, parses the data and writes the proccesed information in the output folder
                        if len(data[target]) > 0:
                            subfile = processed_output_folder(config_name) + simulation_output_file+f'{target}_{current_sim}.dat' 
                            with open(subfile, "w+") as f:
                                for data_line in data[target]:
                                    f.write(separator.join(
                                            [str(item) for item in data_line]
                                            ) + os.linesep)
            else:
                for target in targets:  
                    subfile = processed_output_folder(config_name) + simulation_output_file+f'{target}_{current_sim}.dat'                  

                    if os.path.isfile(subfile):
                        rawdata = None
                        with open(subfile, "r") as f: #Reads data from a proccessed file and saves the data for each target
                            rawdata = f.readlines()
                        data[target] = []
                        for line in rawdata:
                            aux = line.strip()
                            if aux != "":
                                data[target].append(aux.split(separator))

            Path(f"{output_graphs_folder}/{config_name}/").mkdir(parents=True,
                                                          exist_ok=True)


            

            if code == 0:
                variables = []
                aux = int(current_sim)
                for max_var in maxCoords[::-1]:
                    variables.append(aux % max_var)
                    aux = aux // max_var
                variables = variables[::-1]
                for i in range(len(labels)):
                    if len(labels[i]) <= variables[i]:
                        print(f"{variables[i]} label not found (out of bounds) "
                              f" for the variable number {i}", file=sys.stderr)
                        code = 13
                        break
            if code == 0:
                megalabel = "-".join([str(labels[i][variables[i]])
                                        for i in range(len(labels))])   #Creates the label to name each graph

                selected_graphs=[]
                params=[]

                try:    #Checks if graphs was defined in the JSON file
                    graphs=vars['graphs']   
                    print(graphs)
                    for i in range(len(graphs)):
                        selected_graphs.append(graphs[i]['name'])


                

                    for graph in selected_graphs:
                        for i in range(len(graphs)):  #Launches the graphs selected in the JSON file with the defined parameters
                            params={ key:value for key, value in graphs[i].items() if key!= 'name' }
                            graph_output_file=f'{output_graphs_folder}/{config_name}/{graph}'
                            Path(graph_output_file).mkdir(parents=True, exist_ok=True)
                            graph_output_file+=f'/{megalabel}_{current_sim}'
                            output_graphs(graph,graph_output_file ,data, megalabel, **params)
                except  KeyError: # If not, warns the user that no graphs will be produced in this script
                    print("atribute graph not defined in the JSON file, launch.py won't produce graphs")            
   
                
        print("END!")
    except EnvironmentError as e:
        print(e, file=sys.stderr)
        code = 10

    return code


if __name__ == "__main__":
    main(len(sys.argv) - 1, sys.argv[1:])
