@echo off
set EXE=x64\Release\TestFPE.EXE
set NB=2000000
%EXE% -N %NB%
echo =========
%EXE% -throwfpe -N %NB%
echo =========
