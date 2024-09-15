//-------------------------------------------------------------------
//
// unittest.cpp
//
// Main module of program to run the unit tests for various functions
// of the WAV audio processing program.
//
// This program takes one or more WAV files on the command line and
// performs several tests on each WAV file.
//
//-------------------------------------------------------------------
//
// (C) Copyright 2024 Ammon R. Campbell.
//
// I wrote this code for use in my own educational and experimental
// programs, but you may also freely use it in yours as long as you
// abide by the following terms and conditions:
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials
//     provided with the distribution.
//   * The name(s) of the author(s) and contributors (if any) may not
//     be used to endorse or promote products derived from this
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.  IN OTHER WORDS, USE AT YOUR OWN RISK, NOT OURS.  
//--------------------------------------------------------------------

#include "waveform.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

// Declare any test functions we will be calling from other test modules.
extern bool test_wavfile_read_write(wchar_t *filename);
extern bool test_normalize(wchar_t *filename);
extern bool test_segmentation();

// Performs tests using the specified WAV file.
static bool process_wav_file(wchar_t *filename)
{
    printf("Testing with WAV file '%S'\n", filename);

    unsigned error_count = 0;

    if (!test_wavfile_read_write(filename))
        error_count++;

    if (!test_normalize(filename))
        error_count++;

    printf("Done testing with '%S'\n", filename);

    return (error_count == 0);
}

int wmain(int argc, wchar_t **argv)
{
    // Make sure the user gave us at least one filename.
    if (argc < 2)
    {
        printf("Usage:  unittest file1.wav [file2.wav ...]\n");
        return EXIT_FAILURE;
    }

    unsigned error_count = 0;
    try
    {
        // Process each WAV file that was given on the command line.
        for (int iarg = 1; iarg < argc; iarg++)
        {
            if (!process_wav_file(argv[iarg]))
                ++error_count;
        }

        // Run any tests that don't use the WAV files.
        if (!test_segmentation())
            error_count++;
    }
    catch(...)
    {
        printf("ERROR: Unexpected program exception!\n");
        ++error_count;
    }

    if (error_count)
    {
        printf("ERROR: Exiting with %u error(s)!\n", error_count);
        return EXIT_FAILURE;
    }

    printf("Completed OK.\n");
    return EXIT_SUCCESS;
}

