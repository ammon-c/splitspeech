//-------------------------------------------------------------------
//
// segment.cpp
//
// C++ module for finding the non-silent segments of an audio
// waveform.
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

#include "segment.h"
#include <math.h>

// Returns the standard deviation of an array of data values.
static float standard_deviation(const float *data, size_t count)
{
    if (count < 1)
        return 0.0f;

    float sum = 0.0f;
    for (size_t i = 0; i < count; i++)
        sum += data[i];

    float mean = sum / count;
    float v = 0;
    for (size_t i = 0; i < count; i++)
        v += powf(data[i] - mean, 2);

    return sqrtf(v / count);
}

// Determines where the segments are in the given waveform by
// detecting where the waveform is silent (or near silent).
// A list of the non-silent segments is returned.  An empty
// list is returned if the entire waveform is silent.
std::vector<Segment> FindSegmentsInAudioWaveform(const Waveform &wav)
{
    //
    // Algorithm:
    //
    // We examine the waveform in chunks of a few milliseconds
    // at a time.  For each chunk, we calculate the standard
    // deviation of the samples within the chunk.  If we see
    // that a sufficient number of the recent chunks had
    // "loud" audio signals (signals above a certain standard
    // deviation threshold), then we mark the beginning of an
    // audio segment.  Processing of the waveform continues,
    // and when a sufficient number of recent chunks have
    // "quiet" audio signals (signals below a certain standard
    // deviation threshold), we mark the end of the segment
    // and go back to looking for "loud" chunks to mark the
    // beginning of the next segment; and so on.
    //

    const unsigned samples_per_chunk = static_cast<unsigned>(wav.m_frequency * 0.05);
    const unsigned num_chunks = static_cast<unsigned>(wav.m_data.size() / samples_per_chunk);

    // Calculate the standard deviation for each chunk in the waveform.
    std::vector<float> stddev_per_chunk(num_chunks);
    for (unsigned ichunk = 0; ichunk < num_chunks; ichunk++)
    {
        unsigned isample = static_cast<unsigned>(ichunk * samples_per_chunk);
        stddev_per_chunk[ichunk] = standard_deviation(&wav.m_data[isample], samples_per_chunk);
    }

    // Calculate the threshold we'll use to separate "loud" from "quiet".
    float sample_min = 0.0f, sample_max = 0.0f;
    wav.FindMinMaxSamples(sample_min, sample_max);
    float sample_peak = (sample_max - sample_min) / 2.0f;
    float stddev_threshold = sample_peak * 0.05f;

    // These values can be tweaked so that more or less silence is
    // required to trigger a change between "loud" or "quiet" in the
    // waveform.
    const unsigned recent_count = 10;   // How many chunks we will look backward for loud or quiet.
    const unsigned louds_to_start = 3;  // If this many recent chunks are loud, we start a new segment.
    const unsigned quiets_to_stop = 8;  // If this many recent chunks are quiet, we stop the current segment.

    // Process each chunk in the waveform, looking for segments.
    std::vector<Segment> list;
    unsigned segment_started_tick = 0;
    for (unsigned tick = 0; tick < num_chunks; tick++)
    {
        // Determine how many recent segments were "loud" or "quiet".
        unsigned recent_quiet = 0;
        unsigned recent_loud = 0;
        for (int j = tick; j > static_cast<int>(tick - recent_count); j--)
        {
            bool quiet = true;
            if (j > 0)
                quiet = (stddev_per_chunk[j] < stddev_threshold);
            if (quiet)
                ++recent_quiet;
            else
                ++recent_loud;
        }

        if (!segment_started_tick && recent_loud >= louds_to_start)
        {
            // A new segment starts here.
            segment_started_tick = tick - recent_loud;
        }
        else if (segment_started_tick &&
            (recent_quiet >= quiets_to_stop || tick == num_chunks - 1))
        {
            // The segment ends here.
            Segment seg;
            seg.m_start = static_cast<unsigned>(segment_started_tick * samples_per_chunk);
            seg.m_count = static_cast<unsigned>((tick - segment_started_tick) * samples_per_chunk);
            list.push_back(seg);
            segment_started_tick = 0;
        }
    }

    return list;
}

