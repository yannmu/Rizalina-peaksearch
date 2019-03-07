import os
import sys
import time
from argparse import ArgumentParser
from time import sleep
import numpy as np
from random import randint
parser = ArgumentParser()
requiredNamed = parser.add_argument_group("required named arguments")

#ToDO: add few more parameters to use for the shell script selection
parser.add_argument ("-s", help="scripts to run", dest = "script",  default="")
parser.add_argument ("-d", help="dir of shell scripts", dest = "scripts_dir",  default="../shell_scripts/")
args = parser.parse_args()
global script
script = args.script
script_dir = args.scripts_dir

bat_precision = "low"
#datasets=["EnrBEGe", "EnrCoax", "Natural"]
datasets=["EnrBEGe"]
a = 600
b = 1001

def create_job_name(script):
  if script =="":
    script = sys.argv[0]
    script = script.replace('submit', 'run')
    script = script.replace('.py', '.sh')
    script = "".join([script_dir, script])
  idx2 = script.index('.sh')
  idx1 = script.index('run_')
  return script, script[idx1+len('run_'): idx2]

ibinning = 1
scr, job_name = create_job_name(script)
for ids in datasets:
  for it in range(3):
    log_time =  str(time.time())
    command = "qsub -q gerda -V -d /nfs/gerda2/users/rizalinko/gamma-fitter/logs3/  -e localhost:erlog_{}.txt   -o localhost:log_{}.txt -N {}_{} ".format(log_time, log_time, job_name, it)
    inp_args=""
    
    en = 500
    while en<5500:
    	script=scr[:-3]+str(en)+'.sh'
    	inp_args = " -v inp_ds='{}',inp_bin='{}',inp_bat='{}',inp_log='{}',inp_it='{}' {}".format(ids, str(ibinning), bat_precision, log_time, it, script)

    
    	launch_command =  "".join([command, inp_args])
    	print(launch_command)
    	os.system(launch_command)
    	sleep(10)
    	en+=500
#sleep(5000)


'''
 inp_ds='EnrBEGe'
 inp_bin='1'
 inp_bat='1'
 inp_bat='low'
 inp_log='0'
 inp_woi='14'
 inp_is='limit_640'
-bash-4.2$
'''
