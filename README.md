![Prawcessor Logo](prawcessor500t.png)

# pRAWcessor

A command-line tool for converting RAW camera images to JPG/PNG with advanced color correction.

Built on top of [libraw](https://github.com/libraw/libraw) and [exiv2](https://github.com/exiv2/exiv2) to mimic the functionality of `ufraw` while adding support for modern RAW formats like Sony CR3.

## Features

- **White Balance Control**: Auto, camera WB, or custom presets (daylight, cloudy, tungsten, fluorescent, flash)
- **Exposure Adjustment**: Fine-tune exposure compensation in stops
- **Brightness Adjustment**: Multiplier-based brightness control
- **Multiple Output Formats**: JPG (with quality control) or PNG
- **EXIF Metadata Preservation**: Copies EXIF, IPTC, and XMP data from source file

## Requirements

### System Dependencies

```bash
apt install libraw-dev libexiv2-dev
```

### Runtime Dependencies

- [libraw](https://github.com/libraw/libraw) library (from package)
- [libexiv2](https://github.com/exiv2/exiv2) library (from package)
- [stb_image_write](https://github.com/nothings/stb) header file (manual)

### Installing STB Image Write

```bash
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h -O stb_image_write.h
```

## Building

```bash
g++ -o prawcessor prawcessor.cpp -lraw -lexiv2 -I.
```

## Usage

```bash
./prawcessor [options] <RAW_FILE>
```

### Command Line Options

| Short | Long         | Description                                      | Default   |
|-------|--------------|--------------------------------------------------|-----------|
| -e    | --exposure   | Exposure compensation in stops                   | 0.0       |
| -b    | --bright     | Brightness multiplier                            | 1.0       |
| -w    | --wb         | White balance mode                              | camera    |
| -n    | --no-auto    | Disable auto-brightness                         | false     |
| -f    | --format     | Output file format                              | jpg       |
| -q    | --quality    | JPG quality (1-100)                             | 90        |

### Examples

```bash
# Basic conversion to JPG
./prawcessor my_photo.CR3

# Convert with custom white balance (cloudy)
./prawcessor -w cloudy my_photo.CR3

# Full pipeline with exposure + white balance
./prawcessor --exposure 1.5 --wb cloudy --no-auto --format jpg my_photo.CR3

# Export as PNG at full quality
./prawcessor -f png -q 100 my_photo.CR3
```

## White Balance Presets

| Preset      | Description                      | RGB Multiplier |
|-------------|----------------------------------|----------------|
| camera      | Use camera's embedded WB         | -              |
| auto        | Auto-detect and apply WB         | -              |
| daylight    | Daylight lighting conditions     | 2.1, 1.0, 1.2  |
| cloudy      | Cloudy overcast conditions       | 2.3, 1.0, 1.1  |
| tungsten    | Incandescent/tungsten lighting   | 1.3, 1.0, 2.4  |
| fluorescent | Fluorescent lighting             | 1.8, 1.0, 2.1  |
| flash       | Flash photography                | 2.4, 1.0, 1.3  |

## Supported RAW Formats

- CR3 (Sony)
- NEF (Nikon)
- ARW (Canon)
- CR2 (Canon)
- RAF (Fujifilm)
- RAW (Panasonic/Lumix)
- And others supported by libraw

## Output

The tool saves output to:
```
{input_file}_processed.{format}
```

Example: `my_photo.CR3` → `my_photo_processed.jpg`

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Known Issues

- Some RAW formats may not be fully supported due to libraw limitations
- HDR files are not supported
- Very large images may take several minutes to process

## Contributing

Pull requests are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

---

Built with ❤️ for camera enthusiasts.
