#!/bin/bash

extract_deps()
{
	for l in $(ldd "$1" | grep -o '/[^ ]*'); do
		echo "Copying $l ..."
		cp --dereference --parents "$l" ./rootfs
	done
}

rm -rf ./rootfs
mkdir ./rootfs
cp ./gnb.sa.band78.106prb.rfsim.conf ./rootfs
cp ../cmake_targets/ran_build/build/nr-softmodem ./rootfs
extract_deps ./rootfs/nr-softmodem
cp ../cmake_targets/ran_build/build/libdfts.so ./rootfs/lib
extract_deps ./rootfs/lib/libdfts.so
cp ../cmake_targets/ran_build/build/libldpc.so ./rootfs/lib
extract_deps ./rootfs/lib/libldpc.so
cp ../cmake_targets/ran_build/build/libparams_libconfig.so ./rootfs/lib
extract_deps ./rootfs/lib/libparams_libconfig.so
cp ../cmake_targets/ran_build/build/librfsimulator.so ./rootfs/lib
extract_deps ./rootfs/lib/librfsimulator.so