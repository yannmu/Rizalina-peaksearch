#!/bin/bash
export PATH+=:$HOME/bin
export SWMOD_HOSTSPEC=linux-centos-7-x86_64
export SWMOD_MODPATH=/users/m/mingazheva/GerdaSoftware/install    
export SWMOD_INST_BASE=/users/m/mingazheva/GerdaSoftware/install
. swmod.sh init
# gerda@dp-v4.0.0

export ROOTSYS=/users/m/mingazheva/GerdaSoftware/install/root
#/linux-centos-7-x86_64/6.04.10/
swmod load root@6.04.10
swmod load  bat@0.9.4_1
swmod load clhep@2.1.3.1  
#SWMOD_INST_BASE=/nfs/gerda2/users/rizalinko/GerdaSoftware/install
#SWMOD_MODPATH=/nfs/gerda2/users/rizalinko/GerdaSoftware/install

export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:$PATH
export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
#export GA_BASE_DIR=/nfs/gerda5/gerda-data/blind/v04.00/gen/
#export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/
export MU_CAL=/nfs/gerda5/gerda-data/blind/v04.00/meta/config/_aux/geruncfg/
export CPATH="`bat-config --prefix`/include:$CPATH"
#export LD_LIBRARY_PATH=/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib/
runs="run0053-run0092"
#lar_mode="LArAC"
#psd_mode="PSDAC"
export LD_LIBRARY_PATH=/users/m/mingazheva/GerdaSoftware/install/clhep/linux-centos-7-x86_64/2.1.3.1/lib:/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/lib:/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib:/usr/lib64/mpich/lib:/usr/local/root/lib
export PATH=/nfs/gerda2/users/rizalinko/gamma-analysis/:/nfs/gerda2/users/mingazheva/local/bin:/nfs/gerda5/gerda-sw/bin:/nfs/gerda2/users/rizalinko/gamma-analysis/:/nfs/gerda2/users/mingazheva/local/bin:/nfs/gerda5/gerda-sw/bin:/nfs/gerda2/users/rizalinko/gamma-analysis/:/users/m/mingazheva/GerdaSoftware/install/clhep/linux-centos-7-x86_64/2.1.3.1/bin:/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/bin:/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/bin:/nfs/gerda2/users/mingazheva/local/bin:/nfs/gerda5/gerda-sw/bin:/usr/lib64/qt-3.3/bin:/usr/lib64/mpich/bin:/usr/local/root/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin::/software/bin:.
export MANPATH=/users/m/mingazheva/GerdaSoftware/install/clhep/linux-centos-7-x86_64/2.1.3.1/share/man:/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/share/man:/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/man/man1:/usr/share/man/mpich:/usr/share/man:/usr/local/share/man

export SWMOD_LIBRARY_PATH=/users/m/mingazheva/GerdaSoftware/install/clhep/linux-centos-7-x86_64/2.1.3.1/lib:/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/lib:/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib:/usr/lib64/mpich/lib
CPATH=/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/include:/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/include:
LIBPATH=/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib

DYLD_LIBRARY_PATH=/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib
PYTHONPATH=/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib
SHLIB_PATH=/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10/lib
SWMOD_LOADED_PREFIXES=/users/m/mingazheva/GerdaSoftware/install/clhep/linux-centos-7-x86_64/2.1.3.1:/users/m/mingazheva/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1:/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10:
CMAKE_PREFIX_PATH=/users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10
which root
#printenv
lar_mode=""
psd_mode=""

pseudo_iter=${inp_it}
ds="enrBEGe"

#isotope=${inp_is}
log_file="test_pseudo"
bing=1
batprec="med"
woi=14
pseudo_path="/nfs/gerda2/users/rizalinko/gerda-bkg-model/gerda-bkg-model/input/data/"

kl="/nfs/gerda5/gerda-data/blind/v04.00/meta/data-sets/phy/${runs}-phy-analysis.txt"

#kl="/nfs/gerda2/users/rizalinko/keylists/run86_87.txt"

rescrv="/nfs/gerda2/Calibration_PhaseII/SuperCalibrations/mithists/PhII/ZAC/PhIIZACgammalines-supercal.root"


line="${isotope}"
en=620

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

gammacounts -g ${line} -e ${en} -k ${kl} -d ${ds} -b ${bing} -r ${rescrv} -o ${odir} -j ${batprec} -w ${woi}  > log_${log_file}.txt > out.txt
#break



