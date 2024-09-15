//-------------------------------------------------------------------
//
// wavfile.h
//
// C++ module to read and write Microsoft WAV audio files.
// This is older code that has been modified several times over
// the years.  It isn't quite as shiny as new code would be.
//
// Note this module intentionally doesn't use any definitions from
// windows.h so we can avoid including it here.
//
// Limitations:
//
// * Only supports raw PCM audio formats, including 8-bit
//   unsigned integer samples, 16-bit signed integer samples,
//   and 32-bit floating-point samples.  Doesn't currently
//   support compressed or adaptive formats.
//
// * When reading WAV files, assumes there are no extra RIFF
//   chunks or padding byte between the file header, the format
//   header, and the audio data.
//
//-------------------------------------------------------------------
//
// (C) Copyright 1994-2024 Ammon R. Campbell.
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
#include <stddef.h>

// Describes the format of the audio data from a Microsoft WAV file.
struct WAVInfo
{
    unsigned m_rate = 48000;        // Sample rate in Hertz.
    unsigned m_channels = 1;        // Channel count: 1=mono, 2=stereo.
    unsigned m_bits = 16;           // Bits per sample: 8, 16, or 32.
    bool m_is_float = false;        // True if sample data is floating-point.
    unsigned m_sample_count = 0;    // Number of audio samples in file.

    // Returns the number of bytes needed to hold the waveform's sample data.
    unsigned CalculateBufferSize() const { return m_channels * m_bits / 8 * m_sample_count; }
};

// Reads the header portion of a WAV file.  Among other things, the
// information from the header can be used to determine how large
// of a sample buffer will be needed to read the audio data from the
// WAV file in a subsequent call to WAVFileReadSamples.
//
// Returns true if successful.
bool WAVFileReadHeader(const wchar_t *filename, WAVInfo &header);

// Reads the audio samples from a WAV file into the provided buffer.
// The buffer_size parameter should indicate the size limit of the
// buffer in bytes.
//
// Returns true if successful.
bool WAVFileReadSamples(const wchar_t *filename, void *sample_buffer, size_t buffer_size);

// Writes a buffer of audio samples to a WAV file.
// The given header specifies the format of the data in the buffer.
//
// Returns true if successful.
bool WAVFileWrite(const wchar_t *filename, const WAVInfo &header, const void *samples);

