#!/usr/bin/env python3.8
# *_* coding: utf-8 *_*

"""
module submit - Submits an execution to pbs queue.

This module submits jobs specified in a file to the PBS queue in GALGO or the specified cluster and contains different parameters that can be configured according to the current experiment
"""


import asyncio
import os
import os.path
import subprocess
import sys

from job_utils import parse_job_file
from config import cluster_script


def main(argc, argv):
    """
    Check whether arguments are valid or not before calling launcher.

    Keyword arguments:
        argc -- the number of arguments, must be 2
        argv -- the list of arguments: the job file and the simulations folder
    """
    

    if (argc != 1):
        script = os.path.split(sys.argv[0])[1]
        print("Syntax: " + script + " <job file>",
              file=sys.stderr)
        sys.exit(1)
    job_file = argv[0]
    if not os.path.isfile(job_file):
        print("Job file doesn't exist!", file=sys.stderr)
        sys.exit(2)
    code = asyncio.run(launcher(job_file))
    sys.exit(code)


async def launcher(file_name):
    """
    Call methods when needed to launch the simulations.

    Keyword arguments:
        file_name -- the name of the file to be parsed
        simulations_path -- the name of the folder containing the omnetpp.ini

    Returns:
        The error code (0 means no error)
    """
    code, jobs = fileparser(file_name)

    if code == 0:

        # simulations_path = "${HOME}/SIMCAN-2.0/simulations"
        tasks = set()
        for job in jobs:
            if not os.path.isdir(job.simulations_path):
                print("Simulations folder doesn't exist!", file=sys.stderr)
                sys.exit(3)
            if job.action == "omit":
                print("omited:", job)
            else:
                print('\n')
                print("running:", job.__dict__)
                print('\n')
                coroutine = job.submit(job.simulations_path, job.cluster,cluster_script(job.cluster) )
                task = asyncio.create_task(coroutine)
                tasks.add(task)
        done, pending = await asyncio.wait(tasks)
        for task in done:
            code = task.result()
            if code != 0:
                print(f"Task failed with code {code}", file=sys.stderr)
                break

    return code


def fileparser(filename):
    """
    Parse the file and return the error code and the parsed list of jobs.

    Keyword arguments:
        filename -- the name of the file to be parsed

    Returns:
        The error code (0 means no error) and the list of jobs
    """
    code, jobs = parse_job_file(filename)
    if code != 0:
        print("Check the JSON file for syntax errors")
    return (code, jobs)


if __name__ == "__main__":
    main(len(sys.argv) - 1, sys.argv[1:])



