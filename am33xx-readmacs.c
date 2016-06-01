#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef AM33XX_CTRL_DEVICE_BASE
/* see u-boot <board/ti/am335x/board.c> */
#define AM33XX_CTRL_DEVICE_BASE 0x44e10600
#endif

/* copied from u-boot <arch/arm/include/asm/arch-am33xx/cpu.h> */
struct ctrl_dev {
  unsigned int padding[12];
  unsigned int macid0l;		/* offset 0x30 */
  unsigned int macid0h;		/* offset 0x34 */
  unsigned int macid1l;		/* offset 0x38 */
  unsigned int macid1h;		/* offset 0x3c */
};

int
main (const int argc, const char *argv[])
{

  const long page_size = sysconf (_SC_PAGESIZE);

  const int devmemfd = open ("/dev/mem", O_RDONLY | O_SYNC);
  if (devmemfd == -1) {
    perror ("failed to open /dev/mem");
    return 1;
  }

  const off_t target = AM33XX_CTRL_DEVICE_BASE;

  const unsigned offset = target & (page_size - 1);
  const bool multi_page = (offset + sizeof (struct ctrl_dev)) > page_size;
  const long mapped_size = page_size << (multi_page ? 1 : 0);

  printf (
    "page_size=0x%04lx, "
    "offset=0x%04x, "
    "mapped_size=0x%04lx\n",
    page_size,
    offset,
    mapped_size
  );

  void *const map_base = mmap (
    NULL,
    mapped_size,
    PROT_READ,
    MAP_SHARED,
    devmemfd,
    target & ~(off_t)(page_size - 1)
  );

  if (map_base == MAP_FAILED) {
    perror ("failed to mmap");
    close (devmemfd);
    return 1;
  }

  struct ctrl_dev *const dev = (struct ctrl_dev *) (
    ((uint8_t *) map_base) + offset
  );

  char mac0[13], mac1[13];

  /* the bytes are ordered opposite to mac address display format */
  snprintf (
    mac0,
    sizeof (mac0),
    "%08x%04x",
    htonl (dev->macid0h),
    htonl (dev->macid0l)
  );

  snprintf (
    mac1,
    sizeof (mac1),
    "%08x%04x",
    htonl (dev->macid1h),
    htonl (dev->macid1l)
  );

  if (argc < 2) {
    printf ("mac0:%s\n", mac0);
    printf ("mac1:%s\n", mac1);
  } else {
    const char arg = argv[1][0];
    printf ("%s\n", arg != '1' ? mac0 : mac1);
  }

  munmap (map_base, mapped_size);
  close (devmemfd);

  return 0;

}
