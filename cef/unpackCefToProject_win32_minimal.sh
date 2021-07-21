#/bin/sh
tar -jxf cef_binary_90.6.7+g19ba721+chromium-90.0.4430.212_windows32_minimal.tar.bz2
mv -f cef_binary_90.6.7+g19ba721+chromium-90.0.4430.212_windows32_minimal Windowsi386
cp -f cef_dll_wrapper.pro Windowsi386/libcef_dll
cd ..
