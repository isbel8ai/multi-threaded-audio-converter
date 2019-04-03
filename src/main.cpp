#include <iostream>
#include <cstring>
#include <dirent.h>
#include "Converter.h"

const char RIFF_MAGIC[4] = {'R', 'I', 'F', 'F'};
const int FORMAT_OFFSET = 8;
const char WAVE_FORMAT[4] = {'W', 'A', 'V', 'E'};
const int AUDIO_FORMAT_OFFSET = 20;
const short PCM_AUDIO_FORMAT = 1;

int main(int argc, char *argv[]) {

    std::cout << argv[0] << std::endl;

    // Check the number of parameters
    if (argc != 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <directory>" << std::endl;
        return 1;
    }

    // Get directory from command line arguments
    char *directory = argv[1];

    // Check if directory exist and if it is a directory
    DIR* dir_ptr = opendir(directory);
    if (dir_ptr == nullptr) {
        // Notify directory does not exist
        std::cerr << "Not found directory '" << directory << "'" << std::endl;
        return 2;
    }

    // Create converter object
    Converter converter;

    struct dirent * entry;
    // Iterate over all items in the directory
    while ((entry = readdir(dir_ptr)) != nullptr) {
        auto filename = entry->d_name;
        // Select only *.wav files to be processed
        if (strcasecmp(strrchr(filename, '.'), ".wav") != 0) {
            continue;
        }

        std::string file_path = std::string(directory) + "/" + std::string(filename);

        FILE *wav_file = fopen(file_path.c_str(), "rb");
        if (!wav_file) {
            std::cerr << "Unable to open file " << file_path << std::endl;
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
            std::cerr << "No valid file format in file " << file_path << std::endl;
            continue;
        }
        // Convert WAV file to MP3
        converter.add_file(std::string(file_path));
    }

    closedir(dir_ptr);

    converter.wait_for_finish();

    return 0;
}
