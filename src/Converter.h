#ifndef CINEMO_TEST_CONVERTER_H
#define CINEMO_TEST_CONVERTER_H

#include <queue>
#include <pthread.h>
#include <semaphore.h>

const int IN_SAMPLE_RATE = 44100;
const int PCM_SIZE = 8192;
const int MP3_SIZE = 8192;
const int QUALITY_LEVEL = 5; // VBR quality good

class Converter {
private:
    /** List of files to be converted*/
    std::queue<std::string> files;
    /** Mutex to control critical section to access files queue*/
    sem_t files_mutex{};
    /** Semaphore to control number of files to process*/
    sem_t files_sem{};
    /** Workers to process files*/
    pthread_t *workers;
    /** Maximum possible number of threads to be use*/
    unsigned int max_thread_count;
    /** Run flag*/
    bool running;

public:

    /**
     * Default constructor
     */
    Converter();

    /**
     * Destructor
     */
    ~Converter();

    /**
     *
     * @param context: pointer to converter
     * @return
     */
    static void *run_worker(void *context);

    /**
     *  Convert WAV file in path to MP3 format
     * @param wav_path: path to file to be converted
     */
    static void convert(std::string &wav_path);

    /**
     * Add a new file to bi converted
     * @param wav_path: path to the file to be processed
     */
    void add_file(const std::string& path);

    /**
     * Wait until all files are processed
     */
    void wait_for_finish();
};


#endif //CINEMO_TEST_CONVERTER_H
