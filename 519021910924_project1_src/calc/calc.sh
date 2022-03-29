#! /bin/bash
cd /home/wf/linux_files/linux_exp/CS353-Linux-Kernel/linux_exp/CS353-2022-Spring/project1/
make calc
sudo dmesg -C
sudo insmod calc.ko operand1=2 operand2=1,2,3,4 operator=add
dmesg
cat /proc/519021910924/calc
echo 3 > /proc/519021910924/calc
cat /proc/519021910924/calc
sudo rmmod calc
#cat /proc/519021910924/2