#!/usr/bin/env python3.8
# *_* coding: utf-8 *_*

"""
module launch - Launches simulations, processes data and draws time2d graphic.

This module launches the specified simulation with Simcan2Cloud (if asked) and
stores the raw result, processes the result of a simulation (if asked) and
stores the processed result and finally generates the time2d graphic associated
with the processed result of the specified simulation.
"""

__version__ = "1.0.0"
__author__ = "Hernán"  # only code writers
__email__ = "hernanindibil.cruz@uclm.es"
__maintainer__ = "Hernán"  # the person who will fix bugs and make improvements
# __copyright__ = "Copyright 2020, Simcan2Cloud"
# __license__ = "GPL"
__status__ = "Prototype"  # Prototype, Development or Production
__credits__ = ["Hernán I. de la Cruz", "Adrián Bernal"]

from contextlib import contextmanager
from pathlib import Path
import asyncio
import os
import os.path
import sys

from graphs import time2d, user2d, user_time, cpuTime2d, cpuUsage2d
from job_utils import Cluster, Action, _get_natural, _get_cluster, _get_action, execute_command


separator = ','


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


def main(argc, argv):
    """
    Check whether arguments are valid or not before calling launcher.

    Keyword arguments:
        argc -- the number of arguments, must be 9, 6 and 7 ignored
        argv -- the list of arguments
    """
    if (argc != 10):
        script = os.path.split(sys.argv[0])[1]
        print("Syntax: " + script + " <cluster> <action> <current_sim> <name>" +
              " <num_vars> <max_vars> <file_labels>" +
              " <ignored> <ignored> <simulations_folder>",
              file=sys.stderr)
        sys.exit(1)

    cluster = _get_cluster(argv[0])
    if cluster is None:
        print("cluster must be [galgo|cirrus]", file=sys.stderr)
        sys.exit(2)
    action = _get_action(argv[1])
    if action is None or action == Action.OMIT:
        print("action must be [simulate|process|paint]", file=sys.stderr)
        sys.exit(2)
    current_sim = _get_natural(argv[2], True)
    if current_sim is None:
        print("current_sim must be a non-negative integer",
              file=sys.stderr)
        sys.exit(3)
    name = argv[3]
    num_vars = _get_natural(argv[4], False)
    if num_vars is None:
        print("num_vars must be an integer greater than zero", file=sys.stderr)
        sys.exit(4)
    aux = argv[5].split(":")
    max_vars = [_get_natural(s, False) for s in aux]
    if None in max_vars:
        print("max_vars must be a list of integers greater than zero "
              "separated by :", file=sys.stderr)
        sys.exit(5)
    num_simulations = max_vars[0]
    for max_var in max_vars[1:]:
        num_simulations *= max_var
    if current_sim >= num_simulations:
        print(f"current_sim is outside [0,{num_simulations})", file=sys.stderr)
        sys.exit(6)
    simulations_folder = argv[9]
    if not os.path.isdir(simulations_folder):
        print("Simulations folder doesn't exist!", file=sys.stderr)
        sys.exit(8)
    file_labels = argv[6]
    if file_labels != "auto":
        if not os.path.isfile(file_labels):
            print("Labels file doesn't exist!", file=sys.stderr)
            sys.exit(7)
        file_labels = os.path.abspath(file_labels)

    code = launcher(cluster, action, current_sim, name, num_vars, max_vars,
                    file_labels, simulations_folder)

    # for i in range(num_simulations):
    #     code = launcher(cluster, action, i, name, num_vars, max_vars,
    #                 file_labels, simulations_folder)

    sys.exit(code)


def launcher(cluster, action, current_sim, name, num_vars, max_vars,
             file_labels, simulations_path):
    """
    Call methods when needed to launch the simulations.

        cluster -- the cluster where to run the jobs (must be a valid Cluster)
        action -- the action to perform (must be a valid Action)
        current_sim -- integer in [0, num_simulations)
        name -- name of the experiment
        num_vars -- number of variables
        max_vars -- list with the number of possible values of each variable
        file_labels -- path of the labels file
        simulations_path -- path of the simulations folder

    Returns:
        The error code (0 mea    Keyword arguments:
ns no error)
    """
    code = 0
    print(f"RUNNING {name}: {current_sim}")
    try:
        with cd(simulations_path):
            output = None
            ofile = f"exp_cost/{name}/raw/test_{current_sim}.dat"
            if action == Action.SIMULATE:
                Path(f"exp_cost/{name}/raw").mkdir(parents=True, exist_ok=True)
                # print("Commanding!", flush=True)
                if cluster == Cluster.GALGO:
                    str_cmd = f"./run_galgo omnetpp.ini -u Cmdenv -c {name} -r {current_sim}"
                elif cluster == Cluster.CIRRUS:
                    str_cmd = f'singularity exec /lustre/home/dc010/abbc/omnetpp.sif /lustre/home/dc010/abbc/Simcan2CloudPruebas/simulations/ScenarioTest/run_cirrus omnetpp.ini -c {name} -r {current_sim} > {ofile}'
                    #str_cmd = f'singularity exec /lustre/home/dc010/abbc/omnetpp.sif /lustre/home/dc010/abbc/Simcan2CloudPruebas/simulations/ScenarioTest/run_cirrus omnetpp.ini -c {name} -r {current_sim} | grep "#___" > {ofile}'
                    #str_cmd = f"singularity exec /lustre/home/dc010/abbc/omnetpp.sif /lustre/home/dc010/abbc/Simcan2CloudPruebas/simulations/ScenarioTest/run_cirrus omnetpp.ini -c {name} -r {current_sim}"
                print(str_cmd)
                cmd = execute_command(str_cmd, merge_output=True)
                #cmd = execute_command(str_cmd, merge_output=True,
                 #                     filter=lambda line: "#___" in line)
                olines, _, _ = asyncio.run(cmd)
                # print("Output:")
                #print(simulations_path)
                # olines = rawout.replace('\r', '').split('\n')
                # with open(ofile, "a+") as f:
                # with open(ofile, "w+") as f:
                #     output = []
                #     for line in olines:
                #         if "#___" in line:
                #             output.append(line)
                #             f.write(line + os.linesep)
            # elif action == Action.PROCESS:
            #     with open(ofile, "r") as f:
            #         output = f.readlines()

            with open(ofile, "r") as f:
                output = f.readlines()

            data = {}
            if action != Action.PAINT:
                if output is None or len(output) == 0:
                    print(f"Empty output {ofile}", file=sys.stderr)
                    code = 11
                else:
                    for line in output:
                        target = None
                        if "#___ini#" in line:
                            line = line.split("#___ini#")[1]
                            target = "init"
                        elif "#Success " in line:
                            line = line.split("#Success ")[1]
                            target = "success"
                        elif "#RentTimeout " in line:
                            line = line.split("#RentTimeout ")[1]
                            target = "rentTimeout"
                        elif "#Unprovided " in line:
                            line = line.split("#Unprovided ")[1]
                            target = "unprovided"
                        elif "#Cost " in line:
                            line = line.split("#Cost ")[1]
                            target = "cost"
                        elif "#RentTimeoutCost " in line:
                            line = line.split("#RentTimeoutCost ")[1]
                            target = "rentTimeoutCost"
                        elif "#Compensation " in line:
                            line = line.split("#Compensation ")[1]
                            target = "compensation"
                        elif "#Timeout " in line:
                            line = line.split("#Timeout ")[1]
                            target = "timeout"
                        elif "#___cpuTime#"  in line:
                            line = line.split("#___cpuTime#")[1]
                            target = "cpuTime"
                        elif "#___cpuUsage#"  in line:
                            line = line.split("#___cpuUsage#")[1]
                            target = "cpuUsage"
                        elif "#___machinesInUse#"  in line:
                            line = line.split("#___machinesInUse#")[1]
                            target = "machinesInUse"
                        elif "#___activeMachines#"  in line:
                            line = line.split("#___activeMachines#")[1]
                            target = "activeMachines"
                        elif "#___activeOrInUseMachines#"  in line:
                            line = line.split("#___activeOrInUseMachines#")[1]
                            target = "activeOrInUseMachines"
                        elif "#___machinesInUseForecast#"  in line:
                            line = line.split("#___machinesInUseForecast#")[1]
                            target = "machinesInUseForecast"
                        if target is not None:
                            if target not in data:
                                data[target] = []
                            data_array = line.strip().split()
                            data[target].append(data_array)
                    Path(f"exp_cost/{name}/dat").mkdir(parents=True,
                                                       exist_ok=True)
                    for target in data:
                        if len(data[target]) > 0:
                            subfile = f"exp_cost/{name}/" + \
                                      f"dat/test_{target}_{current_sim}.dat"
                            with open(subfile, "w+") as f:
                                for data_line in data[target]:
                                    f.write(separator.join(
                                            [str(item) for item in data_line]
                                            ) + os.linesep)
            else:
                targets = ["init", "success", "timeout", "rentTimeout",
                           "unprovided", "cost", "rentTimeoutCost",
                           "compensation", "cpuTime", "cpuUsage", "machinesInUse", "activeMachines", "activeOrInUseMachines", "machinesInUseForecast"]
                for target in targets:
                    subfile = f"exp_cost/{name}/" + \
                              f"dat/test_{target}_{current_sim}.dat"
                    if os.path.isfile(subfile):
                        rawdata = None
                        with open(subfile, "r") as f:
                            rawdata = f.readlines()
                        data[target] = []
                        for line in rawdata:
                            aux = line.strip()
                            if aux != "":
                                data[target].append(aux.split(separator))
            Path(f"exp_cost/results/{name}/user2d").mkdir(parents=True,
                                                          exist_ok=True)
            Path(f"exp_cost/results/{name}/time2d").mkdir(parents=True,
                                                           exist_ok=True)
            Path(f"exp_cost/results/{name}/cpu2d").mkdir(parents=True,
                                                          exist_ok=True)
            labels = None
            llines = None
            if file_labels != "auto":
                with open(file_labels, 'r') as f:
                    llines = f.readlines()
                if llines is None or len(llines) < num_vars:
                    print("Labels file must contain two lines",
                          file=sys.stderr)
                    code = 12
                else:
                    labels = [lline.strip().split() for lline in llines]
            else:
                labels = [[str(j) + " x" + str(i) for j in range(max_vars[i])]
                          for i in range(num_vars)]
            if code == 0:
                vars = []
                aux = current_sim
                for max_var in max_vars[::-1]:
                    vars.append(aux % max_var)
                    aux = aux // max_var
                vars = vars[::-1]
                for i in range(len(labels)):
                    if len(labels[i]) <= vars[i]:
                        print(f"{vars[i]} label not found (out of bounds) "
                              f" for the variable number {i}", file=sys.stderr)
                        code = 13
                        break
            if code == 0:
                megalabel = "-".join([labels[i][vars[i]]
                                        for i in range(len(labels))])
                folder = f"exp_cost/results/{name}/user2d/"
                user2d(data, "User Arrival",
                       folder + f"output_user_hist_{current_sim}",
                       extensions=["png"])
                user_time(data, "User Arrival",
                          folder + f"output_user_scatter_{current_sim}",
                          extensions=["png"])
                graph_title = f"Time - {name} - " + megalabel
                sub_time = labels[0][vars[0]][:-1]
                graph_title = f"Max sub time = {sub_time} h"
                folder = f"exp_cost/results/{name}/time2d/"
                time2d(data, graph_title,
                       folder + f"output_2dtime_bar_{current_sim}",
                       folder + f"output_2dtime_dot_{current_sim}",
                       extensions_bar=["png"],
                       extensions_scatter=["png"])
                folder = f"exp_cost/results/{name}/cpu2d/"
                # graph_title = f"Core time usage \n {name} \n " + megalabel
                # cpuTime2d(data, graph_title,
                #        folder + f"output_2dcpuTime_bar_{current_sim} - {megalabel}",
                #        extensions=["png"])
                # graph_title = f"Cores used \n {name} \n " + megalabel
                # cpuUsage2d(data, graph_title,
                #       folder + f"output_2dcpuUsage_serie_{current_sim} - {megalabel}",
                #       extensions=["png"])

                ###Las commented
                graph_title = f"Core time usage \n {name} \n " + megalabel
                cpuTime2d(data, graph_title,
                       folder + f"time-{megalabel}",
                       extensions=["png"])
                graph_title = f"Cores used \n {name} \n " + megalabel
                cpuUsage2d(data, graph_title,
                      folder + f"usage-{megalabel}",
                      extensions=["png"])
        print("END!")
    except EnvironmentError as e:
        print(e, file=sys.stderr)
        code = 10

    return code


if __name__ == "__main__":
    main(len(sys.argv) - 1, sys.argv[1:])
