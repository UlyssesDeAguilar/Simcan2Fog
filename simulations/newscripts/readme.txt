
-----------This readme file will list all the things you need to know before using this scripts.--------------

1. "action", "maxCoords", "configName", "cluster", "simulations_path" MUST have a value, don´t remove any of those variables from the JSON schema or else the scripts won't work.

2. You can add variables to the schema and change the values of the existing ones (except action) and the scripts will dynamically adapt. The current supported types are: numbers (integer and float), strings, lists, dictionaries and lists of lists 

3.  If you want to change the cluster used, make sure to add the new script to the dictionary in config.py. It is also compulsory to add in this new script a variable that represents the current simulation with the exact name "current_sim". Here is an example:
current_sim=${PBS_ARRAY_INDEX}

4. The script that will be used depending the cluster should define a log folder, although it's optional. To receive the variables and process them you can use job_vars, which lists all variables of the job. 
To do so, create a loop that reads each element of job_vars and store those values in an array. Then, use a loop to create a string that contains all job variables and its values in the form "key=value". 
This string will be passed and proccessed by launch.py and plot.py. Here is an example in bash:

IFS='·' read -r -a job_variables <<< "$job_vars" #Reads the job_vars list and creates an array. 

for name in ${job_variables[@]}    
do
 var=$var"$name"="${!name} "        #Creates a string that contains all job variables
done
var=$var"current_sim=$current_sim"  #Adds the number of the current simulation to the string



 
5. !!! IMPORTANT !!! If you want to write a string that contains numbers and letters, please surrond it with simple quotes ' or else the code won't recognise it as a string. Also, don't use middle dots · or asterisks *. Here is an example:
labels:  "[['40000MIPS', '50000MIPS', '60000MIPS', '70000MIPS'],[128, 256, 512, 1024]]"


6. If an error occurs, you can check the log in the folder whose path is defined by the script of the cluster