#/bin/sh
tar -jxf cef_binary_90.6.7+g19ba721+chromium-90.0.4430.212_windows64.tar.bz2
mv -f cef_binary_90.6.7+g19ba721+chromium-90.0.4430.212_windows64 Windowsx86_64
cp -f cef_dll_wrapper.pro Windowsx86_64/libcef_dll
cd ..
