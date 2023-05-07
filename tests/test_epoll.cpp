/*
 * @Author: Cui XiaoJun
 * @Date: 2023-05-07 14:20:13
 * @LastEditTime: 2023-05-07 15:00:05
 * @email: cxj2856801855@gmail.com
 * @github: https://github.com/SocialistYouth/
 */
#include <iostream>
#include <sys/epoll.h> // epoll, IO事件通知工具

using namespace std;

/*
epoll API 的核心概念是epoll实例----一个内核内的数据结构，可以看出两个列表的容器:
- interest list(epoll set): 进程已注册的要监视的文件描述符集合
- ready list: IO就绪的文件描述符集合(是interest list的子集[更精确的说, 是一组引用])

system call:
epoll_create()[创建一个epoll实例并返回引用该实例的文件描述符]
epoll_ctl()[添加文件描述符到interest list]
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout )[阻塞等待IO事件]
                                            [ready list]    [>0]      [阻塞的毫秒数]

*/
int main(int argc, char const *argv[])
{
    
    return 0;
}
