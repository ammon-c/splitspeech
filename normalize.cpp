//-------------------------------------------------------------------
//
// normalize.cpp
//
// C++ module for normalizing an audio waveform to a preset level.
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

#include "normalize.h"
#include <math.h>

// From an attenuation level between 0 dB (loudest) and -infinity
// dB (quietest), returns the corresponding linear gain multiplier
// value.
static float db_to_linear(float db)
{
    return powf(10.0f, db / 20.0f);
}

// Normalizes an audio waveform such that the level doesn't exceed
// the specified dB level (where 0dB=loudest, -infinity=quietest).
// The waveform data is modified in place.
void NormalizeAudioWaveform(Waveform &wav, float db_level)
{
    if (wav.m_data.empty())
        return;

    //
    // Examine the waveform in chunks of about 50 milliseconds
    // each, finding the peak volume level of each chunk as we go.
    //
    // If the current chunk's audio peak is lower than the target
    // maximum, we increase the gain slightly.  If the peak is
    // higher than the target maximum, reduce the gain abruptly.
    //
    // So it behaves kind of like an analog microphone compressor.
    //

    const float max_vol = db_to_linear(db_level);
    const unsigned samples_per_chunk = static_cast<unsigned>(wav.m_frequency * 0.01f);
    const unsigned num_chunks = static_cast<unsigned>(wav.m_data.size() / samples_per_chunk);
    float gain = 1.0f;

    for (unsigned chunk = 0; chunk < num_chunks; chunk++)
    {
        // Determine the peak volume of the samples in this chunk.
        unsigned isample = static_cast<unsigned>(chunk * samples_per_chunk);
        float local_peak = 0.0f;
        for (unsigned subsample = 0; subsample < samples_per_chunk; subsample++)
        {
            float vol = fabsf(wav.m_data[isample + subsample]);
            if (vol > local_peak)
                local_peak = vol;
        }

        // If this chunks's peak volume is less than the target max,
        // gradually increase the gain.
        if (local_peak < max_vol && gain < 100.0f)
            gain *= 1.05f;

        // If this chunks's peak volume exceeds the target max,
        // drop the gain abruptly. 
        if (local_peak * gain > max_vol)
        {
            if (local_peak < 0.02f)
                gain = max_vol / 0.02f;
            else
                gain = max_vol / local_peak;
        }

        // Apply the gain multiplier to the samples in this chunk.
        // If this is the last full chunk, also apply the gain to
        // any remaining partial chunk at the very end of the
        // waveform.
        size_t count = samples_per_chunk;
        if (chunk == num_chunks - 1)
            count = wav.m_data.size() - isample;
        for (size_t subsample = 0; subsample < count; subsample++)
            wav.m_data[isample + subsample] *= gain;
    }
}

