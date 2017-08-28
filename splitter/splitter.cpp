#include <vector>
#include <limits>
#include <iostream>
#include <cstdio>
#include <cstdint>

template<typename T>
class SilenceFinder
{
    public:
        SilenceFinder(T * data, uint32_t size, uint32_t samples) : sBegin(0), d(data), s(size), samp(samples), status(Undefined) {}

        std::vector<std::pair<uint32_t, uint32_t>> find(const T threshold, const uint32_t window) {
            auto r = findSilence(d, s, threshold, window);
            regionsToTime(r);
            return r;
        }

    private:
        enum Status {
            Silent, Loud, Undefined
        };

        void toggleSilence(Status st, uint32_t pos, std::vector<std::pair<uint32_t, uint32_t>> & res) {
            if (st == Silent) {
                if (status != Silent) sBegin = pos;
                status = Silent;
            }
            else {
                if (status == Silent) res.push_back(std::pair<uint32_t, uint32_t>(sBegin, pos));
                status = Loud;
            }
        }

        void end(Status st, uint32_t pos, std::vector<std::pair<uint32_t, uint32_t>> & res) {
            if ((status == Silent) && (st == Silent)) res.push_back(std::pair<uint32_t, uint32_t>(sBegin, pos));
        }

        static T delta(T * data, const uint32_t window) {
            T min = std::numeric_limits<T>::max(), max = std::numeric_limits<T>::min();
            for (uint32_t i = 0; i < window; ++i) {
                T c = data[i];
                if (c < min) min = c;
                if (c > max) max = c;
            }
            return max - min;
        }

        std::vector<std::pair<uint32_t, uint32_t>> findSilence(T * data, const uint32_t size, const T threshold, const uint32_t win) {
            std::vector<std::pair<uint32_t, uint32_t>> regions;
            uint32_t window = win;
            uint32_t pos = 0;
            Status s = Undefined;
            while ((pos + window) <= size) {
                if (delta(data + pos, window) < threshold) s = Silent;
                else s = Loud;
                toggleSilence(s, pos, regions);
                pos += window;
            }
            if (delta(data + pos, size - pos) < threshold) s = Silent;
            else s = Loud;
            end(s, pos, regions);
            return regions;
        }

        void regionsToTime(std::vector<std::pair<uint32_t, uint32_t>> & regions) {
            for (auto & r : regions) {
                r.first /= samp;
                r.second /= samp;
            }
        }

        T * d;
        uint32_t sBegin, s, samp;
        Status status;
};

struct WavHdr_t
{
    /* RIFF Chunk Descriptor */
    uint8_t         RIFF[4];        // RIFF Header Magic header
    uint32_t        ChunkSize;      // RIFF Chunk Size
    uint8_t         WAVE[4];        // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t         fmt[4];         // FMT header
    uint32_t        Subchunk1Size;  // Size of the fmt chunk
    uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t        bytesPerSec;    // bytes per second
    uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t         Subchunk2ID[4]; // "data"  string
    uint32_t        Subchunk2Size;  // Sampled data length
};

int main(int argc, char* argv[])
{
    uint8_t header[74];
    FILE* fin = fopen(argv[1], "rb");
    fread(header, sizeof(uint8_t), 74, fin);
    int16_t* sound_buffer;
    uint32_t data_size;

    fread(&data_size, sizeof(data_size), 1, fin);
    data_size /= sizeof(int16_t);
    sound_buffer = new int16_t[data_size];
    fread(sound_buffer, sizeof(int16_t), data_size, fin);

    SilenceFinder<int16_t> finder(sound_buffer, data_size, 16000);
    int16_t threshold = 10;
    uint32_t scanWindow = 10;
    auto res = finder.find(threshold, scanWindow);
    // and output the silent regions
    for (auto r : res) {
        std::cout << r.first << " " << r.second << std::endl;
    }

    fclose(fin);
    delete sound_buffer;
    return 0;
}
