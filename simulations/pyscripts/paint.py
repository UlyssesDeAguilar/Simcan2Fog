#!/usr/bin/env python3.8
# *_* coding: utf-8 *_*

"""
module paint - Draws plots related with a batch of simulations.

This module draws all the plots that can only be drawn when all the
simulations end (3d plots) and stores them in the specified folder
"""

__version__ = "1.0.0"
__author__ = "Hernán"  # only code writers
__email__ = "hernanindibil.cruz@uclm.es"
__maintainer__ = "Hernán"  # the person who will fix bugs and make improvements
# __copyright__ = "Copyright 2020, Simcan2Cloud"
# __license__ = "GPL"
__status__ = "Prototype"  # Prototype, Development or Production
__credits__ = ["Hernán I. de la Cruz", "Adrián Bernal"]

from datetime import datetime
from pathlib import Path
import os
import os.path
import pandas as pd
import sys

from graphs import cost2d, cost3d, provided2d, catplot2d, catplot2d2, boxplot, catplotp2d
from job_utils import Cluster, Action, _get_natural, _get_cluster, _get_action
from launch import cd, separator


def main(argc, argv):
    """
    Check whether arguments are valid or not before calling launcher.

    Keyword arguments:
        argc -- the number of arguments, must be 8
        argv -- the list of arguments
    """
    if (argc != 9):
        script = os.path.split(sys.argv[0])[1]
        print("Syntax: " + script + " <cluster> <action> <name>" +
              " <x_coord> <y_coord> <file_labels>" +
              " <min_key> <max_key> <simulations_folder>",
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
    name = argv[2]
    num_vars = _get_natural(argv[3], False)
    if num_vars is None:
        print("num_vars must be an integer greater than zero", file=sys.stderr)
        sys.exit(3)
    aux = argv[4].split(":")
    max_vars = [_get_natural(s, False) for s in aux]
    if None in max_vars:
        print("max_vars must be a list of integers greater than zero "
              "separated by :", file=sys.stderr)
        sys.exit(4)
    simulations_folder = argv[8]
    if not os.path.isdir(simulations_folder):
        print("Simulations folder doesn't exist!", file=sys.stderr)
        sys.exit(5)
    file_labels = argv[5]
    if file_labels != "auto":
        if not os.path.isfile(file_labels):
            print("Labels file doesn't exist!", file=sys.stderr)
            sys.exit(6)
        file_labels = os.path.abspath(file_labels)
    min_key = argv[6]
    try:
        min_key = float(min_key)
    except ValueError:
        print("min_key must be a real number!", file=sys.stderr)
        sys.exit(7)
    max_key = argv[7]
    try:
        max_key = float(max_key)
    except ValueError:
        print("max_key must be a real number!", file=sys.stderr)
        sys.exit(8)
    code = launcher(action, name, num_vars, max_vars, file_labels,
                    min_key, max_key, simulations_folder)
    sys.exit(code)


def launcher(action, name, num_vars, max_vars, file_labels,
             min_key, max_key, simulations_path):
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
    try:
        with cd(simulations_path + os.path.sep + "exp_cost"):
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
            df_table = None
            df_3d = None
            df_3dcost = None
            with_cost = True
            file_table = f"results/{name}/dat/output_table.dat"
            file_3d = f"{name}/dat/output_graph.dat"
            file_3dcost = f"{name}/dat/output_graph_cost.dat"
            if action != Action.PAINT:
                num_simulations = max_vars[0]
                for max_var in max_vars[1:]:
                    num_simulations *= max_var
                now = datetime.now()
                strnow = now.strftime("[%H:%M:%S - %d/%m/%Y]")
                timed_name = f"##{strnow}: {name}"
                graph_data = []
                cost_data = []
                table_data = []
                table_cost_cols = ["n", "Sim", "PProvi", "PRProvi", "RProvi",
                                   "TotalUnpro", "RegUnpro", "PriUnpro",
                                   "CostOffer", "AcceptOffer",
                                   "UsersAcceptOffer", "RentTimeouts",
                                   "NoSub", "Sub", "TotalCost"]
                table_nocost_cols = ["n", "Sim", "MaxSub", "MeanSub",
                                     "TotalUnpro", "NoSub", "Sub", "NUsers", "AcceptOffer",
                                   "UsersAcceptOffer"]
                curr_3d = []
                curr_co = []
                maxcount = 3
                types_no_cost = [float, float, int, int, int, int, int, int]
                types_cost = [int, int, int, int, int, int, float,
                              int, int, int, int, int, float]
                for curr_sim in range(num_simulations):
                    vars = []
                    aux = curr_sim
                    for max_var in max_vars[::-1]:
                        vars.append(aux % max_var)
                        aux = aux // max_var
                    vars = vars[::-1]
                    sim_name = "-".join([labels[i][vars[i]]
                                         for i in range(len(labels))])
                    dat_3d = False
                    dat_co = False
                    dat_t = False
                    count = 0
                    str_3d = "#___3d#"
                    str_co = "#___3dcost#"
                    str_t = "#___t#"
                    with open(f"{name}/raw/test_{curr_sim}.dat", 'r') as f:
                        for rawline in f:
                            if count == maxcount:
                                break
                            if not dat_t and str_t in rawline:
                                data = rawline.split(str_t)[1].strip().split()
                                if len(data) == 8 and maxcount == 3:
                                    maxcount = 2
                                typelist = types_cost
                                if maxcount == 2:
                                    typelist = types_no_cost
                                data = [typelist[i](data[i])
                                        for i in range(len(data))]
                                data = [curr_sim,
                                        sim_name
                                        ] + data
                                table_data.append(data)
                                dat_t = True
                                count += 1
                                continue
                            if not dat_3d and str_3d in rawline:
                                data = float(rawline.split(str_3d)[1].strip())
                                curr_3d.append(data)
                                if curr_sim != 0 and \
                                        (curr_sim + 1) % max_vars[0] == 0:
                                    graph_data.append(curr_3d)
                                    curr_3d = []
                                dat_3d = True
                                count += 1
                                continue
                            if maxcount == 3 and \
                                    not dat_co and str_co in rawline:
                                data = float(rawline.split(str_co)[1].strip())
                                curr_co.append(data)
                                if curr_sim != 0 and \
                                        (curr_sim + 1) % max_vars[0] == 0:
                                    cost_data.append(curr_co)
                                    curr_co = []
                                dat_co = True
                                count += 1
                                continue
                    if count == 2 and not dat_co:
                        maxcount = 2
                    if count != maxcount:
                        err_file = f"{name}/raw/test_{curr_sim}.dat"
                        print("Couldn't find data in file " + err_file,
                              file=sys.stderr)
                        code = 11
                if code == 0:
                    Path(f"results/{name}/dat").mkdir(parents=True,
                                                      exist_ok=True)
                    with_cost = maxcount == 3
                    if with_cost:
                        df_table = pd.DataFrame(table_data,
                                                columns=table_cost_cols)
                        table_data = [table_cost_cols] + table_data
                    else:
                        df_table = pd.DataFrame(table_data,
                                                columns=table_nocost_cols)
                        table_data = [table_nocost_cols] + table_data
                    df_table = df_table.set_index("n")
                    with open(file_table, "w+") as f:
                        for data_line in table_data:
                            f.write(separator.join([
                                    str(item) for item in data_line]) +
                                    os.linesep)
                    df_3d = pd.DataFrame(graph_data)
                    with open(file_3d, "w+") as f:
                        f.write(timed_name + os.linesep)
                        for data_line in graph_data:
                            f.write(separator.join([
                                    str(item) for item in data_line]) +
                                    os.linesep)
                    if with_cost:
                        df_3dcost = pd.DataFrame(cost_data)
                        with open(file_3dcost, "w+") as f:
                            f.write(timed_name + os.linesep)
                            for data_line in cost_data:
                                f.write(separator.join([
                                        str(item) for item in data_line]) +
                                        os.linesep)
            else:
                df_table = pd.read_csv(file_table, sep=separator)
                df_table = df_table.set_index("n")
                df_3d = pd.read_csv(file_3d, skiprows=1, header=None,
                                    sep=separator)
                with_cost = len(df_table.columns) > 7
                if with_cost:
                    df_3dcost = pd.read_csv(file_3dcost, sep=separator,
                                            skiprows=1, header=None)
            if code == 0:
                print(df_table)
                print(df_3d)
                if with_cost and any([i > 1 for i in max_vars]):
                    print(df_3dcost)
                    Path(f"results/{name}/cost2d").mkdir(parents=True,
                                                         exist_ok=True)
                    if max_vars[0] > 1:
                        cost2d(df_3dcost, name, labels[0], labels[1], "2dcost",
                               f"results/{name}/cost2d", transpose=False,
                               extensions=["png"])
                    if max_vars[1] > 1:
                        cost2d(df_3dcost, name, labels[0], labels[1], "2dcost",
                               f"results/{name}/cost2d", transpose=True,
                               extensions=["png"])
                    if max_vars[0] > 1 and max_vars[1] > 1:
                        cost3d(df_table, f"Income - {name}",
                               f"results/{name}/output_3d_graph_cost",
                               min_key, max_key,
                               extensions=["png"])
                Path(f"results/{name}/provided2d").mkdir(parents=True,
                                                         exist_ok=True)
                for i in range(max_vars[1]):
                    provided2d(df_table, i, max_vars[0], with_cost,
                               f"results/{name}/provided2d/" +
                               f"output_2dprovided_{i}",
                               extensions=["png"])

                # Leer datos de tiempo de espera de cada simulación
                targets = ["init", "success", "timeout", "rentTimeout",
                           "unprovided", "cost", "rentTimeoutCost",
                           "compensation", "cpuTime", "cpuUsage", "machinesInUse", "activeMachines", "activeOrInUseMachines", "machinesInUseForecast"]
                data = {}
                df_times = pd.DataFrame()
                for current_sim in range(num_simulations):
                    for target in targets:
                        subfile = f"{name}/" + \
                                  f"dat/test_{target}_{current_sim}.dat"
                        data[target] = []
                        if os.path.isfile(subfile):
                            rawdata = None
                            with open(subfile, "r") as f:
                                rawdata = f.readlines()
                            for line in rawdata:
                                aux = line.strip()
                                if aux != "":
                                    data[target].append(aux.split(separator))

                    vars = []
                    aux = current_sim
                    for max_var in max_vars[::-1]:
                        vars.append(aux % max_var)
                        aux = aux // max_var
                    vars = vars[::-1]
                    labe_subtime = labels[0][vars[0]]
                    labe_nodes = labels[1][vars[1]]

                    data_time = data['success'] + data['timeout']
                    serie_time_all = pd.Series(data=[float(d[1]) if float(d[1])>=0.0 else float(d[2]) for d in data_time], index=pd.Index(name='user', data=[int(d[0])-1 for d in data_time]), name='time', dtype='float64')
                    serie_wait_time = pd.Series(data=[float(d[3]) for d in data_time], index=pd.Index(name='user', data=[int(d[0])-1 for d in data_time]), name='time', dtype='float64')
                    serie_time_provided = pd.Series(data=[float(d[1]) for d in data_time], index=pd.Index(name='user', data=[int(d[0])-1 for d in data_time]), name='time', dtype='float64')
                    serie_init = pd.Series(data=[float(d[1]) for d in data['init']], index=pd.Index(name='user', data=[int(d[0]) for d in data['init']]), dtype='float64')
                    df_times_sim = pd.DataFrame({'init': serie_init, 'time_provided': serie_time_provided, 'wait_time': serie_wait_time, 'time_all': serie_time_all})
                    #df_times_sim = pd.DataFrame({'init': serie_init, 'time_provided': serie_time_provided, 'time_all': serie_time_all})
                    df_times_sim['nodes'] = labe_nodes
                    df_times_sim['subtime'] = labe_subtime
                    df_times_sim = df_times_sim.set_index(['subtime', 'nodes'], append=True)
                    #print(labe_nodes)
                    #print(df_times_sim.describe(include='all'))

                    if df_times.empty:
                        df_times = df_times_sim
                    else:
                        df_times = df_times.append(df_times_sim)

                Path(f"results/{name}/catplot2d").mkdir(parents=True,
                                                         exist_ok=True)

                # catplot2d(df_times.reset_index(), "title",
                #                f"results/{name}/catplot2d/" +
                #                f"output_2dtime",
                #                extensions=["png"])

                # Removes 128nodes data
                # df_times = df_times.drop(['128nodes'], level=2)

                # Remove if has all 0
                # df_times = df_times.loc[df_times[~(df_times['time'] <= 0).values].index.get_level_values('user').unique()]

                # Remove any 0
                # df_times = df_times.loc[df_times['time']>0]
                df_times_provided = df_times.loc[df_times['time_provided'] >= 0]

                for i, label in enumerate(labels[0]):
                    graph_title = "CPU speed (MIPS) " + label
                    # ## Only provided. Filtered dataframe.
                    # df_times_provided.xs(label, level=1, drop_level=False).time_provided.groupby('nodes').describe(
                    #     include='all').to_csv(
                    #     f"results/{name}/dat/output_catp2dtime_provided_first_wait_{label}_{i}.dat", sep="\t")
                    # catplotp2d(df_times_provided.xs(label, level=1, drop_level=False), i, max_vars[0], graph_title,
                    #            f"results/{name}/catplot2d/" +
                    #            f"output_catp2dtime_provided_first_wait_{label}_{i}",
                    #            extensions=["png"],
                    #            column="time_provided")
                    # ## Only provided. All waiting times.
                    # df_times_provided.xs(label, level=1, drop_level=False).wait_time.groupby('nodes').describe(
                    #     include='all').to_csv(f"results/{name}/dat/output_catp2dtime_provided_all_wait_{label}_{i}.dat",
                    #                           sep="\t")
                    # catplotp2d(df_times.xs(label, level=1, drop_level=False), i, max_vars[0], graph_title,
                    #            f"results/{name}/catplot2d/" +
                    #            f"output_catp2dtime_provided_all_wait_{label}_{i}",
                    #            extensions=["png"],
                    #            column="wait_time")
                    # ## All users. All waiting times.
                    # df_times.xs(label, level=1, drop_level=False).wait_time.groupby('nodes').describe(
                    #     include='all').to_csv(f"results/{name}/dat/output_catp2dtime_all_all_wait_{label}_{i}.dat",
                    #                           sep="\t")
                    # catplotp2d(df_times.xs(label, level=1, drop_level=False), i, max_vars[0], graph_title,
                    #            f"results/{name}/catplot2d/" +
                    #            f"output_catp2dtime_all_all_wait_{label}_{i}",
                    #            extensions=["png"],
                    #            column="wait_time")
                    # ## All users. First time wait time.
                    # df_times.xs(label, level=1, drop_level=False).time_all.groupby('nodes').describe(
                    #     include='all').to_csv(f"results/{name}/dat/output_catp2dtime_all_first_wait_{label}_{i}.dat",
                    #                           sep="\t")
                    # catplotp2d(df_times.xs(label, level=1, drop_level=False), i, max_vars[0], graph_title,
                    #            f"results/{name}/catplot2d/" +
                    #            f"output_catp2dtime_all_first_wait_{label}_{i}",
                    #            extensions=["png"],
                    #            column="time_all")

                    print(label)
                    print(df_times.init.groupby('nodes').describe(include='all'))

                    # catplot2d2(df_times.xs(label, level=1, drop_level=False), i, max_vars[0], graph_title,
                    #            f"results/{name}/catplot2d/" +
                    #            f"output_cat2dtimewo_{i}",
                    #            extensions=["png"])

                    # boxplot(df_times.xs(label, level=1, drop_level=False), i, max_vars[0], graph_title,
                    #            f"results/{name}/catplot2d/" +
                    #            f"output_box2dtimewo_{i}",
                    #            extensions=["png"])

                for i in range(max_vars[1]):
                    pass
    except EnvironmentError as e:
        print(e, file=sys.stderr)
        code = 10

    return code


if __name__ == "__main__":
    main(len(sys.argv) - 1, sys.argv[1:])
