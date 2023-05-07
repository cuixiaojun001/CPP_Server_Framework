/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-04 11:54:07
 * @LastEditTime: 2023-05-06 21:19:29
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test_fiber() {
	SYLAR_LOG_INFO(g_logger) << "test in fiber";
}
int main(int argc, char const* argv[]) {
	SYLAR_LOG_INFO(g_logger) << "main begin";
	sylar::Scheduler sc(1, true, "thr");
	int n = 2;
	while (n --) {
		SYLAR_LOG_INFO(g_logger) << "向任务队列中添加任务...";
		sc.schedule(&test_fiber);
	}
	sc.start();
	SYLAR_LOG_INFO(g_logger) << "scheduler";
	// sc.stop();
	SYLAR_LOG_INFO(g_logger) << "main over";
	return 0;
}
