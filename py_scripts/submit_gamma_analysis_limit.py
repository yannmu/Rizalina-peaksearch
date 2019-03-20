import os
import sys
import time
from argparse import ArgumentParser
from time import sleep
import numpy as np
from random import randint
import os.path


parser = ArgumentParser()
requiredNamed = parser.add_argument_group("required named arguments")

#ToDO: add few more parameters to use for the shell script selection
parser.add_argument ("-s", help="scripts to run", dest = "script",  default="")
parser.add_argument ("-d", help="dir of shell scripts", dest = "scripts_dir",  default="../shell_scripts/")
args = parser.parse_args()
global script
script = args.script
script_dir = args.scripts_dir

bat_precision = "med"
datasets=["enrBEGe", "enrCoax", "Natural"]

 
##############################################################################################
log_dir = "/nfs/gerda2/users/rizalinko/gamma-fitter/logs_gammas/"
ibinning = 0.3
woi = 20
script = '{}/run_gamma_qsub.sh'.format(script_dir)
log_time = str(time.time())
larmodes = ["bare", "LArAC", "LArC"]
for lmode in larmodes:
	for ds in datasets:
		command = "qsub -q gerda -V -d {} -e localhost:erlog_{}.txt   -o localhost:log_{}_{}.txt -N {}_{}_{} ".format\
			(log_dir, log_time, log_time, 'limit', 'gamma', lmode, ds)
		inp_args = " -v inp_bin='{}',inp_log='{}',inp_woi='{}',inp_lar='{}',inp_ds='{}' {}".format\
			(str(ibinning),log_time, woi, lmode, ds, script)



		launch_command =  "".join([command, inp_args])
		print launch_command
		os.system(launch_command)



'''
 inp_ds='EnrBEGe'
 inp_bin='1'
 inp_bat='1'
 inp_bat='low'
 inp_log='0'
 inp_woi='114'
-bash-4.2$
'''




