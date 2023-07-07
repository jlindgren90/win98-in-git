@echo off
cd \windows
if exist system.dat goto err
if exist user.dat goto err
smartdrv
regedit /c all.reg
goto out
:err
echo system.dat or user.dat exists, not overwriting
:out
cd \
