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
#lar_mode="LArAC"
#psd_mode="PSDAC"


lar_mode=""
psd_mode=""

#en=${inp_en}
ds="$1"

#isotope=${inp_is}
log_file="$2"
bing=1
batprec="low"
woi="${3}"
  
pseudo_iter="${4}"
ds="enrBEGe"

#isotope=${inp_is}
log_file="test_pseudo"
pseudo_path="/nfs/gerda2/users/rizalinko/gerda-bkg-model/gerda-bkg-model/input/data/"

kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"

#kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"

rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"


line="${isotope}"


odir="/nfs/gerda2/users/rizalinko/gamma-fitter/run0053-run0092/klines_sens/"
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

odir="${odir}/woi_${woi}"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi

echo ${odir}

for ((en=1500; en<1550; en++));
do
	line="superwimp_${en}"

  #echo "gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -l ${lar_mode} -p ${psd_mode} -w ${woi}> log_${log_file}.txt"
  gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -l ${lar_mode} -p ${psd_mode} -w ${woi} -f ${pseudo_path} -i ${pseudo_iter} -t
  #sleep 10    
done



