#!/bin/bash
#source /nfs/gerda5/gerda-sw/bin/swmod-env.sh
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh gerda@dp-v4.0.0
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  bat@0.9.4

SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install

export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH

export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/

export CPATH="`bat-config --prefix`/include:$CPATH"

runs="run0053-run0092"

en=$1
ds=$2
isotope=$3
bing=0.2


kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"

#kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"
lar_mode=""
rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"
batprec="low"


line="${isotope}_${en}"


odir="/nfs/gerda2/users/rizalinko/gamma-fitter/${runs}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi


odir="/nfs/gerda2/users/rizalinko/gamma-fitter/${runs}/${ds}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi


odir="${odir}/${line}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi

odir="${odir}/binning_${bing}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi

#echo "gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec}"
gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec}
