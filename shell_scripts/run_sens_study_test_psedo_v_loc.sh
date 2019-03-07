#!/bin/bash
#source /nfs/gerda5/gerda-sw/bin/swmod-env.sh
#env -i bash

source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh gerda@dp-v4.0.0
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  bat@master
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  clhep@2.1.3.1
export LD_LIBRARY_PATH=/nfs/gerda5/gerda-sw/sw/clhep/linux-centos-7-x86_64/2.1.3.1/lib/:$LD_LIBRARY_PATH
SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install


export PATH=/nfs/gerda5/gerda-sw/sw/clhep/linux-centos-7-x86_64/2.1.3.1/bin/:$PATH
export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH

export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/

export CPATH="`bat-config --prefix`/include:$CPATH"
runs="run0053-run0092"

#lar_mode="LArAC"
#psd_mode="PSDAC"

#printenv
lar_mode=""
psd_mode=""

pseudo_iter=${inp_it}
ds="enrCoax"

#isotope=${inp_is}
log_file="test_pseudo"
bing=1
batprec="low"
woi=14
pseudo_path="/nfs/gerda2/users/rizalinko/gerda-bkg-model/gerda-bkg-model/input/data/"

kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"

#kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"

rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"


line="${isotope}"
en=300
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

pseudo_iter=22080
line="superwimp_${en}_${pseudo_iter}"

gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi} -f ${pseudo_path} -i ${pseudo_iter} -t
#break



