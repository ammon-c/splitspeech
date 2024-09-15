#
# Makefile to build the splitspeech audio processing program
# on Windows.  Requires Microsoft Visual Studio 2022 with NMAKE.
#
# To build in debug mode:
#    NMAKE
#
# To build in release mode:
#    NMAKE RELEASE=1
#
# Debug binaries live in "x64\Debug" subdirectory.
# Release binaries live in "x64\Release" subdirectory.
#

!ifdef RELEASE
OBJDIR=x64\ReleaseObj
BINDIR=x64\Release
CPPFLAGS= -nologo -c -Gs -EHsc -W4 -WX -DWIN32 -D_WIN32 -MT -Ox
!else
OBJDIR=x64\DebugObj
BINDIR=x64\Debug
CPPFLAGS= -nologo -c -Gs -EHsc -W4 -WX -DWIN32 -D_WIN32 -D_DEBUG -MTd -Od -Zi
!endif

HDRS= waveform.h wavfile.h segment.h normalize.h

.SUFFIXES: .c .cpp

{.}.cpp{$(OBJDIR)}.obj:
   cl $(CPPFLAGS) -Fo$*.obj -Fd$(OBJDIR)\vc140.pdb $<

{.}.c{$(OBJDIR)}.obj:
   cl $(CPPFLAGS) -Fo$*.obj -Fd$(OBJDIR)\vc140.pdb $<


all:  $(BINDIR) $(OBJDIR) $(BINDIR)\splitspeech.exe $(BINDIR)\unittest.exe

# Create the subdirectory where the binaries get placed during the build.
$(BINDIR):
    if not exist x64 mkdir x64
    if not exist $(BINDIR) mkdir $(BINDIR)

# Create the subdirectory where the object files get placed during the build.
$(OBJDIR):
    if not exist x64 mkdir x64
    if not exist $(OBJDIR) mkdir $(OBJDIR)

# Build the WAV audio processing program from the object files.
$(BINDIR)\splitspeech.exe: $(OBJDIR)\splitspeech.obj $(OBJDIR)\wavfile.obj \
        $(OBJDIR)\waveform.obj $(OBJDIR)\normalize.obj \
        $(OBJDIR)\segment.obj
   link /NOLOGO /DEBUG $** gdi32.lib user32.lib /OUT:$@

# Build the program that runs the unit tests.
$(BINDIR)\unittest.exe: $(OBJDIR)\unittest.obj \
        $(OBJDIR)\wavfile_test.obj $(OBJDIR)\normalize_test.obj \
        $(OBJDIR)\segment_test.obj $(OBJDIR)\wavfile.obj \
        $(OBJDIR)\waveform.obj $(OBJDIR)\normalize.obj \
        $(OBJDIR)\segment.obj
   link /NOLOGO /DEBUG $** gdi32.lib user32.lib /OUT:$@

$(OBJDIR)\normalize.obj:       normalize.cpp       $(HDRS)
$(OBJDIR)\normalize_test.obj:  normalize_test.cpp  $(HDRS)
$(OBJDIR)\segment.obj:         segment.cpp         $(HDRS)
$(OBJDIR)\segment_test.obj:    segment_test.cpp    $(HDRS)
$(OBJDIR)\splitspeech.obj:     splitspeech.cpp     $(HDRS)
$(OBJDIR)\unittest.obj:        unittest.cpp        $(HDRS)
$(OBJDIR)\waveform.obj:        waveform.cpp        $(HDRS)
$(OBJDIR)\wavfile.obj:         wavfile.cpp         $(HDRS)
$(OBJDIR)\wavfile_test.obj:    wavfile_test.cpp    $(HDRS)

# Purge all target and object files, leaving just the source files.
clean:
    if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
    if exist $(OBJDIR)\*.res del $(OBJDIR)\*.res
    if exist $(OBJDIR)\*.pdb del $(OBJDIR)\*.pdb
    if exist $(BINDIR)\*.exe del $(BINDIR)\*.exe
    if exist $(BINDIR)\*.ilk del $(BINDIR)\*.ilk
    if exist $(BINDIR)\*.pdb del $(BINDIR)\*.pdb
    if exist $(BINDIR)\*.map del $(BINDIR)\*.map
    if exist $(BINDIR)\.vs rmdir /s /q $(BINDIR)\.vs
    if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
    if exist $(BINDIR) rmdir /s /q $(BINDIR)
    if exist x64 rmdir /s /q x64
    if exist .vs rmdir /s /q .vs
    if exist *.bak del *.bak
    if exist *.user del *.user
    if exist *_seg*.wav del *_seg*.wav
    if exist temp.wav del temp.wav
    if exist test.out del test.out
    if exist unittest.out del unittest.out
