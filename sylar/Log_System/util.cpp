#include "util.h"

namespace sylar {
    pid_t GetThreadId() {
         return syscall(SYS_getppid);
    }

    uint32_t GetFiberId() {
        return 0;
    }
}
