source /nfs/gerda5/gerda-sw/bin/swmod-env.sh
# gerda@dp-v4.0.0
export SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/gerda_sw/install
export SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/gerda_sw/install



export ROOTSYS=/users/m/mingazheva/GerdaSoftware/install/root
swmod load root@6.04.10
swmod load  bat@0.9.4_2
swmod load clhep@2.1.3.1
#SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
#SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install

export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH

#export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
#export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/

export CPATH="`bat-config --prefix`/include:$CPATH"

 cd /nfs/gerda2/users/rizalinko/gamma-analysis/

make clean && make 
