#include <libraw/libraw.h>
#include <exiv2/exiv2.hpp>
#include <iostream>
#include <string>
#include <getopt.h>
#include <cmath>
#include <map>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// --- Metadata Helper ---
void inject_metadata(const std::string& srcPath, const std::string& destPath) {
    try {
        auto src = Exiv2::ImageFactory::open(srcPath);
        if (!src.get()) return;
        src->readMetadata();

        auto dest = Exiv2::ImageFactory::open(destPath);
        if (!dest.get()) return;

        dest->setExifData(src->exifData());
        dest->setIptcData(src->iptcData());
        dest->setXmpData(src->xmpData());
        dest->writeMetadata();
        std::cout << "[Metadata] Successfully copied to " << destPath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Metadata Error] " << e.what() << std::endl;
    }
}

// --- White Balance Presets ---
struct WB_Preset { float r, g, b; };
std::map<std::string, WB_Preset> wb_presets = {
    {"daylight",    {2.1, 1.0, 1.2}},
    {"cloudy",      {2.3, 1.0, 1.1}},
    {"tungsten",    {1.3, 1.0, 2.4}},
    {"fluorescent", {1.8, 1.0, 2.1}},
    {"flash",       {2.4, 1.0, 1.3}}
};

void print_help(char* name) {
    std::cout << "Usage: " << name << " [options] <RAW_FILE>\n"
              << "Options:\n"
              << "  -e, --exposure <f>   Exposure compensation in stops (default 0.0)\n"
              << "  -b, --bright <f>     Brightness multiplier (default 1.0)\n"
              << "  -w, --wb <mode>      camera, auto, daylight, cloudy, tungsten\n"
              << "  -n, --no-auto        Disable auto-brightness (don't stretch histogram)\n"
              << "  -f, --format <ext>   jpg or png (default jpg)\n"
              << "  -q, --quality <int>  JPG quality 1-100 (default 90)\n";
}

int main(int argc, char* argv[]) {
    float exposure = 0.0f;
    float brightness = 1.0f;
    bool auto_bright = true;
    std::string wb_mode = "camera";
    std::string format = "jpg";
    int quality = 90;

    static struct option long_options[] = {
        {"exposure", required_argument, 0, 'e'},
        {"bright",   required_argument, 0, 'b'},
        {"wb",       required_argument, 0, 'w'},
        {"no-auto",  no_argument,       0, 'n'},
        {"format",   required_argument, 0, 'f'},
        {"quality",  required_argument, 0, 'q'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "e:b:w:nf:q:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'e': exposure = std::stof(optarg); break;
            case 'b': brightness = std::stof(optarg); break;
            case 'w': wb_mode = optarg; break;
            case 'n': auto_bright = false; break;
            case 'f': format = optarg; break;
            case 'q': quality = std::stoi(optarg); break;
            default: print_help(argv[0]); return 1;
        }
    }

    if (optind >= argc) { print_help(argv[0]); return 1; }
    std::string inFile = argv[optind];
    std::string outFile = inFile + "_processed." + format;

    LibRaw processor;
    if (processor.open_file(inFile.c_str()) != LIBRAW_SUCCESS) return 1;
    if (processor.unpack() != LIBRAW_SUCCESS) return 1;

    // 1. Set White Balance
    if (wb_mode == "auto") {
        processor.imgdata.params.use_auto_wb = 1;
    } else if (wb_mode == "camera") {
        processor.imgdata.params.use_camera_wb = 1;
    } else if (wb_presets.count(wb_mode)) {
        processor.imgdata.params.user_mul[0] = wb_presets[wb_mode].r;
        processor.imgdata.params.user_mul[1] = wb_presets[wb_mode].g;
        processor.imgdata.params.user_mul[2] = wb_presets[wb_mode].b;
        processor.imgdata.params.user_mul[3] = wb_presets[wb_mode].g;
    }

    // 2. Set Exposure & Brightness
    if (exposure != 0.0f) {
        processor.imgdata.params.exp_correc = 1;
        // Shift is applied as a multiplier: 2^EV
        processor.imgdata.params.exp_shift = std::pow(2.0f, exposure);
    }
    processor.imgdata.params.bright = brightness;
    processor.imgdata.params.no_auto_bright = auto_bright ? 0 : 1;
    processor.imgdata.params.output_bps = 8; // 8-bit for STB

    // 3. Process to Memory
    std::cout << "[Process] Decoding sensor data..." << std::endl;
    if (processor.dcraw_process() != LIBRAW_SUCCESS) return 1;

    int err;
    libraw_processed_image_t* img = processor.dcraw_make_mem_image(&err);

    if (img && img->type == LIBRAW_IMAGE_BITMAP) {
        // 4. Save Pixels
        int success = 0;
        if (format == "png") {
            success = stbi_write_png(outFile.c_str(), img->width, img->height, img->colors, img->data, img->width * img->colors);
        } else {
            success = stbi_write_jpg(outFile.c_str(), img->width, img->height, img->colors, img->data, quality);
        }

        if (success) {
            std::cout << "[IO] Saved pixels to " << outFile << std::endl;
            // 5. Inject EXIF from Original
            inject_metadata(inFile, outFile);
        }
        processor.dcraw_clear_mem(img);
    }

    processor.recycle();
    return 0;
}
