# Running the nr-softmodem in Unikraft

Build the nr-softmodem
```bash
cd cmake_builds
./build_oai --gNB
```

Generate a rootfs
```bash
cd unikraft
./gen-rootfs.sh
```

Run the gNB
```bash
cd unikraft
./run-nr-softmodem.sh
```