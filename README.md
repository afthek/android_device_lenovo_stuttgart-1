android_device_lenovo_stuttgart, for Lenovo K860/K860i (stuttgart)
====

fork from Mr. Scue

Now I'm trying to port CM11 for stuttgart, hope I can make it.


2014年 8月26日 星期二 22时06分20秒 CST






** build for stuttgart:**

        . build/envsetup.sh
        breakfast stuttgart
        cd device/lenovo/stuttgart/
        mkdir tmp/
        ln -s /path/to/stack_system_dir tmp/system # NOTE: please replace '/path/to/stock_rom_system_dir'
        ./extract-files.sh
        croot
        brunch stuttgart > out.txt 2>&1 &
        tail -f out.txt

** pack to a szb file(after built successfully):**

        cd device/lenovo/stuttgart/tools/
        ./repackszb.sh

