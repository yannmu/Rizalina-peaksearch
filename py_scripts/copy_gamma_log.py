from argparse import ArgumentParser
from IPython.utils.path import ensure_dir_exists
import os
import shutil

parser = ArgumentParser()
requiredNamed = parser.add_argument_group("required named arguments")

#ToDO: add few more parameters to use for the shell script selection
parser.add_argument ("-d1", help="base_dir_from_where_to_copy", dest = "dir1",  default="")
parser.add_argument ("-d2", help="base_dir_to where_to_copy", dest = "dir2",  default="")

args = parser.parse_args()
base_dir = args.dir1
dir2 = args.dir2


copied_files =[]
for subdir, dirs, files in os.walk(dir2):
   copied_files.append(subdir.split("/")[-1])
#print copied_files

for subdir, dirs, files in os.walk(base_dir):
	for idir in dirs:
		if  idir in copied_files:
			continue
		else:				
			dir_to_create = os.path.join(dir2, idir)
			ensure_dir_exists(dir_to_create, int("775", 8))
			file_to_create = "".join([dir_to_create, "/gamma.log"])
			
			target_file = os.path.join(base_dir, idir, "gamma.log")
			print ('copying ', target_file)
			
			
			shutil.copy2(target_file, file_to_create)


		
