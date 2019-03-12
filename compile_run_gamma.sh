
export CPATH="`bat-config --prefix`/include:$CPATH"

 cd /nfs/gerda2/users/rizalinko/gamma-analysis/

make clean && make 
time source shell_scripts/run_gamma_analysis_sens_test.sh
