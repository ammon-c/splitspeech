//-------------------------------------------------------------------
//
// wavfile_test.cpp
//
// Simple test of the wavfile.cpp module.  Given the name of a WAV
// file, this attempts to read, write, and re-read the waveform(s)
// from the WAV file(s), comparing to the original to confirm that
// the code is not altering it while reading or writing it.
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

#include "wavfile.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

bool test_wavfile_read_write(wchar_t *filename)
{
    printf("Starting WAV read/write test with '%S'\n", filename);

    // Read the WAV file.
    WAVInfo info;
    if (!WAVFileReadHeader(filename, info))
    {
        printf("WAVFileReadHeader failed reading '%S'\n", filename);
        return false;
    }
    std::vector<char> samples(info.CalculateBufferSize());
    if (!WAVFileReadSamples(filename, samples.data(), samples.size()))
    {
        printf("WAVFileReadSamples failed reading '%S'\n", filename);
        return false;
    }

    // Write the waveform to a new WAV file.
    const wchar_t *new_filename = L"temp.wav";
    if (!WAVFileWrite(new_filename, info, samples.data()))
    {
        printf("WAVFileWrite failed writing '%S'\n", filename);
        return false;
    }

    // Read the new WAV file.
    WAVInfo info2;
    if (!WAVFileReadHeader(new_filename, info2))
    {
        printf("WAVFileReadHeader failed reading '%S'\n", new_filename);
        _wunlink(L"temp.wav");
        return false;
    }
    std::vector<char> samples2(info2.CalculateBufferSize());
    if (!WAVFileReadSamples(new_filename, samples2.data(), samples2.size()))
    {
        printf("WAVFileReadSamples failed reading '%S'\n", new_filename);
        _wunlink(L"temp.wav");
        return false;
    }

    _wunlink(L"temp.wav");

    // Check if the info from the new WAV file is similar to
    // the original WAV file.
    if (info.m_rate != info2.m_rate)
    {
        printf("Sample rate of re-written WAV doesn't match!\n");
        printf("  Before: %u Hz\n", info.m_rate);
        printf("  After:  %u Hz\n", info2.m_rate);
        return false;
    }
    else
    {
        printf("Sample rate matches OK.\n");
    }

    if (info.m_sample_count != info2.m_sample_count)
    {
        printf("Sample count of re-written WAV doesn't match!\n");
        printf("  Before:  %u\n", info.m_sample_count);
        printf("  After:   %u\n", info2.m_sample_count);
        return false;
    }
    else
    {
        printf("Sample count matches OK.\n");
    }

    // Compare the audio samples, too.
    if (samples.size() == samples2.size())
    {
        if (memcmp(samples.data(), samples2.data(), samples.size()))
        {
            printf("Sample data of re-written WAV doesn't match!\n");
            return false;
        }
        else
        {
            printf("Sample data matches OK.\n");
        }
    }

    return true;
}

