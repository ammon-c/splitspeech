### README - SplitSpeech audio splitter program

---

### What is this?

**SplitSpeech** is a program that takes an audio recording of
human speech (spoken words) and splits it into individual
sentences or phrases.  This can be useful when generating speech
data for artificial intelligence training or statistical
analysis.  

The program is run from the Windows command prompt, and it
expects to be given the filenames of one or more Microsoft .WAV
audio files as command line parameters.

The program starts by reading the audio waveform from the given
Microsoft .WAV file, then does the following to it:  

1.  Prints the sample rate and duration of the waveform that
is being processed.

2.  Splits the audio waveform into segments of individual
sentences and phrases by detecting the silences between them. 
The position and duration of each segment is printed to the
console.  

3.  Normalizes the audio level to a specific decibel level.  The
default level is -1 dB (or 1 dB below clipping), but this can be
changed on the command line by adding a command line parameter
of the form "--level=X", where "X" is the desired decibel level.

4.  After all audio processing, the audio segments are written to
WAV files whose names are similar to the original WAV file, but
with the text "_seg" and the segment number inserted at the end
of the base of the filename.  For example, the first two
segments from a file named **myfile.wav** would be written to
files named **myfile_seg1.wav** and **myfile_seg2.wav** in the
current working directory.  

### Platforms

This program is intended to compile with Microsoft Visual Studio
2022, and run on Windows 10 or 11.  

There are only a few Windows-specific things in the program's
code, so it could probably also be made to work on Linux or
macOS with a few hours of additional work.  

### Limitations

1. Supports single channel (mono) audio formats only.  Stereo or
multi-channel audio files are flattened to a monophonic format
before processing, so the output will always be mono.  

2. Supports raw PCM audio formats only.  WAV files that contain
compressed/adaptive/differential PCM data are not currently
supported.  

### Building the program

**Building** :  From the Windows command prompt, run **NMAKE**
without command line arguments to build the DEBUG version of the
program.  Or run **NMAKE RELEASE=1** to build the RELEASE
version of the program.  Note that you must have already
configured your Visual Studio tools to be accessible from the
Windows command line by running the appropriate Visual Studio
environment script (e.g.  vcvarsall.bat) in the same console
session before doing a build.  

**Build target** :  If the build is successful, the binary
executable for the DEBUG version is written to
**x64\Debug\splitspeech.exe**.  If building the RELEASE version,
the binary is written to **x64\Release\splitspeech.exe**
instead.  The unit tests are compiled into a binary named
**unittest.exe**, which is located in the same directory with
the **splitspeech.exe** file.  

### Documentation

* [**readme.md**](readme.md) : The file you are reading right now.

* Please refer to comments in the source code for more
information about how the code works.  

### Source code

* [**splitspeech.cpp**](splitspeech.cpp) :  This is the main
module of the program.  The main entry point and higher level
program functions are located here.  

* [**waveform.h**](waveform.h),
[**waveform.cpp**](waveform.cpp) :  Implements a simple
container class for a floating-point PCM audio waveform.  The
calling app uses this to store and manipulate audio samples in
memory.  

* [**normalize.h**](normalize.h),
[**normalize.cpp**](normalize.cpp) :  This is the code for
normalizing an audio waveform to a preset level.  It processes
the waveform in chunks of several milliseconds at a time,
calculating the peak level, and adjusting the audio gain
multiplier up if the peak is too low, or down if the peak is too
high.  

* [**segment.h**](segment.h), [**segment.cpp**](segment.cpp) : 
This is the code for identifying segments in an audio waveform
by looking for the quiet sections that occur between sentences
or phrases.  It examines the waveform in chunks of several
milliseconds at a time, calculating the standard deviation for
the audio samples, and if there have been several chunks in a
row with a very low standard deviation, it treats that as a
silence between segments.  

* [**wavfile.h**](wavfile.h), [**wavfile.cpp**](wavfile.cpp) :  
This is some older code I wrote to read and write Microsoft .WAV
files.  The .WAV file code in **waveform.cpp** calls this
module.  

* [**makefile**](makefile) :  A build script for building the
**splitspeech.exe** program from the source code using the Microsoft
**NMAKE** tool.  **NMAKE** is one of the Microsoft Visual Studio
command line tools.  

* [**unittest.cpp**](unittest.cpp),
[**unittest.vcxproj**](unittest.vcxproj),
[**wavfile_test.cpp**](wavfile_test.cpp),
[**normalize_test.cpp**](normalize_test.cpp),
[**segment_test.cpp**](segment_test.cpp) :  Source code for
some very basic unit tests.  

### Tests

**Manual tests:**

The [**testdata**](testdata) directory contains several example
.WAV files for testing purposes.  

The file [**runtest.bat**](runtest.bat) is a Windows batch
script that runs the audio processing program on several .WAV
files from the **testdata** directory.  The script's console
output prints to a file named **test.out** when executed.
After running the **runtest.bat** script, you may examine the
**test.out** file for expected outputs and/or error messages. 
After running the test script, there will also be several new
.WAV files which contain the segmented audio that was extracted
from the .WAV test files.

Please note that these tests are *not* fully automatic.  The
human operator will need to listen to the new .WAV files and
compare them to the original .WAV files to verify that the
program produced the desired output.  

**Unit tests:**

There are some very basic unit tests for some of the program
functions.  They get compiled into a binary named
**unittest.exe** that is located in the same directory with the
**splitspeech.exe** file.  

The file [**unittest.bat**](unittest.bat) is a Windows batch
script that runs the unit tests on several .WAV files from the
**testdata** directory.  The script's console output prints
to a file named **unittest.out** when executed.
After running the script, you may examine the **unittest.out**
file to see if all of the tests passed.  The script also returns
a non-zero exit code if any unit test fails.

-*- end -*-
