//-------------------------------------------------------------------
//
// splitspeech.cpp
//
// This program does the following with a WAV audio file:
//
//   1. Prints the sample rate and duration to the console.
//   2. Segments the audio by detecting silences between spoken
//      phrases or sentences.
//      The starting and ending position of each segment is printed
//      to the console.
//   3. Normalizes the audio level to a specific decibel level
//      (default level is -1 dB below clipping).
//
// After all audio processing, the audio segments are written to
// WAV files whose names are similar to the original WAV file, but
// with "_seg" and the segment number inserted at the end of the
// base of the filename.  For example, the first two segments from
// "myfile.wav" would be written to files named "myfile_seg1.wav"
// and "myfile_seg2.wav" in the current working directory.
//
// Platforms:
//
// * This program is intended to compile with Microsoft Visual
//   Studio 2022 and run on Windows 10 or 11. 
//
// Limitations:
//
// * Supports single channel (mono) audio formats only.  Stereo or
//   multi-channel audio files are flattened to a monophonic
//   format before processing, so the output will always be mono.
//
// * Supports raw PCM audio formats only.  WAV files that contain
//   compressed/adaptive/differential PCM data are not currently
//   supported. 
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
#include "segment.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <wchar.h>
#include <vector>

#define MAX_PATH 512

// Prints a time duration to the console in a consistent format,
// showing the elapsed hours, minutes, and seconds.
void print_duration(float seconds)
{
    int mhour = static_cast<int>(seconds / (60 * 60));
    seconds -= mhour * (60 * 60);
    int mmin = static_cast<int>(seconds / 60);
    seconds -= mmin * 60;

    if (mhour)
        printf("%dh:", mhour);

    if (mmin)
        printf(mhour ? "%02dm:" : "%dm:", mmin);

    printf((mhour || mmin) ? "%05.2fds" : "%.2fs", seconds);
}

// Writes the waveform's audio segments to individual WAV files.
// The filename passed here is used as a template for creating
// the segment WAV files, by inserting "_seg" and a number at
// the end of the filename.  So, for example, the first two
// segments from "myfile.wav" would be written to files named
// "myfile_seg1.wav" and "myfile_seg2.wav" in the current working
// directory.
// Returns true if successful.
static bool write_audio_segments_to_wav_files(
    const Waveform &wav,
    const wchar_t *filename,
    const std::vector<Segment> &segments)
{
    if (wav.m_data.empty() || segments.empty())
    {
        printf("ERROR: No audio data to output.\n");
        return false;
    }
    if (!filename || !*filename)
    {
        printf("ERROR: Missing filename.\n");
        return false;
    }

    // Extract the basename portion of the filename.
    wchar_t basename[MAX_PATH] = {0};
    const wchar_t *base_start = wcsrchr(filename, '\\');
    if (!base_start)
        base_start = filename;
    else
        base_start++;
    const wchar_t *base_end = wcsrchr(filename, '.');
    if (!base_end)
        base_end = filename + wcslen(filename);
    wcsncpy_s(basename, MAX_PATH, base_start, base_end - base_start);

    // Write the processed audio to new WAV file(s).
    unsigned seg_num = 0;
    for (const Segment &segment : segments)
    {
        wchar_t new_filename[MAX_PATH] = {0};
        _snwprintf_s(
            new_filename, MAX_PATH, L"%s_seg%u.wav", basename, ++seg_num);

        printf("Writing '%S' starting at %zu for %zu samples\n", new_filename, segment.m_start, segment.m_count);

        if (!wav.WriteToWAVFile(new_filename, static_cast<unsigned>(segment.m_start), static_cast<unsigned>(segment.m_count)))
        {
            printf("ERROR: Attempted write of '%S' was not successful.\n", new_filename);
            return false;
        }
    }

    return true;
}

// Performs audio processing tasks on the given WAV file.
// Returns true if successful.
static bool process_wav_file(wchar_t *filename, float db_level)
{
    // Load PCM audio from the WAV file.
    Waveform wav;
    if (!wav.LoadFromWAVFile(filename))
    {
        printf("ERROR: Attempted load of '%S' was not successful.\n", filename);
        return false;
    }

    // Print info about the WAV file.
    printf("File %S:\n", filename);
    printf("  Sample rate:  %.2f KHz\n", wav.m_frequency / 1000.0);
    printf("  Duration:     ");
    print_duration(wav.m_data.size() / static_cast<float>(wav.m_frequency));
    printf("\n");

    // Segment the audio.
    auto segments = FindSegmentsInAudioWaveform(wav);
    if (segments.empty())
    {
        printf("ERROR: Failed segmenting '%S'.  Is the entire waveform silent?\n", filename);
        return false;
    }

    // Print info about the audio segments to the console.
    int seg_num = 0;
    for (const Segment &segment : segments)
    {
        printf("Segment %d:\n", ++seg_num);
        printf("  Starts at sample %zu, runs for %zu samples\n", segment.m_start, segment.m_count);
        printf("  Start time:  ");
        print_duration(segment.m_start / static_cast<float>(wav.m_frequency));
        printf("\n");
        printf("  Length:      ");
        print_duration(segment.m_count / static_cast<float>(wav.m_frequency));
        printf("\n");
        printf("  End time:    ");
        print_duration((segment.m_start + segment.m_count) / static_cast<float>(wav.m_frequency));
        printf("\n");
    }

    // Normalize the audio to a uniform level.
    NormalizeAudioWaveform(wav, db_level);

    // Save the processed audio segments.
    return write_audio_segments_to_wav_files(wav, filename, segments);
}

// The entry point is wmain instead of main so we get Unicode
// command line arguments from Windows.  Otherwise non-English
// filenames don't work (Windows doesn't support UTF-8 in file
// system calls like Linux and macOS do). 
int wmain(int argc, wchar_t **argv)
{
    // Make sure the user gave us at least one argument.
    if (argc < 2)
    {
        printf(
            "Usage:  splitspeech [--level=X] file1.wav [file2.wav ...]\n"
            "\n"
            "Options:\n"
            "  --level=X  Normalize audio waveforms to X decibels,\n"
            "             where X is between -100 and 0 inclusive.\n"
            "             The default is -1.0 dB.\n"
            );

        return EXIT_FAILURE;
    }

    float db_level = -1.0f;
    unsigned error_count = 0;
    try
    {
        // Process each WAV file that was given on the command line.
        for (int iarg = 1; iarg < argc; iarg++)
        {
            const wchar_t *level_option = L"--level=";
            const size_t level_option_len = wcslen(level_option);

            if (wcsncmp(argv[iarg], level_option, level_option_len) == 0)
            {
                db_level = static_cast<float>(_wtof(&argv[iarg][level_option_len]));
                if (db_level > 0.0f || db_level < -100.0f)
                {
                    printf("ERROR: Level value %S out of range (expected value -100 to 0).\n", argv[iarg]);
                    return EXIT_FAILURE;
                }
            }
            else if (wcsncmp(argv[iarg], L"--", 2) == 0)
            {
                printf("ERROR: Unrecognized option switch: %S\n", argv[iarg]);
                return EXIT_FAILURE;
            }
            else if (!process_wav_file(argv[iarg], db_level))
            {
                printf("ERROR: One or more error(s) processing %S\n", argv[iarg]);
                ++error_count;
            }
        }
    }
    catch(...)
    {
        printf("ERROR: Unexpected program exception!\n");
        ++error_count;
    }

    if (error_count)
    {
        printf("Exiting with %u error(s)!\n", error_count);
        return EXIT_FAILURE;
    }

    printf("Completed OK.\n");
    return EXIT_SUCCESS;
}

