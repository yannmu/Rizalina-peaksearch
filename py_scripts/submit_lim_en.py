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

parser.add_argument ("-s", help="scripts to run", dest = "script",  default="")
parser.add_argument ("-d", help="dir of shell scripts", dest = "scripts_dir",  default="../shell_scripts/")
args = parser.parse_args()

script = args.script

script_dir = args.scripts_dir


def create_job_name(script):
    if script =="":
        script = sys.argv[0]
        script = script.replace('submit', 'run')
        script = script.replace('.py', '.sh')
        script = "".join([script_dir, script])
        idx2 = script.index('.sh')
        idx1 = script.index('run_')
    return script, script[idx1+len('run_'): idx2]




##############################################################################################
#modifying shell scripts for our needs

def get_new_script(en, ds):
    scr, job = create_job_name(script)
    print(scr)
    f = open(scr,"r")
    lines = f.readlines()
    f.close()
    new_script = "../shell_scripts/limit_{}keV_{}.sh".format(en, ds)
    f = open(new_script,"w+")
    old_log = 'olog'
    new_log = '/disk/data1/atp/rizalina/superwimp/log//log_{}{}.txt'.format(en,ds)
    for line in lines:
        if line.count('ds=') and line.count('#')==0:
            line = 'ds=\"{}\"'.format(ds)
            print(line)

        elif line.count("gammacounts"):
            line = line.replace("gammacounts", "peaksearch")
            print(line)
        elif line.count("en=${inp_en}"):
            line = "en={}\n".format(en)
        elif line.count('gammas='):
            line = 'gammas=${bin_dir}/g_lines_strong.txt'
        elif line.count(old_log):
            line=line.replace(old_log, new_log)
        f.write(line)
    f.close()
    return new_script
##############################################################################################
ds = ["enrBEGe", "enrCoax"]

def submit_jobs(en):
    for d in ds:
        script = get_new_script(en, d)
        command = "sbatch -p standard,atp -o /dev/null -e /dev/null {}".format(script)
        print(command)
        #os.system(command)

for en in range(600, 610):
        submit_jobs(en)
        if en % 100 == 0:
            sleep(60)
