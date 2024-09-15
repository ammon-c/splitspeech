//-------------------------------------------------------------------
//
// waveform.h
//
// Waveform container class.  Stores and manipulates a monophonic
// floating-point PCM audio signal.
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

#pragma once
#include "wavfile.h"
#include <vector>

// Container class for a single-channel PCM audio waveform.
// Internally we store the audio as an array of floating-point
// sample values between -1.0 and +1.0.  The caller may access
// these data values directly through the m_data member.
class Waveform
{
public:
    Waveform() = default;
    ~Waveform() = default;

    // Returns the duration of the waveform in seconds.
    double DurationInSeconds() const;

    // Finds the lowest and highest sample values in the waveform.
    void FindMinMaxSamples(float &smin, float &smax) const;

    // Loads this waveform object with the PCM audio from a WAV file.
    // Returns true if successful.
    bool LoadFromWAVFile(const wchar_t *filename);

    // Writes the PCM audio waveform to a WAV file on disk.
    // A specific subset of the waveform can be written to the
    // file by using the 'start_sample' and 'num_samples'
    // parameters.
    // Returns false if the file could not be written.
    bool WriteToWAVFile(const wchar_t *filename, unsigned start_sample = 0, unsigned num_samples = 0) const;

    unsigned m_frequency = 48000;   // Sample frequency in Hertz.
    std::vector<float> m_data;      // Buffer of audio samples.
};

