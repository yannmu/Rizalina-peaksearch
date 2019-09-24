#!/bin/bash

export PATH=/disk/data1/atp/rizalina/sw/gamma-analysis:/home/atp/rhiller/swmod/bin:$PATH
export SWMOD_MODPATH=/disk/data1/atp/rizalina/sw/install


export PATH=/disk/data1/atp/rizalina/sw/gamma-analysis:/home/atp/rhiller/swmod/bin:$PATH
export PATH=/disk/data1/atp/rizalina/sw/gamma-analysis:/home/atp/rhiller/swmod/bin:$PATH


export CPATH="`bat-config --prefix`/include:$CPATH"
runs="run0053-run0092"
#psd_mode="PSDAC"
#printenv

source /disk/data1/atp/rizalina/sw/swmod/bin/swmod.sh load bat@0.9.4_1
source /app/cern/root_v6.14.00/bin/thisroot.sh
lar_mode=""
psd_mode=""
bin_dir="/disk/data1/atp/rizalina/superwimp_sw/"
gammas=${bin_dir}/gammalines.txt
line="${isotope}"
rescrv="/disk/data1/atp/rizalina/superwimp/input/PhIIZACgammalines-supercal.root"

bing=1
batprec="low"
en=600
woi=12
sfile="/disk/data1/atp/rizalina/superwimp/input/PhaseII53-92_blind.root"


#dsets=("EnrBEGe" "EnrCoax" "Natural")
dsets=("enrBEGe")
#dsets=("enrCoax")
#for i in ${dsets[@]};
#do
ds="enrBEGe"
odir="/disk/data1/atp/rizalina/superwimp/test_superwimp_sw"
if [ ! -d ${odir} ]; then
    mkdir -m g=wr ${odir}
fi

odir=${odir}/${ds}

if [ ! -d ${odir} ]; then
	    mkdir -m g=wr ${odir}
fi

odir=${odir}/${en}


    if [ ! -d ${odir} ]; then
	        mkdir -m g=wr ${odir}
	fi




line="superwimp_${en}"

${bin_dir}/peaksearch -g ${line} -e ${en}  -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi} -n ${gammas} -s ${sfile} 

#done


