![Alt Text](prawcessor500t.png "Prawcessor logo")

# pRAWcessor

cli tool to convert raw files to jpg/png 

Based on exiv2 and libraw.  

Meant to mimic ufraw, but with support for current RAW types (CR3, etc)

## requirements

If you haven't already, install:

    apt install libraw-dev libexiv2-dev

...and then...

    wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

...and hopefully you have a c++ compiler installed, where you can run:

    g++ prawcessor.cpp -o prawcessor -lraw -lexiv2 

## usage

```
$ ./prawcessor 
Usage: ./prawcessor [options] <RAW_FILE>
Options:
  -e, --exposure <f>   Exposure compensation in stops (default 0.0)
  -b, --bright <f>     Brightness multiplier (default 1.0)
  -w, --wb <mode>      camera, auto, daylight, cloudy, tungsten
  -n, --no-auto        Disable auto-brightness (don't stretch histogram)
  -f, --format <ext>   jpg or png (default jpg)
  -q, --quality <int>  JPG quality 1-100 (default 90)
```
```
$ ./prawcessor --exposure 1.5 --wb cloudy --no-auto --format jpg my_photo.CR3
```

etc.
