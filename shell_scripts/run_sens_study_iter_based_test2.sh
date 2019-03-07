#!/bin/bash
#source /nfs/gerda5/gerda-sw/bin/swmod-env.sh


#env -i bash
echo $ds

source /nfs/gerda2/users/rizalinko/bin/swmod-env.sh
source /nfs/gerda2/users/rizalinko/bin/swmod-env-cpath.sh gerda@dp-v5.3.0
export SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install 
export SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
#swmod target -l bat@rizalinko
export ROOTSYS=/nfs/gerda5/gerda-sw/sw/root/linux-centos-7-x86_64/6.04.10
eval swmod load  root@6.04.10
#source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  bat@0.9.4

export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH
export CPATH="`bat-config --prefix`/include:$CPATH"
'''
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh gerda@dp-v4.0.0
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  bat@master
#source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  bat@0.9.4

SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install

export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH

export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/
export CPATH="`bat-config --prefix`/include:$CPATH"
'''
runs="run0053-run0092"
#lar_mode="LArAC"
#psd_mode="PSDAC"


lar_mode=""
psd_mode=""

pseudo_iter=90113
log_file='$(pwd)'
bing=1
batprec="low"
woi=14
inp_pseudo_path="/nfs/gerda2/users/rizalinko/gerda-bkg-model/gerda-bkg-model/input/data/"

kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"
line="${isotope}"

dsets=("EnrBEGe")
#ds=$2
rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"
echo "$rescrv"
#dsets=("EnrBEGe" "EnrCoax" "Natural")
for i in ${dsets[@]};
do
ds=$i
echo $ds
for ((en=600; en<601; en++));
do
odir="/nfs/gerda2/users/rizalinko/gamma-fitter/run0053-run0092/sensitivity/"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi

odir="${odir}/${ds}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi


odir="${odir}/binning_${bing}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi


if [ ! ${lar_mode} == "" ]; then
  odir="${odir}/${lar_mode}"
  if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
  fi
fi

odir="${odir}/${en}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi

echo ${odir}

line="superwimp_${en}_${pseudo_iter}"

#echo "/nfs/gerda2/users/rizalinko/gamma-analysis/gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi} -f ${inp_pseudo_path} -i ${pseudo_iter} -t > log_${log_file}.txt"
/nfs/gerda2/users/rizalinko/gamma-analysis/gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi} -f ${inp_pseudo_path} -i ${pseudo_iter} -t > log_${log_file}.txt

done
done


SWMOD_MODPATH=/nfs/gerda5/gerda-sw/sw
