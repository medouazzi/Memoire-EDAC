#ifndef EDAC_IOCTL_H
#define EDAC_IOCTL_H

#include <linux/ioctl.h>

#define EDAC_GETMEMORYSIZE _IOR('E', 1, size_t)
#define EDAC_SETMEMORYSIZE _IOW('E', 2, size_t)
#define EDAC_FAULT         _IOW('E', 3, int)
#define EDAC_PERIOD _IOW('E', 4, int)

#endif /* EDAC_IOCTL_H */

