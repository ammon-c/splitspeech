//-------------------------------------------------------------------
//
// normalize_test.cpp
//
// Simple test of the normalize.cpp module.  Given the name of a
// WAV file, reads the waveform, normalizes it, then checks the
// audio data to confirm that it was normalized.
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
#include "normalize.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

// From an attenuation level between 0 dB (loudest) and -infinity
// dB (quietest), returns the corresponding linear gain multiplier
// value.
static float db_to_linear(float db)
{
    return powf(10.0f, db / 20.0f);
}

bool test_normalize(wchar_t *filename)
{
    printf("Starting audio normalization test with '%S'\n", filename);

    // Read the WAV file.
    Waveform wav;
    if (!wav.LoadFromWAVFile(filename))
    {
        printf("WAVFileReadHeader failed reading '%S'\n", filename);
        return false;
    }

    // Normalize the waveform.
    float db_level = -1.0f;
    float linear_level = db_to_linear(db_level);
    NormalizeAudioWaveform(wav, db_level);

    // See if the max level in the waveform matches the
    // normalization level we asked for.
    float smin = 0, smax = 0;
    wav.FindMinMaxSamples(smin, smax);
    if (smax > linear_level * 1.001)
    {
        printf("Audio level higher than expected after normalization!\n");
        printf("  Target:  %.4f\n", linear_level);
        printf("  Actual:  %.4f\n", smax);
        return false;
    }
    if (smax < linear_level * 0.8)
    {
        printf("Audio level lower than expected after normalization!\n");
        printf("  Target:  %.4f\n", linear_level);
        printf("  Actual:  %.4f\n", smax);
        return false;
    }

    return true;
}

