#!/bin/bash
export PATH+=:$HOME/bin
export SWMOD_HOSTSPEC=linux-centos-7-x86_64
source /nfs/gerda5/gerda-sw/bin/swmod-env.sh
# gerda@dp-v4.0.0
export SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/gerda_sw/install
export SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/gerda_sw/install

. swmod.sh init
# gerda@dp-v4.0.0

export ROOTSYS=/nfs/gerda2/users/rizalinko/gerda_sw/install/root
#/linux-centos-7-x86_64/6.04.10/
swmod load root@6.04.10
swmod load  bat@0.9.4_2
swmod load clhep@2.1.3.1
export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH

export CPATH="`bat-config --prefix`/include:$CPATH"
runs="run0053-run0092"
lar_mode=""
psd_mode=""

pseudo_iter=${inp_it}
ds="enrBEGe"

log_file="test_pseudo"
bing=1
batprec="med"

woi=14
pseudo_path="/nfs/gerda2/users/rizalinko/gerda-bkg-model/gerda-bkg-model/input/data/"

kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"

#kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"

rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"


line="${isotope}"
en=1525

odir="/nfs/gerda2/users/rizalinko/gamma-fitter/test_lim/"
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
pseudo_iter=22080
line="superwimp_${en}_"
#gdb --args /nfs/gerda2/users/rizalinko/gamma-analysis/gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -l ${lar} -b ${bing} -r ${rescrv}

gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi}
#break



