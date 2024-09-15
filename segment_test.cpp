//-------------------------------------------------------------------
//
// segment_test.cpp
//
// Simple test of the segment.cpp module.  Generates a waveform
// containing several 'beep' tones, then runs the waveform through
// the segmentation function and comfirms the number of segments
// found matches the number of test tones that were generated.
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
#include "segment.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

bool test_segmentation()
{
    printf("Starting audio segmentation test\n");

    // Generate a 100 second long waveform with 9 non-overlapping test tones.
    Waveform wav;
    wav.m_frequency = 10000;
    wav.m_data.resize(wav.m_frequency * 100);
    memset(wav.m_data.data(), 0, wav.m_data.size());
    for (int itone = 1; itone <= 9; itone++)
    {
        unsigned offset = itone * (wav.m_frequency * 10) + ((rand() % 2000) - 1000);
        unsigned tone_len = 1000 + (rand() % 5000);
        float level = 0.1f + (1 + rand() % 1000) / 2000.0f;
        unsigned width = 1 + (rand() % 4);

        for (unsigned i = 0; i < tone_len; i++)
            wav.m_data[offset + i] = ((i % (width * 2)) < width) ? level : -level;
    }

    auto segments = FindSegmentsInAudioWaveform(wav);

    // Print the segments info to the console for debugging:
    //
    // int iseg = 0;
    // for (auto segment : segments)
    // {
    //     printf("  Segment %d:  start=%zu  count=%zu\n",
    //         ++iseg, segment.m_start, segment.m_count);
    // }

    // See if we found the number of segments we expected to find.
    if (segments.size() != 9)
    {
        printf("Expected 9 segments, found %zu!\n", segments.size());
        return false;
    }

    // TODO: Also check the positions of the segments within the
    // waveform to see if they roughly correspond to where we
    // placed the test tones.

    printf("Segmentation test OK.\n");
    return true;
}

