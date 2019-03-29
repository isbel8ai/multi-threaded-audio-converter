#include <iostream>
#include <fstream>
#include <strings.h>
#include <experimental/filesystem>
#include <lame/lame.h>
#include <fstream>
#include <cstring>
#include <fstream>

namespace fs = std::experimental::filesystem;

bool lameEncode(const std::string &input) {
    const size_t IN_SAMPLE_RATE = 44100; // default sample-rate
    const size_t PCM_SIZE = 8192;
    const size_t MP3_SIZE = 8192;
    const size_t LAME_GOOD = 5;
    int16_t pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[MP3_SIZE];
    const size_t bytes_per_sample = 2 * sizeof(int16_t); // stereo signal, 16 bits
    const std::string ext = {"mp3"};

    std::string output(input);
    output.replace(output.end() - ext.length(), output.end(), ext);
    std::ifstream wav;
    std::ofstream mp3;

    wav.exceptions(std::ifstream::failbit);
    mp3.exceptions(std::ifstream::failbit);
    try {
        wav.open(input, std::ios_base::binary);
        mp3.open(output, std::ios_base::binary);
    }
    catch (std::ifstream::failure &e) {
        std::cerr << "Error opening input/output file: " << std::strerror(errno) << std::endl;
        return false;
    }

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, IN_SAMPLE_RATE);
    lame_set_VBR(lame, vbr_default);
    lame_set_VBR_q(lame, LAME_GOOD);

    if (lame_init_params(lame) < 0) {
        wav.close();
        mp3.close();
        return false;
    }

    while (wav.good()) {
        int write = 0;
        wav.read(reinterpret_cast<char *>(pcm_buffer), sizeof(pcm_buffer));
        int read = wav.gcount() / bytes_per_sample;
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        mp3.write(reinterpret_cast<char *>(mp3_buffer), write);
    }

    wav.close();
    mp3.close();

    lame_close(lame);
    return true;
}

const unsigned char RIFF_CHUNK_ID[4] = {'R', 'I', 'F', 'F'};
const unsigned char WAVE_FORMAT_ID[4] = {'W', 'A', 'V', 'E'};
const unsigned short WAVE_FORMAT_PCM = 1;

typedef struct wav_header_t {
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM
    short num_channels;
    int sample_rate;
    int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth; // Number of bits per sample

    // Data
    char data_header[4]; // Contains "data"
    int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} wav_header;


bool convert(const std::string &pcm_path) {

    const int IN_SAMPLE_RATE = 44100;
    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;
    const int QUALITY_LEVEL = 5; // VBR quality good
    const std::string PCM_EXT = "wav";
    const std::string MP3_EXT = "mp3";

    short int pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[MP3_SIZE];

    std::string mp3_path(pcm_path);
    mp3_path.replace(mp3_path.end() - PCM_EXT.length(), mp3_path.end(), MP3_EXT);

    FILE *wav = fopen(pcm_path.c_str(), "rb");
    FILE *mp3 = fopen(mp3_path.c_str(), "wb");

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, IN_SAMPLE_RATE);
    lame_set_VBR(lame, vbr_default);
    lame_set_VBR_q(lame, QUALITY_LEVEL);

    if (lame_init_params(lame) < 0) {
        fclose(mp3);
        fclose(wav);
        return false;
    }

    int read, write;

    do {
        read = fread(pcm_buffer, 2 * sizeof(short int), PCM_SIZE, wav);
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(wav);
    return true;
}

int main(int argc, char *argv[]) {
    // Check the number of parameters
    if (argc != 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <directory>" << std::endl;
        return 1;
    }
    // Get directory from command line arguments
    char *directory = argv[1];
    // Check if directory exist
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
        std::ifstream input(it->path(), std::ios::binary);
        if (!input.is_open()) {
            std::cerr << "Unable to open file " << it->path() << std::endl;
            continue;
        }

        wav_header_t header{};
        input.read((char *) (&header), sizeof(header));
        input.close();
        // Check if file format is correct
        if (std::memcmp(header.riff_header, RIFF_CHUNK_ID, sizeof(header.riff_header)) != 0 ||
            std::memcmp(header.wave_header, WAVE_FORMAT_ID, sizeof(header.wave_header)) != 0 ||
            header.audio_format != WAVE_FORMAT_PCM) {
            std::cerr << "No valid file format in file " << it->path() << std::endl;
            continue;
        }
        convert(it->path());
    }

    return 0;
}
