#!/bin/bash
#source /nfs/gerda5/gerda-sw/bin/swmod-env.sh
env -i bash
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh gerda@dp-v4.0.0
source /nfs/gerda5/gerda-sw/bin/swmod-env-cpath.sh  bat@0.9.4

SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install

export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH

export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/

export CPATH="`bat-config --prefix`/include:$CPATH"

runs="run0056"
#lar_mode="LArAC"
#psd_mode="PSDAC"


lar_mode=""
psd_mode=""

en=${inp_en}
ds=${inp_ds}

#isotope=${inp_is}
log_file=${inp_log}
bing=${inp_bin}
batprec=${inp_bat}
woi=${inp_woi}
pseudo_data_path=${inp_pseudo_path}
  

kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"

#kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"

rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"







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
for ((pseudo_iter=100; pseudo_iter<10000000; pseudo_iter++));
do
 
line="${pseudo_iter}_superwimp_${en}"
#echo "gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -l ${lar_mode} -p ${psd_mode} -w ${woi}> log_${log_file}.txt"
gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi} -f ${inp_pseudo_path} -i ${pseudo_iter} -t > log_${log_file}_${pseudo_iter}.txt    
done



