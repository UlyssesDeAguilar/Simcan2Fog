#!/usr/bin/env python3.8
# *_* coding: utf-8 *_*

"""
module plot - Draws plots related with a batch of simulations.

This module draws all the plots that can only be drawn when all the
simulations end (3d plots) and stores them in the specified folder
"""



from datetime import datetime
import math
from pathlib import Path
import os
import os.path
import pandas as pd
import sys


from config import *

from launch import cd, transform_string, interpret_constant


def main(argc, argv):
    """
    Check whether arguments are valid or not before calling launcher.

    Keyword arguments:
        argc -- the number of arguments, must be 8
        argv -- the list of arguments
    """
    arguments={}
    for i in range(argc):
        arguments = {**arguments , **dict(x.replace("·",",").replace("*",'"').split("=") for x in argv[i].split(";"))} #This code creates a dictionary containing all values passed through the queue manager, which used the format "key=value,"
    


    



    transform_string(arguments)



    code = launcher(arguments)

def launcher(args):
    """
    Call methods when needed to launch the simulations.

    Keyword arguments:
        action -- the action to perform (must be a valid Action)
        current_sim -- integer in [0, x * y)
        name -- name of the experiment
        num_vars -- number of variables
        max_vars -- list with the number of possible values of each variable
        file_labels -- path of the labels file
        min_key -- min key in graph (float)
        max_key -- max key in graph (float)
        simulations_path -- path of the simulations folder

    Returns:
        The error code (0 means no error)
    """
    code = 0

    labels=args['labels']
    num_vars=args['num_simulations']
    max_vars=args['max_coords']
    name=args['config_name']
    action=args['action']
    try:
        simulation_type=args["simulation_type"]
        columns={}
        for type in simulation_type.keys():      #For every type of simulation (cost no cost etc.) creates a dictionary with the corresponding columns
            for i in range(len(simulation_type[type])):
                if type not in columns:
                     columns[type]= globals()[simulation_type[type][i]]
                else:
                     columns[type]=columns[type]+globals()[simulation_type[type][i]]
    except KeyError:
        print("omitted plot.py because there´s no simulation_type defined in the JSON file")
        sys.exit(1)
        

        



    

    with cd(args['simulations_path'] + os.path.sep + output_folder):

        df_table = {}
        file_table = f"{output_plot_graphs_folder(name)}/dat/output_table.dat"



        num_simulations = math.prod(max_vars)

        if action != 'plot':
            
            table_data=dict()
            
            for curr_sim in range(num_simulations):
                vars = []
                aux = curr_sim
                for max_var in max_vars[::-1]:
                    vars.append(aux % max_var)
                    aux = aux // max_var
                vars = vars[::-1]
                sim_name = "-".join(str(labels[i][vars[i]]) for i in range(len(labels)))
                
                with open(f"{name}/raw/{simulation_output_file}{curr_sim}.dat", 'r') as f:      #Processes all data from the files produced by launch.py  
                    
                    for rawline in f:
                        for type in simulation_type.keys():
                            if type in rawline:
                                data = rawline.split(type)[1].strip().split()
                                data=[eval(data[i]) for i in range(len(data))] #Determines if the data read is an int or a float.
                                data = [curr_sim, sim_name ] + data

                                if type not in table_data:
                                    table_data[type]=[data]
                                else:
                                    table_data[type].append(data)
                    
                
                Path(f'{output_plot_graphs_folder(name)}/dat').mkdir(parents=True, exist_ok=True)
                for type in simulation_type.keys():

                    df_table[type] = pd.DataFrame([table_data[type][0]], columns=columns[type]) #Creates the dataframe with the previously read data
                    df_table[type] = df_table[type].set_index("n")       

            
            print('\n', table_data)
            for type in simulation_type.keys():
                with open(f'{file_table}_{type}', "w+") as f:
                    f.write(separator.join(columns[type])+os.linesep)   #Writes the columns depending on the type of simulation
                    for data_line in table_data[type]:
                        f.write(separator.join([str(item) for item in data_line]) +  os.linesep) #Writes the data in an output file

        else:

            for type in simulation_type.keys():

                df_table[type] = pd.read_csv(f'{file_table}_{type}', sep=separator)
                df_table[type] = df_table[type].set_index("n")

        if code == 0:
            for type in simulation_type.keys():
                df_table[type] = pd.read_csv(f'{file_table}_{type}', sep=separator) #Reads the data 
                df_table[type] = df_table[type].set_index("n")
            
            for type in simulation_type.keys():
                print(df_table[type])
            
            
            selected_graphs=[]
            params=[]

            
            try:    #Checks if graphs_plot was defined in the JSON file
                graphs_plot=args['graphs_plot']
                for i in range(len(graphs_plot)):
                    selected_graphs.append(graphs_plot[i]['name'])
                for graph in selected_graphs: #Launches the graphs selected in the JSON file with the defined parameters
                    for i in range(len(graphs_plot)):
                        params={ key:value for key, value in graphs_plot[i].items() if key!= 'name' }
                        graph_output_file=f'{output_plot_graphs_folder(name)}/{graph}'
                        Path(graph_output_file).mkdir(parents=True, exist_ok=True)
                        graph_output_file+=f'/{sim_name}_prueba'
                        plot_graphs(graph,graph_output_file ,df_table["#___3d#"], **params) 
            except KeyError:     # If not, warns the user that no graphs will be produced in this script
                print('omitted plot.py graph generation because graphs_plot is not defined in the JSON file')                
            
            for i in range(max_vars[1]):
                pass


    return code


if __name__ == "__main__":
    main(len(sys.argv) - 1, sys.argv[1:])
