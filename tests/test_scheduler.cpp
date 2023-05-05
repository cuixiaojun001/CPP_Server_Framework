/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-04 11:54:07
 * @LastEditTime: 2023-05-05 00:19:59
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test_fiber() {
	SYLAR_LOG_INFO(g_logger) << "test in fiber";
	static int s_count = 5;
	sleep(1);
	while(--s_count >= 0) {
		sylar::Scheduler::GetThis()->schedule(&test_fiber);
	}
}
int main(int argc, char const* argv[]) {
	SYLAR_LOG_INFO(g_logger) << "main begin";
	sylar::Scheduler sc(2, false, "thr");
	sc.start();
	sc.schedule(&test_fiber);
	SYLAR_LOG_INFO(g_logger) << "scheduler";
	sc.stop();
	SYLAR_LOG_INFO(g_logger) << "main over";
	return 0;
}
