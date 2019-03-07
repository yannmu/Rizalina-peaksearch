import os
import time
from time import sleep

woi = 4
for en in range(27, 37):

	log_time =  str(time.time())
	command = "qsub -q gerda -V -d /nfs/gerda2/users/rizalinko/gamma-fitter/logs_lowen/  -e localhost:erlog_{}.txt   -o localhost:log_{}.txt -N energy_{}_keV".format(log_time, log_time, en)
	inp_args=""
	
	inp_args = " -v inp_en='{}',inp_woi='{}' ../shell_scripts/run_low_en_test.sh".format(en, woi)

	cmd =  "".join([command, inp_args])
	print cmd
	os.system(cmd)
	
	if en< 160:
		en+=1
	else:
		en+=20
		woi = 14

	print('********************************************************************')
	print(cmd)
	print('********************************************************************')
	sleep(10)
