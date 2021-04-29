"""
module job_utils - Contains things related to jobs.

This module contains an enumeration of the possible
realizable actions as well as the job class and job
related functions
"""

__version__ = "1.0.0"
__author__ = "Hernán"  # only code writers
__email__ = "hernanindibil.cruz@uclm.es"
__maintainer__ = "Hernán"  # the person who will fix bugs and make improvements
# __copyright__ = "Copyright 2020, Simcan2Cloud"
# __license__ = "GPL"
__status__ = "Prototype"  # Prototype, Development or Production
__credits__ = ["Hernán I. de la Cruz", "Adrián Bernal"]

import asyncio
import json
import numpy as np
import os
import subprocess
import sys

from datetime import datetime
from enum import Enum
from jsonschema import validate


# def get_var_names(size):
#     return ["x" + str(i) for i in range(size)]


def load_json(file_name):
    """Load the given schema available."""
    data = None
    try:
        with open(file_name, 'r') as file:
            data = json.load(file)
    except Exception as err:
        print(err)
    return data


def validate_json(json_data, json_schema):
    """Validate json data against a schema."""
    result = False
    try:
        validate(instance=json_data, schema=json_schema)
        result = True
    except Exception as err:
        print(err)
    return result


async def execute_command(shell_command, merge_output=False, filter=None):
    """
    Execute a command in the shell.

    Keyword arguments:
        shell_command -- command to execute

    Returns:
        The output and error output of the command
    """
    serr = subprocess.PIPE
    if merge_output:
        serr = subprocess.STDOUT
    print('Executing "' + shell_command + '"', flush=True)
    p = subprocess.Popen(shell_command, stdout=subprocess.PIPE,
                         stderr=serr, shell=True)
    output = []
    err = []
    if p.stdout is not None:
        for line in iter(p.stdout.readline, b''):
            line = line.decode()
            if filter is None or filter(line):
                line = line.replace('\r', '').replace('\n', '')
                if not "#___" in line:
                    print("O:", line, flush=True)
                output.append(line)
        # print("END OF OUTPUT"+str(output), flush=True)
    if not merge_output and p.stderr is not None:
        for line in iter(p.stderr.readline, b''):
            line = line.decode()
            if filter is None or filter(line):
                line = line.replace('\r', '').replace('\n', '')
                # print("E:", line, flush=True)
                err.append(line)
        # print("END OF ERROR", flush=True)
    # print("Waiting...", flush=True)
    status = p.wait()
    # print("Waited!", flush=True)
    # res = os.popen(shell_command).read()
    return (output, err, status)


def parse_job_file(filename):
    """
    Parse the job file and return the list of jobs.

    Keyword arguments:
        filename -- the name of the file

    Returns:
        A tuple with the error code and the list of parsed jobs
    """
    code = 0
    jobs = None

    json_data = load_json(filename)
    json_schema = load_json("jobs_schema.json")

    if json_data is None:
        print("Couldn't open specified data file", file=sys.stderr)
        code = 10
    elif json_schema is None:
        print("Couldn't open specified schema file", file=sys.stderr)
        code = 11
    elif not validate_json(json_data, json_schema):
        print("Couldn't validate data against the schema", file=sys.stderr)
        code = 12
    else:
        jobs = []
        for i in range(len(json_data)):
            code, job = _parse_job_line(json_data[i], i + 1)
            if code == 0:
                if job is not None:
                    jobs.append(job)
            else:
                jobs = None
                break
    return (code, jobs)


def _parse_job_line(job_line, number):
    """
    Parse a line of the job file and return the parsed job.

    Keyword arguments:
        job_line -- the read line

    Returns:
        A tuple with the error code and the parsed job
    """
    job = None
    code = 0
    cluster = _get_cluster(job_line["cluster"])
    action = _get_action(job_line["action"])
    max_coords = job_line["maxCoords"]
    labels_file = "auto"
    if "labelsFile" in job_line:
        labels_file = job_line["labelsFile"]
    names = job_line["configName"]
    min_key = np.nan
    if "minKey" in job_line:
        min_key = job_line["minKey"]
    max_key = np.nan
    if "maxKey" in job_line:
        max_key = job_line["maxKey"]
    try:
        job = Job(cluster, action, max_coords,
                  labels_file, names, min_key, max_key)
    except ValueError as e:
        print(f"Exception while parsing line {number}:", e,
              file=sys.stderr)
        code = 13
    return (code, job)

def _get_cluster(cluster_str):
    """
    Check if a string is a valid cluster and return its associated enum value.

    Keyword arguments:
        cluster_str -- the value to check

    Returns:
        The enum value of the given cluster or None if invalid cluster
    """
    res = None
    try:
        res = Cluster(cluster_str)
    except ValueError:
        print(cluster_str, "is not a valid cluster!", file=sys.stderr)
    return res


def _get_action(action_str):
    """
    Check if a string is a valid action and return its associated enum value.

    Keyword arguments:
        action_str -- the value to check

    Returns:
        The enum value of the given action or None if invalid action
    """
    res = None
    try:
        res = Action(action_str)
    except ValueError:
        print(action_str, "is not a valid action!", file=sys.stderr)
    return res


def _get_natural(number, with_zero):
    """
    Check if a number is a natural number.

    Keyword arguments:
        number -- the value to check
        with_zero -- whether 0 is a natural number or not (default False)

    Returns:
        The integer number or None if the conversion failed
    """
    result = None
    try:
        result = float(number)
    except ValueError:
        result = None
    else:
        comp = 1
        if with_zero:
            comp = 0
        if result.is_integer() and result >= comp:
            result = int(result)
    return result

class Cluster(Enum):
    """Enumeration of the possible clusters."""

    GALGO = "galgo"
    CIRRUS = "cirrus"

class Action(Enum):
    """Enumeration of the possible actions."""

    OMIT = "omit"
    PAINT = "paint"
    PROCESS = "process"
    SIMULATE = "simulate"


class Job:
    """Class containing job related info and methods."""

    def __init__(self, cluster, action, max_coords,
                 labels_file, names, min_key, max_key):
        """
        Initialize this job instance.

        Keyword arguments:
            action -- the action to perform (must be a valid Action)
            max_coords -- max coordinates (list of integers greater than zero)
            labels_file -- name of the file containing labels or "auto"
            names -- names of the experiments
            min_key -- min key in graph (float)
            max_key -- max key in graph (float)
        Raises:
            ValueError: If any argument is of the wrong type.
        """
        if not isinstance(action, Action):
            raise ValueError("action must be an element of the Action enum")
        self.cluster = cluster
        self.action = action
        self.max_coords = [_get_natural(raw, False) for raw in max_coords]
        if None in self.max_coords:
            raise ValueError("coords must be integers greater than zero")
        self.labels_file = labels_file
        self.names = names
        self.min_key = float(min_key)
        self.max_key = float(max_key)
        self.launch_task = None
        self.paint_task = None

    def __str__(self):
        """Override __str__ method."""
        dict = {}
        dict["action"] = self.action.value
        dict["max_coords"] = self.max_coords
        dict["labels_file"] = self.labels_file
        dict["names"] = self.names
        dict["min_key"] = self.min_key
        dict["max_key"] = self.max_key
        if self.launch_task is not None:
            dict["launch_task"] = self.launch_task
        if self.paint_task is not None:
            dict["paint_task"] = self.paint_task
        return str(dict)

    async def launch(self, simulations_path, script):
        """
        Launch the job with PBS.

        Keyword arguments:
            number -- the value to check
            with_zero -- whether 0 is a natural number or not (default False)

        Returns:
            The error code (0 means no error)
        """
        code = 0
        num_simulations = self.max_coords[0]
        for max_var in self.max_coords[1:]:
            num_simulations *= max_var
        print(f"RUNNING {num_simulations * len(self.names)} simulations")
        # var_names = get_var_names(len(self.max_coords))
        # var_str = ",".join([var_names[i] + "=" + str(self.max_coords[i])
        #                     for i in range(len(self.max_coords))])
        var_str = "maxcoords=" + ":".join([str(v) for v in self.max_coords])
        now = datetime.now()
        date_time = now.strftime("%Y%m%d%H%M%S")
        for configName in self.names:
            if self.cluster == Cluster.GALGO:
                exec_str = f"qsub -J 0-{num_simulations-1} -v " + \
                           f"cluster={self.cluster.value}," + \
                           f"numvars={len(self.max_coords)}," + var_str + "," + \
                           f"minkey={self.min_key}," + \
                           f"maxkey={self.max_key},name={configName}," + \
                           f"action={self.action.value},script=launch," + \
                           f"fileLabels={self.labels_file}," + \
                           f"scriptspath={os.getcwd()}," +\
                           f"timestamp={date_time}," +\
                           f"simulationspath={simulations_path} {script}"
            elif self.cluster == Cluster.CIRRUS:
                exec_str = f"sbatch --parsable --array=0-{num_simulations-1} --export=ALL," + \
                           f"cluster={self.cluster.value}," + \
                           f"numvars={len(self.max_coords)}," + var_str + "," + \
                           f"minkey={self.min_key}," + \
                           f"maxkey={self.max_key},name={configName}," + \
                           f"action={self.action.value},script=launch," + \
                           f"fileLabels={self.labels_file}," + \
                           f"scriptspath={os.getcwd()}," +\
                           f"timestamp={date_time}," +\
                           f"simulationspath={simulations_path} {script}"
            print("input:", exec_str)
            out, err, _ = await execute_command(exec_str)
            if len(err) == 0:
                await asyncio.sleep(0.001)
                self.launch_task = out[0]
                # print("output:", self.launch_task)
                if self.cluster == Cluster.GALGO:
                    paint_str = f"qsub -W depend=afterok:{self.launch_task} -v " + \
                                f"cluster={self.cluster.value}," + \
                                f"numvars={len(self.max_coords)}," + var_str + "," + \
                                f"minkey={self.min_key},maxkey={self.max_key}," + \
                                f"name={configName},action={self.action.value}," + \
                                f"script=paint,fileLabels={self.labels_file}," + \
                                f"scriptspath={os.getcwd()}," + \
                                f"timestamp={date_time}," +\
                                f"simulationspath={simulations_path} {script}"
                elif self.cluster == Cluster.CIRRUS:
                    paint_str = f"sbatch --dependency=afterok:{self.launch_task} --export=ALL," + \
                                f"cluster={self.cluster.value}," + \
                                f"numvars={len(self.max_coords)}," + var_str + "," + \
                                f"minkey={self.min_key},maxkey={self.max_key}," + \
                                f"name={configName},action={self.action.value}," + \
                                f"script=paint,fileLabels={self.labels_file}," + \
                                f"scriptspath={os.getcwd()}," + \
                                f"timestamp={date_time}," + \
                                f"simulationspath={simulations_path} {script}"
                # print("input paint:", paint_str)
                out, err, _ = await execute_command(paint_str)
                if len(err) == 0:
                    self.paint_task = out[0]
                    # print("output paint:", self.paint_task)
                else:
                    print("Failed to execute paint for job", self,
                          file=sys.stderr)
                    print(err)
                    code = 21
            else:
                print("Failed to execute launch for job", self,
                      file=sys.stderr)
                print(err)
                code = 20
        return code
