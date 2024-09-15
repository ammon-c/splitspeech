@echo off
rem ##########################################################
rem # Windows batch script to run the unit tests for certain #
rem # functions of the audio processing test/demo program.   #
rem ##########################################################

rem # Discard output files from prior test run.

if exist unittest.out del unittest.out
if exist temp.wav del temp.wav

rem # Check that the program exists.  If both release and debug
rem # versions were built, the debug version takes precedence.

set EXENAME=
if exist x64\Release\unittest.exe set EXENAME=x64\Release\unittest.exe
if exist x64\Debug\unittest.exe   set EXENAME=x64\Debug\unittest.exe
if not "%EXENAME%"=="" goto runtests

echo -------------------------------------------------------
echo The unittest.exe program file seems to be missing!
echo Did you build it before running this script?
echo -------------------------------------------------------
exit /b 1


:runtests

echo ---------------------------------------------------
echo Test program found at %EXENAME%.
echo Running tests.  One moment please.
echo ---------------------------------------------------
%EXENAME% testdata\airhost.wav                           >> unittest.out
if errorlevel 1 goto :testfail
%EXENAME% testdata\company.wav                           >> unittest.out
if errorlevel 1 goto :testfail
%EXENAME% testdata\counting.wav                          >> unittest.out
if errorlevel 1 goto :testfail
%EXENAME% testdata\lateforwork.wav                       >> unittest.out
if errorlevel 1 goto :testfail
%EXENAME% testdata\two.wav                               >> unittest.out
if errorlevel 1 goto :testfail
%EXENAME% testdata\score_32bitfloat.wav                  >> unittest.out
if errorlevel 1 goto :testfail

type unittest.out
echo Test output may be reviewed in file 'unittest.out'
echo ---------------------------------------------------


:end
set EXENAME=
exit /b 0


:testfail
set EXENAME=
type unittest.out
echo ---
echo Unit test reported error(s)!
echo Test output may be reviewed in file 'unittest.out'
echo ---------------------------------------------------
exit /b 1

