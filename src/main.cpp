#include <iostream>
#include <experimental/filesystem>
#include <lame/lame.h>
#include <cstring>

namespace fs = std::experimental::filesystem;

bool convert(const std::string &wav_path) {

    const int IN_SAMPLE_RATE = 44100;
    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;
    const int QUALITY_LEVEL = 5; // VBR quality good
    const std::string PCM_EXT = "wav";
    const std::string MP3_EXT = "mp3";

    short pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[MP3_SIZE];

    std::string mp3_path(wav_path);
    mp3_path.replace(mp3_path.end() - PCM_EXT.length(), mp3_path.end(), MP3_EXT);

    FILE *wav_file = std::fopen(wav_path.c_str(), "rb");
    FILE *mp3_file = std::fopen(mp3_path.c_str(), "wb");

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, IN_SAMPLE_RATE);
    lame_set_VBR(lame, vbr_default);
    lame_set_VBR_q(lame, QUALITY_LEVEL);

    if (lame_init_params(lame) < 0) {
        fclose(mp3_file);
        fclose(wav_file);
        return false;
    }

    int read, write;

    do {
        read = fread(pcm_buffer, 2 * sizeof(short int), PCM_SIZE, wav_file);
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        fwrite(mp3_buffer, write, 1, mp3_file);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3_file);
    fclose(wav_file);
    return true;
}

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
    // Iterate over all items in the directory
    for (auto it = fs::directory_iterator(directory); it != fs::directory_iterator(); ++it) {
        // Select only *.wav files to be processed
        if (!fs::is_regular_file(it->path()) || strcasecmp(it->path().extension().c_str(), ".wav") != 0) {
            continue;
        }
        std::cout << it->path() << std::endl;
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
        convert(it->path());
    }

    return 0;
}
