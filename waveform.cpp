//-------------------------------------------------------------------
//
// waveform.cpp
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

#include "waveform.h"

double Waveform::DurationInSeconds() const
{
    if (!m_frequency || m_data.empty())
        return 0.;

    return static_cast<double>(m_data.size()) / m_frequency;
}

void Waveform::FindMinMaxSamples(float &smin, float &smax) const
{
    if (m_data.empty())
    {
        smin = smax = 0.f;
        return;
    }

    smin = FLT_MAX;
    smax = -FLT_MAX;
    for (unsigned isample = 0; isample < m_data.size(); isample++)
    {
        if (m_data[isample] < smin)
            smin = m_data[isample];
        if (m_data[isample] > smax)
            smax = m_data[isample];
    }
}

bool Waveform::LoadFromWAVFile(const wchar_t *filename)
{
    WAVInfo header;
    if (!WAVFileReadHeader(filename, header))
        return false;

    std::vector<char> raw(header.CalculateBufferSize());
    if (!WAVFileReadSamples(filename, raw.data(), raw.size()))
        return false;

    m_frequency = header.m_rate;
    m_data.resize(header.m_sample_count);

    // Convert data from the file's format to our internal format.
    // If the data is stereo/multichannel it will also be flattened
    // to mono.
    if (header.m_is_float && header.m_bits == 32)
    {
        // Data is already floating-point, just merge the channels to mono.
        // cppcheck-suppress invalidPointerCast
        const float *in = reinterpret_cast<const float *>(raw.data());
        float *out = reinterpret_cast<float *>(m_data.data());
        for (size_t isample = 0; isample < header.m_sample_count; isample++)
        {
            float sample = 0.0;

            for (unsigned channel = 0; channel < header.m_channels; ++channel)
                sample += *in++;
            sample /= header.m_channels;

            *out++ = sample;
        }
    }
    else if (header.m_bits == 16)
    {
        // Convert 16-bit integer PCM to floating-point, and merge to mono.
        const int16_t *in = reinterpret_cast<const int16_t *>(raw.data());
        float *out = reinterpret_cast<float *>(m_data.data());
        for (size_t isample = 0; isample < header.m_sample_count; isample++)
        {
            float sample = 0.0;

            for (unsigned channel = 0; channel < header.m_channels; ++channel)
                sample += (*in++) / 32768.f;

            sample /= header.m_channels;
            *out++ = sample;
        }
    }
    else if (header.m_bits == 8)
    {
        // Convert 8-bit unsigned integer PCM to floating-point, and merge to mono.
        const uint8_t *in = reinterpret_cast<const uint8_t *>(raw.data());
        float *out = reinterpret_cast<float *>(m_data.data());
        for (size_t isample = 0; isample < header.m_sample_count; isample++)
        {
            float sample = 0.0;

            for (unsigned channel = 0; channel < header.m_channels; ++channel)
                sample += ((*in++) - 128.f) / 128.f;

            sample /= header.m_channels;
            *out++ = sample;
        }
    }

    return true;
}

bool Waveform::WriteToWAVFile(const wchar_t *filename, unsigned start_sample, unsigned num_samples) const
{
    if (!filename || m_data.empty())
        return false;
    if (start_sample >= m_data.size())
        return false;
    if (!num_samples)
        num_samples = static_cast<unsigned>(m_data.size() - start_sample);
    if (start_sample + num_samples > m_data.size())
        return false;

    // Convert the samples from floating-point to 16-bit PCM.
    std::vector<int16_t> samples(num_samples);
    for (size_t isample = 0; isample < num_samples; isample++)
        samples[isample] = static_cast<int16_t>(m_data[start_sample + isample] * 32768);

    // Fill in the header and write the file.
    WAVInfo header;
    header.m_rate = m_frequency;
    header.m_channels = 1;
    header.m_bits = 16;
    header.m_is_float = false;
    header.m_sample_count = num_samples;
    return WAVFileWrite(filename, header, samples.data());
}

