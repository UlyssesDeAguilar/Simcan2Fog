import asyncio
import json
import numpy as np
import os
import subprocess
import sys
import math

from datetime import datetime
from enum import Enum
from jsonschema import validate
from config import cluster_commands

"""
module job_utils: this module is in charge of creating the jobs and the commands that will be used tu submit those jobs to the resources manager.

"""


def fileparse(filename):
    """
    Parse the file and return the error code and the parsed list of jobs.

    """
    code, jobs = parse_job_file(filename)
    if code != 0:
        print("Check if you meet the requirements specified in the JSON schema")
    return (code, jobs)


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




class Job:
    async def submit(self, simulations_path,cluster ,script):
        """
        Launch the job with the commands and script configured in config.py.

        """
        code = 0
        num_simulations = math.prod(self.max_coords)






        for config_name in self.config_name:
            exec_dict = cluster_commands(cluster, num_simulations, "launch", None) #Receives the commands from the config.py file
            exec_str=f'{exec_dict["submit_command"]}' #Adds the submit command to the command that will be executed
            for w in  exec_dict.keys():
                if(w!="submit_command"):    #Transforms the reserved characters to others that don't conflict with the resources manager. This change will be reverted in launch.py and plot.py
                    exec_str = exec_str + f'{w}={exec_dict[w]}'.replace(" ","").replace(",","·").replace("'","*")+","  #Receives the commands from exec_dict
            for w in self.__dict__.keys():
                if(w!="config_name"):
                    exec_str = exec_str + f'{w}={self.__dict__[w]}'.replace(" ","").replace(",","·").replace("'","*")+","   #Receives the commands from the JSON file
               
            
            del exec_dict['submit_command']     #Deletes the submit command so we can create a list with only the job variables
            job_vars=list(self.__dict__.keys()) + list(exec_dict)   #Creates a list with the name of all job vars
            job_vars="·".join(job_vars)     #Converts the list into a string so it can be passed through qsub. The separator used was · for compatibility reasons




            exec_str = exec_str +  f"config_name={config_name}," + f'job_vars={job_vars}' +f" {script}" #exec_str contains all commands that will be submitted
            print("submit command:  ",exec_str)
            print('\n')

            


            out, err, _ = await execute_command(exec_str)   #Executes the command
            if len(err) == 0:   #If there's no error, the plot.py script will launch
                await asyncio.sleep(0.001)
                launch_task = out[0]


                exec_dict = cluster_commands(cluster, num_simulations, "plot",launch_task)  
                plot_str=f'{exec_dict["submit_command"]}'
                for w in  exec_dict.keys():
                    if(w!="submit_command"):
                        plot_str = plot_str + f'{w}={exec_dict[w]}'.replace(" ","").replace(",","·").replace("'","*")+","
                for w in self.__dict__.keys():  #Adapts the qsub command as done previously
                    if(w!="config_name"):
                        plot_str = plot_str + f'{w}={self.__dict__[w]}'.replace(" ","").replace(",","·").replace("'","*")+","
                plot_str = plot_str + f"config_name={config_name}," + f'job_vars={job_vars}' +f" {script}"

                out, err, _ = await execute_command(plot_str)  #The process is the same as before but we do not need to submit put job_vars since we did that previously
                if len(err) == 0:
                    self.plot_task = out[0]

                else:
                    print("Failed to execute plot for job", self, file=sys.stderr)
                    print(err)
                    code = 21
            else:
                print("Failed to execute launch for job", self, file=sys.stderr)
                print(err)
                code = 20
        return code

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
    # print('Executing "' + shell_command + '"', flush=True)
    p = subprocess.Popen(shell_command, stdout=subprocess.PIPE,
                         stderr=serr, shell=True)
    output = []
    err = []
    if p.stdout is not None:
        for line in iter(p.stdout.readline, b''):
            line = line.decode()
            if filter is None or filter(line):
                line = line.replace('\r', '').replace('\n', '')
                # print("O:", line, flush=True)
                output.append(line)
        # print("END OF OUTPUT", flush=True)
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
        print("Couldn't open specified file", file=sys.stderr)
        code = 10
    elif json_schema is None:
        print("Couldn't open specified file", file=sys.stderr)
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
    job=Job()
    code = 0
    
    try:
        
        for i in job_line.keys():
            job.__setattr__(i,job_line[i])
        
    except ValueError as e:
        print(f"Exception while parsing line {number}:", e,
              file=sys.stderr)
        code = 13
    return (code, job)


