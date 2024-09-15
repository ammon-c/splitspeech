@echo off
rem ##########################################################
rem # Windows batch script to run several .WAV files through #
rem # the splitspeech program, for testing.                  #
rem ##########################################################

rem # Discard output files from prior test run.

if exist test.out del test.out
if exist *_seg*.wav del *_seg*.wav

rem # Check that the program exists.  If both release and debug
rem # versions were built, the debug version takes precedence.

set EXENAME=
if exist x64\Release\splitspeech.exe set EXENAME=x64\Release\splitspeech.exe
if exist x64\Debug\splitspeech.exe   set EXENAME=x64\Debug\splitspeech.exe
if not "%EXENAME%"=="" goto runtests

echo ------------------------------------------------------
echo The splitspeech.exe program file seems to be missing!
echo Did you build it before running this script?
echo ------------------------------------------------------
goto end

:runtests

echo ------------------------------------------------------
echo Test program found at %EXENAME%.
echo Running tests.  One moment please.
echo ------------------------------------------------------
echo ------------------------------------------------------ >> test.out
%EXENAME% --level=-1 testdata\airhost.wav                   >> test.out
echo ------------------------------------------------------ >> test.out
%EXENAME% --level=-2 testdata\company.wav                   >> test.out
echo ------------------------------------------------------ >> test.out
%EXENAME% --level=-3 testdata\counting.wav                  >> test.out
echo ------------------------------------------------------ >> test.out
%EXENAME% --level=-6 testdata\lateforwork.wav               >> test.out
echo ------------------------------------------------------ >> test.out
%EXENAME% testdata\two.wav                                  >> test.out
echo ------------------------------------------------------ >> test.out
%EXENAME% testdata\score_32bitfloat.wav                     >> test.out
echo ------------------------------------------------------ >> test.out
set EXENAME=

type test.out
echo Test output may be reviewed in file 'test.out'
echo ------------------------------------------------------

:end
