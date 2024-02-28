# Simcan2Fog
<div align="center">

  <img src="docs/Simcan2Fog.png" alt="Logo" width="400"/>

  ![Status](https://img.shields.io/badge/status-build_passing-green.svg)
  ![Documentation](https://img.shields.io/badge/docs-on_progress-yellow.svg)
  ![GitHub License](https://img.shields.io/github/license/PabloCCanizares/Simcan2Cloud)
  ![GitHub repo size](https://img.shields.io/github/repo-size/PabloCCanizares/Simcan2Cloud)

</div>

## Overview
The objective of this project is to build a customizable simulator for cloud, edge and fog environements. It is based of the Simcan2Cloud Simulator.

## Table of contents

* [Installation](#installation)
* [Quick Start](#quickstart)
* [Design][design]
* [Simulating][execution]
* [Testing][testing]

[design]: docs/design.md
[execution]: docs/execution.md
[testing]: docs/testing.md

## Installation
A future objective is the creation a Docker image for this tool, both for normal users and developers. 
Right now it's not quite the moment to wrap everything up, but it's atleast feasible in a not so distant future.

### Dependencies
As of today, Simcan2Fog relies on the following frameworks:
* **OMNeT++ 5.6.2** : The development framework and simulation kernel.
* **INET4 4.5.2**: The OMNeT++ networking framework.
* **MySQL 8.2.0**: The DB engine used for storing experiment models and experiments.
* **MySQLConnector/C++ 7.5**: The C++ connector for the database.

### Installing the project
#### Installing MySQLConnector/C++
This dependency is not a strict one, but it is really recommended because it enables the configuration via the configuration engine ```SimSchemaMySQL```.
It's also worth to be noted that **this is the only engine currently implemented**. This doesn't mean that it's impossible to support other
data sources, they just have to implement the ```SimSchema``` interface and Simcan2Fog will be able to operate with them.  

For Ubuntu distributions, as of the current version the 22.04 LTS, one just needs to install the following package:
```
sudo apt update
sudo apt install libmysqlcppconn-dev
```
This package currently installs the C++ Connector version 7.5, compatible with the development MySQL database version.

It is also strongly recommended to install the MySQL client:
```
sudo apt install mysql-client
```
Further on we'll discuss how to load the database for experimenting.

#### Installing OMNeT
1. Head to https://omnetpp.org/download/old.html and download OMNeT++ 5.6.2 with the IDE (not the core version).
2. Decompress the file in your user directory
3. Follow the steps included in ```doc/InstallGuide.pdf``` in order to complete the installation.
> Also check out the InstallGuide of the later version (available at: https://doc.omnetpp.org/omnetpp/InstallGuide.pdf) for newer development environements (like Ubuntu 20.04 LTS or 22.04 LTS)

After the installation is complete you can run the examples provided by OMNeT++ (watch out for those who need extra dependencies).
#### Installing INET4
The easiest way to install this framework is from the IDE wizard:
1. Head to `Help > Install Simulation Models`
2. Click on `INET Framework 4`, let the default path and click on `Install Proyect`

After the installation is complete you can run the examples provided by INET to verify everything working correctly.
#### Installing Simcan2Fog
1. Clone the repo inside the omnet-5.6.2 folder
2. Open the OMNeT++ IDE and select **build proyect**

Technically it should build without any trouble from this point on. If the build succeded, congratulations! You can now move forward to [running experiments](./docs/usage.md).

## Quick Start
When the simulator is ready (or at least mostly ready) we will list here some scenarios that you'll be able to run once you finished installing the software.

[//]: # (## Special thanks to
Flaticon
<a href="https://www.flaticon.com/free-icons/data-center" title="data center icons">Data center icons created by smashingstocks - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/dns" title="dns icons">Dns icons created by Freepik - Flaticon</a>
<--->)