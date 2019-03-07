#!/bin/bash
# make clean && cmake . -DCMAKE_BUILD_TYPE=Debug && make
#export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v03.02/gen/

line="K42_1525"
#kl="/nfs/gerda2/users/wiesinger/gerda-metadata/data-sets/phy/run0053-run0089-phy-analysis.txt"
kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"
ds="EnrBEGe"
lar=""
rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"
odir="/nfs/gerda2/users/rizalinko/gamma-fitter/"
batprec="low"
en=1525
bing=0.5

  gdb --args /nfs/gerda2/users/rizalinko/gamma-analysis/gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -l ${lar} -b ${bing} -r ${rescrv}
