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

/* see u-boot <board/ti/am335x/board.c> */
#define AM335X_CTRL_DEVICE_BASE 0x44e10600

/* copied from u-boot <arch/arm/include/asm/arch-am33xx/cpu.h> */
struct ctrl_dev {
  unsigned int padding[12];
  unsigned int macid0l;		/* offset 0x30 */
  unsigned int macid0h;		/* offset 0x34 */
  unsigned int macid1l;		/* offset 0x38 */
  unsigned int macid1h;		/* offset 0x3c */
};

int
main ()
{

  const long page_size = sysconf (_SC_PAGESIZE);

  const int devmemfd = open ("/dev/mem", O_RDONLY | O_SYNC);
  if (devmemfd == -1) {
    perror ("failed to open /dev/mem");
    return 1;
  }

  const off_t target = AM335X_CTRL_DEVICE_BASE;

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

  /* the bytes are ordered opposite to mac address display format */
  printf ("mac0:%08x%04x\n", htonl (dev->macid0h), htons (dev->macid0l));
  printf ("mac1:%08x%04x\n", htonl (dev->macid1h), htons (dev->macid1l));

  munmap (map_base, mapped_size);
  close (devmemfd);

  return 0;

}
