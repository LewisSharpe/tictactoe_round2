#!/bin/bash
#script to find last modified and last accessed time of a file
#SBATCH --job-name=timestamp
#SBATCH --output=tstamp.txt
#
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=10:00

E_NOARG=65
E_MOREARG=66
if [  $# = 0 ]
then
        echo -e '\E[31m' "Usage: $0 filename"
        tput sgr0
        exit $E_NOARG
elif [ $# -gt 1 ]
then
        echo -e '\E[31m' "Please give only one file at a time"
        tput sgr0
        exit $E_MOREARG
fi
LS=`which ls`
MOD_TIME=`$LS -lart $1|cut -f6,7,8 -d" "`
ACC_TIME=`$LS -laru $1|cut -f6,7,8 -d" "`
echo -e "timestamp for tictactoe\n"
echo "to get latest timestamp,"
echo "compile and execute the 'ttt_pt_ver5.c',"
echo -e "then revisited this txt file for the updated timestamp.\n"
echo "The modified time of the file is $MOD_TIME by lsharpe"
echo "The accessed time of the file is $ACC_TIME by lsharpe"

command >> tstamp.txt
