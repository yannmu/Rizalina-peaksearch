base_dir="/nfs/gerda2/users/rizalinko/gamma-fitter/run0053-run0092/"
o_dir=${base_dir}/sensitivity_improved_fit*


cd ${base_dir}

sum_dir=${base_dir}/allSens/
rm -r ${sum_dir}

mkdir ${sum_dir}
mkdir ${sum_dir}/bege
mkdir ${sum_dir}/coax

for ((en=200; en<5400; en++));do
bege_out=${sum_dir}/bege/${en}
coax_out=${sum_dir}/coax/${en}
mkdir ${bege_out}
mkdir ${coax_out}


cat ${o_dir}/enrBEGe/binning_1/${en}/gamma.log > ${bege_out}/gamma.log
cat ${o_dir}/enrCoax/binning_1/${en}/gamma.log > ${coax_out}/gamma.log
done
