# am33xx-readmacs

This reads some memory area in order to find out the MAC addresses of your AM33xx
SoC. Good for projects that don't use the built-in Ethernet controllers but need the
factory-built device IDs.

The memory layout information has been copied from u-boot and the MMAP code is
based on the Busybox devmem applet.
