#!/bin/bash

sudo ./run.sh								\
	-n								\
	-k ~/src/app-elfloader/workdir/build/elfloader_qemu-x86_64	\
	-r ./rootfs							\
	/nr-softmodem							\
	-O gnb.sa.band78.106prb.rfsim.conf				\
	--rfsim								\
	--sa