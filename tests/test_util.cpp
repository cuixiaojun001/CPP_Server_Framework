#include "sylar/sylar.h"
#include <assert.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
void test_assert() {
    SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10, 2, "   ");
    SYLAR_ASSERT1(false);
}

int main() {
    test_assert();
    return 0;
}