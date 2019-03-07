from shutil import copyfile
import os
import os.path
src_dir = "/nfs/gerda2/users/rizalinko/gamma-fitter/run0053-run0092/phy_limit/EnrBEGe/binning_1/"
out_dir = "/nfs/gerda2/users/rizalinko/gamma-fitter/run0053-run0092/sensitivity/EnrBEGe/binning_1/"
gamma_lines = [609.3, 934.0, 1120.3, 1238.0, 1378.0, 1764.5, 2204.2, 295.2, 351.9, 239.0, 1001.0, 1173.2, 1332.3, 911.2, 968.7, 583.2, 861.0, 2614.5, 1524.7, 1460.8]
for g in gamma_lines:
    for en in range(int(g)-20, int(g)+20):
	if en==592:
		continue
        src_file = '{}/{}/gamma.log'.format(src_dir, en)
        dst_file = '{}/{}/gamma.log'.format(out_dir, en)
        dst_file_copy = '{}/{}/gamma_copy.log'.format(out_dir, en)
	if os.path.exists(dst_file):
	        copyfile(dst_file, dst_file_copy)
		#pass
		print("file {} copied".format(dst_file))
        	os.remove(dst_file)
		print("file {} depeted".format(dst_file))
	if os.path.isfile(src_file):
		if not os.path.exists('{}/{}'.format(out_dir, en)):
			os.mkdir('{}/{}'.format(out_dir, en))
	        copyfile(src_file, dst_file)
		print("file {} copied".format(src_file))


