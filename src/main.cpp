#include <iostream>
#include <experimental/filesystem>
#include <lame/lame.h>
#include <cstring>
#include "Converter.h"

namespace fs = std::experimental::filesystem;

const char RIFF_MAGIC[4] = {'R', 'I', 'F', 'F'};
const int FORMAT_OFFSET = 8;
const char WAVE_FORMAT[4] = {'W', 'A', 'V', 'E'};
const int AUDIO_FORMAT_OFFSET = 20;
const short PCM_AUDIO_FORMAT = 1;

int main(int argc, char *argv[]) {

    // Check the number of parameters
    if (argc != 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <directory>" << std::endl;
        return 1;
    }
    // Get directory from command line arguments
    char *directory = argv[1];
    // Check if directory exist and if it is a directory
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        // Notify directory does not exist
        std::cerr << "Not found directory '" << directory << "'" << std::endl;
        return 2;
    }

    Converter converter;

    // Iterate over all items in the directory
    for (auto it = fs::directory_iterator(directory); it != fs::directory_iterator(); ++it) {
        // Select only *.wav files to be processed
        if (!fs::is_regular_file(it->path()) || strcasecmp(it->path().extension().c_str(), ".wav") != 0) {
            continue;
        }
        FILE *wav_file = fopen(it->path().c_str(), "rb");
        if (!wav_file) {
            std::cerr << "Unable to open file " << it->path() << std::endl;
            continue;
        }

        char magic[4];
        char format[4];
        short audio_format;

        std::fread(magic, sizeof(char), 4, wav_file);
        std::fseek(wav_file, FORMAT_OFFSET, SEEK_SET);
        std::fread(format, sizeof(char), 4, wav_file);
        std::fseek(wav_file, AUDIO_FORMAT_OFFSET, SEEK_SET);
        std::fread(&audio_format, sizeof(audio_format), 1, wav_file);
        std::fclose(wav_file);

        // Check if file format is correct
        if (std::memcmp(magic, RIFF_MAGIC, sizeof(magic)) != 0 ||
            std::memcmp(format, WAVE_FORMAT, sizeof(format)) != 0 ||
            audio_format != PCM_AUDIO_FORMAT) {
            std::cerr << "No valid file format in file " << it->path() << std::endl;
            continue;
        }
        // Convert WAV file to MP3
        converter.add_file(it->path().string());
    }

    converter.wait_for_finish();

    return 0;
}
