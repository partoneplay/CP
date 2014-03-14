#ifndef XCP_H
#define XCP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"

#define EXT_NAME ".cxc"
#define BUF_SIZE 1024

// filename max length
#define NAME_MAX 255
// path max length
#define PATH_MAX 4096

#define RET_YES 1
#define RET_NO 0
#define RET_ERROR -1

#define X_NONE 0
#define X_ENCRYPT 1
#define X_DECRYPT 2

#endif