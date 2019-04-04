#include <iostream>
#include <thread>
#include <unistd.h>
#include <lame/lame.h>
#include "Converter.h"

Converter::Converter() {
    max_thread_count = std::thread::hardware_concurrency();
    workers = new pthread_t[max_thread_count];
    sem_init(&files_sem, 0, 0);
    sem_init(&files_mutex, 0, 1);
    running = true;
    for (int i = 0; i < max_thread_count; ++i) {
        pthread_create(workers + i, nullptr, Converter::run_worker, this);
    }
}

Converter::~Converter() {
    wait_for_finish();
    delete[] workers;
}

void *Converter::run_worker(void *context) {
    auto *converter = (Converter *) context;

    while (converter->running) {
        // Wait for files in the queue
        sem_wait(&converter->files_sem);

        // Wait for blocking files queue
        sem_wait(&converter->files_mutex);
        std::string wav_path;
        if (!converter->files.empty()) {
            wav_path = converter->files.front();
            converter->files.pop();
        }
        // Unblock files queue
        sem_post(&converter->files_mutex);
        // Process file if path not empty
        if (!wav_path.empty()) {
            Converter::convert(wav_path);
        }
    }
    pthread_exit(nullptr);
}


void Converter::convert(std::string &wav_path) {
    short pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[MP3_SIZE];

    const std::string PCM_EXT = "wav";
    const std::string MP3_EXT = "mp3";
    std::string mp3_path(wav_path);
    mp3_path.replace(mp3_path.end() - PCM_EXT.length(), mp3_path.end(), MP3_EXT);

    FILE *wav_file = std::fopen(wav_path.c_str(), "rb");
    FILE *mp3_file = std::fopen(mp3_path.c_str(), "wb");

    // Initialize lame object
    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, IN_SAMPLE_RATE);
    lame_set_VBR(lame, vbr_default);
    lame_set_VBR_q(lame, QUALITY_LEVEL);

    if (lame_init_params(lame) < 0) {
        fclose(mp3_file);
        fclose(wav_file);
    }

    std::cout << "Converting file '" << wav_path << "'..." << std::endl;

    int read, write;
    // Encoding process
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
}


void Converter::add_file(const std::string &path) {
    // lock file
    sem_wait(&files_mutex);
    files.push(std::string(path));
    sem_post(&files_mutex);

    sem_post(&files_sem);
    std::cout << "Added file '" << path << "'" << std::endl;
}


void Converter::wait_for_finish() {
    // Wait until file queue is empty
    while (!files.empty()) sleep(1);

    // Unset run flag
    running = false;

    // Unblock
    for (int i = 0; i < max_thread_count; ++i) {
        sem_post(&files_sem);
    }

    // Destroy semaphores
    sem_destroy(&files_mutex);
    sem_destroy(&files_sem);
}
