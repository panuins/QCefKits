#/bin/bash
kernelInfo_ms64=`uname -a | grep -i microsoft | grep 64`
kernelInfo_mingw64=`uname -a | grep -i mingw64`
kernelInfo_ms=`uname -a | grep -i microsoft`
kernelInfo_mingw=`uname -a | grep -i mingw`
kernelInfo_amd64=`uname -a | grep -i amd64`
kernelInfo_x86_64=`uname -a | grep -i x86_64`
kernelInfo_arm64=`uname -a | grep -i arm64`
kernelInfo_aarch64=`uname -a | grep -i aarch64`
if [[ "$kernelInfo_ms64" != "" ]];
then
    echo "win10 x64_64 subsystem detected"
	sh initProject_win64.sh
elif [[ "$kernelInfo_mingw64" != "" ]];
then
    echo "mingw64 detected"
	sh initProject_win64.sh
elif [[ "$kernelInfo_ms" != "" ]];
then
    echo "win10 subsystem detected"
	sh initProject_win32.sh
elif [[ "$kernelInfo_mingw" != "" ]];
then
    echo "mingw detected"
	sh initProject_win32.sh
elif [[ "$kernelInfo_amd64" != "" ]];
then
    echo "linux amd64 system detected"
	sh initProject_linux_amd64.sh
elif [[ "$kernelInfo_x86_64" != "" ]];
then
    echo "linux x86_64 system detected"
	sh initProject_linux_amd64.sh
elif [[ "$kernelInfo_arm64" != "" ]];
then
    echo "linux arm64 system detected"
	sh initProject_linux_arm64.sh
elif [[ "$kernelInfo_aarch64" != "" ]];
then
    echo "linux arm64 system detected"
	sh initProject_linux_arm64.sh
else
	echo "detect system failed, please run initProject_*.sh manually"
fi
