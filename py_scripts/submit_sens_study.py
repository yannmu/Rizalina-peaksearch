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
a = 640
b = 700
f = 1
done = []


def create_job_name(script):
  if script =="":
    script = sys.argv[0]
    script = script.replace('submit', 'run')
    script = script.replace('.py', '.sh')
    script = "".join([script_dir, script])
  idx2 = script.index('.sh')
  idx1 = script.index('run_')
  #print idx2
  #print script[:idx2]
  #idx1 = script.rfind('/', idx_stop_search, idx2)
  return script, script[idx1+len('run_'): idx2]

ibinning = 1
woi = 14
pseudo_path="/nfs/gerda2/users/rizalinko/gerda-bkg-model/input/data/"
script, job = create_job_name(script)
for ids in datasets:
  for en in range(600, 1001):
    log_time =  str(time.time())
    command = "qsub -q gerda -V -d /nfs/gerda2/users/rizalinko/gamma-fitter/logs2/  -e localhost:erlog_{}.txt   -o localhost:log_{}.txt -N {}_{} ".format(log_time, log_time, job, en)
    inp_args=""
    
    inp_args = " -v inp_ds='{}',inp_bin='{}',inp_bat='{}',inp_log='{}',inp_en='{}',inp_pseudo_path='{}',inp_woi='{}' {}".format(ids, str(ibinning), bat_precision, log_time, en, pseudo_path, woi, script)

    
    launch_command =  "".join([command, inp_args])
    print launch_command
    os.system(launch_command)
    #sleep(30)

	
    sleep(5)
sleep(5000)



'''
 inp_ds='EnrBEGe'
 inp_bin='1'
 inp_bat='1'
 inp_bat='low'
 inp_log='0'
 inp_woi='114'
-bash-4.2$
'''

