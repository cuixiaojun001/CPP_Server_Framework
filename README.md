# sylar

## 开发环境

Ubuntu20.04
gcc 9.4.0
cmake

## 项目路径

bin -- 存放二进制
build -- 中间文件路径
cmake -- cmake函数文件夹
CMakeLists.txt -- cmake的定义文件
lib -- 库的输出路径
Makefile
sylar -- 源代码路径
tests -- 测试代码路径

## 日志系统

1.

Log4J

```cpp
Logger(定义日志类别)
   |
   | ------Formatter(日志格式)
   |
Appender(日志输出地方)
```

什么是日志系统?
Log像一台全天24小时开启的摄像头，记录服务器上各种重要事务的状态，有了Log，才可以在事后快速的知道出现问题的所在。例如用户在登录失败时，Log记录下来失败时输入的账号。
什么是Log4J?
全名是Apache Log4J, 是Java程式语言的Log套件(框架)，提供方便的日志记录。

## 协程库封装

## socket函数库

## http协议开发

## 分布协议

## 推荐系统

# 开发过程中使用到的技术

## C++ Template
**泛型程序设计**：所谓“泛型”，指的是算法只要实现一遍，就能适用于多种数据类型。能减少重复代码的反复编写。
### 函数模板
所谓函数模板，旨在建立一个通用函数，他所用到的数据类型(包括返回值类型，参数类型，局部变量类型)可以用一个临时的标识符来替代，在发生函数调用时逆向推导出真正的数据类型。
```cpp
template<typename T1, typename T2, ···> 返回值类型 函数名(形参列表) {
    //在函数体中可以使用类型参数
}
```
`template`是定义函数模板的关键字，它后面紧跟尖括号`<>`
`typename`是声明具体类型参数的关键字, 可以使用`class`关键字代替, 没有任何区别。
### 类模板
类模板的目的同样是把数据的类型参数化
类外定义成员函数同样需要带上模板头(除了template后面要跟类型参数, 类名后也要加类型参数, 只是不加typename关键字)
#### 使用类模板创建对象
创建对象时需要具体指明数据类型
## C++ STL
### STL迭代器iterator
1. 前向迭代器
2. 双向迭代器
3. 随机访问迭代器
#### 迭代器的四种定义方式
|迭代器定义方式|具体格式|
| ------------ | -------|
|正向迭代器|容器类名::iterator 迭代器名|
|正向常量迭代器| 容器类名::const_iterator 迭代器名|
|反向迭代器|容器类名::reverse_iterator 迭代器名|
|常量反向迭代器|容器类名::reverse_iterator 迭代器名|

`*迭代器名`表示迭代器指向的元素

### STL序列性容器
`array, list, vector, deque, forward_list`
不会对存储的元素排序，按照存储的顺序排列
#### array
- 迭代器: 随机访问迭代器
#### vector
- 迭代器: 随机访问迭代器
##### emplace_back()和push_back()的区别
功能逻辑相同，区别在于底层实现机制不同：
push_back() 向容器尾部添加元素前，首先创建这个元素，再将这个元素拷贝或移动到容器中(优先选择移动构造, 如果是拷贝构造的话，事后会自行销毁先前创建的对象); 而emplace_back()在实现的时候直接在容器尾部添加新元素。
##### insert() 和 emplace() 的区别
emplace() 在插入元素时，是在容器的指定位置直接构造元素，而不是先单独生成，再将其复制（或移动）到容器中。
#### deque
- 迭代器: 随机访问迭代器
#### list
- 迭代器: 

## C++ 11

### shared\_ptr 智能指针

在实际的C++开发中，我们经常会碰到程序突然崩溃、所使用内存越来越大，最终不得不重启等问题，这些问题往往是由于内存资源管理不当造成的。
比如：

*   有些内存资源已经释放，但指向它的指针没有改变指向(成为了野指针)，并且后续还在使用。
    *   出现野指针的常见情况：

        *   使用未初始化的指针

        ```cpp
        #include <iostream>
        using namespace std;
        int main() {
            int* p;
            cout << *p << endl; //编译通过，运行时出错
        }
        ```

        *   指针所指对象已经消亡(生命周期结束)

        ```cpp
        #include <iostream>
        using namespace std;

        int* retAddr() {
            int num = 10;
            return &num;
        }

        int main() {
            int* p = NULL;
            p = retAddr();
            cout << &p << endl;
            cout << *p << endl; // runtime error：load of null pointer of type 'int'
        }
        ```

        *   指针释放后之后未置空
*   有些内存资源已经释放，后续还在尝试释放(重复释同一块内存会导致程序运行崩溃)。
*   没有即使释放不再使用的内存资源，造成内存泄漏，程序占用内存资源越来越多。

C++11新标准增添了unique\_ptr、shared\_ptr 以及 weak\_ptr 这 3 个智能指针来实现堆内存的自动回收。
**什么是智能指针？**
所谓智能指针，从字面意思上来看就是“智能的”指针。其使用方法和普通指针相似，但其在适当时机可以自动的释放分配的内存。使用智能指针可以很好的避免“因为忘记释放而导致的内存泄露”的问题

> C++智能指针底层采用引用计数的方式实现的。简单的理解，智能指针在申请堆内存时，会为其配备一个整型值（初始值为1），每当有新对象使用该堆内存时，该整型值+1; 反之，每当使用此堆内存的对象释放时，该整型值-1。当堆空间对应的整型值为0时，即表明不再有对象使用它，该堆空间就会被释放掉。

**shared\_ptr\<T> 定义位于\<memory>头文件中, 并位于std命名空间中。**

shared\_ptr 和 unique\_ptr, weak\_ptr 不同点在于：
多个shared\_ptr智能指针可以同时指向同一块堆内存空间, 由于其在实现上采用引用计数的机制，即便有一个shared\_ptr智能指针放弃了对堆内存的“使用权”(引用计数-1), 也不会影响其他指向同一块堆内存的shared\_ptr智能指针（只有引用计数为 0 时，堆内存才会被自动释放）。

## std::function 和 std::bind

`std::function`是通用多态函数封装器。`std::function`的实例能存储、复制及调用任何可调用目标——— 函数，`lambda`表达式，`bind`表达式或者其他函数对象，还有指向成员函数指针和指向数据成员指针。
**用途:**   可以实现函数回调，即在运行时指定要调用的函数，而不是在编译时固定下来。

## std::transform

在指定的范围内应用于给定的操作，并将结果存储在指定的另一个范围内。

## Lambda表达式
通常用于传递函数对象作为参数，以及定义函数对象
```cpp
[capture list] (parameters) -> return_type { function body }
```
- capture list：捕获列表，用于捕获一些外部变量，可以是值传递或引用传递。捕获列表可以为空。
- parameters：参数列表，类似于函数参数列表，可以为空。
- return_type：返回类型，可以省略，编译器会自动推断。
- function body：函数体，类似于普通函数的函数体。

## &#x20;npos --> size\_t的最大值

```cpp
std::string::npos
```

## Boost库函数

## Yaml-CPP API:
1. `YAML::Load`: 从字符串或文件中读取YAML文档并解析它。返回一个YAML::Node对象。
2. `YAML::Node`: 一个YAML文档的节点。可以使用下标运算符或成员函数来访问节点。节点可以是标量、序列或映射。
3. `YAML::Node::Type()`: 返回节点的类型（标量、序列或映射）。
4. `YAML::Node::IsNull()`: 返回节点是否为NULL。
5. `YAML::Node::as<>`: 返回节点的值。可以使用不同类型的模板参数来获取节点的不同类型值，如as<int>()、as<double>()、as<std::string>()等。
6. `YAML::Node::size()`: 返回序列节点的元素个数。
7. `YAML::Node::begin()`和`YAML::Node::end()`: 返回序列节点的起始和结束迭代器。
8. `YAML::Node::push_back()`: 将一个值添加到序列节点的末尾。
9. `YAML::Node::remove()`: 从序列节点中删除指定的元素。
10. `YAML::Node::insert()`: 在映射节点中插入一个键值对。
11. `YAML::Node::remove()`: 从映射节点中删除指定的键值对。
12. `YAML::Node::begin()`和`YAML::Node::end()`: 返回映射节点的起始和结束迭代器。
13. `YAML::Emitter`: 用于将YAML数据生成为字符串或文件。提供了一组操作符，用于生成不同类型的节点。可以使用流操作符将节点写入流。
14. `YAML::Parser`: 在一个输入流中解析YAML文档。提供了一组回调函数，以处理不同类型的节点。每当解析器遇到一个新节点时，都会调用相应的回调函数。


## C++ 常见软件注释规范Doxygen

## 宏定义(带参宏定义)

无参数宏定义的格式为：`#define 标识符 替换列表`

*   使用标识符表示一常量

宏定义不是语句，是预处理指令，不能有`;`。
续行符`\`后直接按`Enter`换行，**不能**含有包括空格在内的任何字符，否则是错误的宏定义形式。
带参数的宏定义格式为：`#define 标识符(参数1,参数2,...,参数n) 替换列表`
删除宏定义的格式为：`#undef 标识符`

### 带参宏定义VS函数调用

1.  调用的发生时间：
    *   在源程序进行编译之前，即预处理阶段进行宏替换；
    *   函数调用则发生在程序运行期间。

2.  参数类型检查
    *   函数参数类型检查严格。程序在编译阶段，需要检查实参与形参个数是否相等及类型是否匹配或兼容，若有问题则会编译不通过。
    *   在预处理阶段，对带参宏调用中的参数不做检查。即宏定义时不需要指定参数类型，既可以认为这是宏的优点，即适用于多种数据类型，又可以认为这是宏的一个缺点，即类型不安全。故在宏调用时，需要程序设计者自行确保宏调用参数的类型正确。

3.  参数是否需要空间
    *   函数调用时，需要为形参分配空间，并把实参的值复制一份赋给形参分配的空间中。
    *   而宏替换，仅是简单的文本替换，且替换完就把宏名对应标识符删除掉，即不需要分配空间。

4.  执行速度
    *   函数在编译阶段需要检查参数个数是否相同、类型等是否匹配等多个语法，而宏替换仅 是简单文本替换，不做任何语法或逻辑检查。

    *   函数在运行阶段参数需入栈和出栈操作，速度相对较慢。

5.  代码长度
    *   由于宏替换是文本替换，即如果需替换的文本较长，则替换后会影响代码长度；而函数不会影响代码长度。

**故使用较频繁且代码量较小的功能，一般采用宏定义的形式，比采用函数形式更合适。** 前面章节频繁使用的getchar()，准确地说，是宏而非函数。

```c
#define getchar() getc(stdin)
```

### 关于#和#\#

**#的功能是 将其后面的宏参数进行字符串化操作。**

## 枚举LogLevel

**为什么在类中可以直接通过`类名::enum值`?**
该枚举是一个常量，在编译的时候已经放入到了常量区。调用的时候不需要枚举的类型也能调用。

## typedef in class

在类内部typedef，该类型别名的作用域仅在类内，和变量，函数一样具有作用域
该定义在该类的子类中可以得到继承，可以在子类中使用该新类型定义新变量和函数。

## 带默认形参的函数

1.有默认值的形参必须在形参列表的最后: 函数调用中,实参与形参按从左到右的顺序建立对应关系
2.相同作用域内,不允许在同一个函数的多个声明中对同一个参数的默认值重复定义,值相同也不行
3.函数在定义之前有原型声明,默认形参值在原型声明中给出,定义中为了清晰可用注释形式/\* \*/

## std::enable\_shared\_from\_this (C++ 11)

一般来说，不建议函数直接返回this指针，如果返回的this指针被存到外部的一个局部/全局变量，当对象被析构后，外部变量并不能知道指针指向的对象已经被析构了，此时外部使用了这个指针就会发生程序崩溃，这种情况，既要像指针操作对象一样，又能够安全的析构对象，就应该使用智能指针。

std::enable\_shared\_from\_this 是一个模板类，子类继承enable\_shared\_from\_this模板类之后，通过shared\_from\_this() 成员函数来获取目标类型。

# test.cpp时序图

## new 直接初始化(direct initialization)

1.  创建logger对象
2.  为logger对象的属性m\_formatter成员分配内存，并执行初始化->init()
3.  init() 解析\*\*"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T\[%p]%T\[%c]%T%f:%l%T%m%n"\*\*
4.  为logger创建标准输出appender
5.  执行StdoutAppender的构造函数
6.  局部变量LogEventWrap构造函数动态创建一个LogEvent对象, 超出生命周期时调用LogEventWrap的析构函数: 成员属性m\_event的m\_logger调用log函数, 传入event级别和event
7.  m\_logger遍历他的appender,调用每一个appender的log(), 把自己, 日志级别, 日志事件传入
8.  m\_appender调用m\_formatter的format()
9.  m\_formatter遍历每一个m\_item,调用对应的format

    logger调用StdoutAppender的log
    它std::cout 输出 继承自LogAppender的类LogFormatter成员变量m\_formatter的format()函数的值,返回值是string
    
[日志模块类图](https://mermaid.live/view#pako:eNqVVMFq20AQ_RWxEEhbW9iyY8nCBNomhkDcSwwNRRA20lhZkLRitQ5NHd9a2l5Kv6DXQg-lkKMP_RrX_YyutLK8suzG3oM0mnnz3uzOaCfIpR4gG7kBTpITgn2GQyfSxPIIA5cTGmnDF9JzcKAtZu_nXz_NPzwsHmaHL2kY04SkmCcScU59H9gAR1i8NAc1HaQ9rdeFFQlLRjU7ZylS-pSFmHMRSrHy64xDWEX6OWYb0-ktRPw1w3FJuym1s-Bmzg2VPo9jiLxNIusJOf1qF5WMgmuPNPUpDv7ysiBZzL7_-fz778cfKnVVq3cvZC64R8f8cVyfBFBBZbJKO-Y_vy2-_FIaJGEKonev69oAkkT0X-2jzNwMP4dbCFbO_2JPAxwnsCN4eMMAe2fejvA-uQa2M1qSv8LhrtXsAT3BHIYk3HqCj_-HJBIDNcIuVJqV_efLKZ5IX7rq5VkMr0ZLew3jZ1FGKV8LFAMVXuHcTFaQZ1pA_UNd1_MKp2vlFNlKTb0evk44wy4_PlbFlHNZ1pl-7KG22uhETZJc2_IU2VKRxWGrVZbItBIbqqEQRIx44t7NmBzEbyAEB9nC9GCExwF3kBNNBRSPOb24i1xkczaGGhrHnhiP_KZeOsEjnLJBfpWnrxqKcYTsCXqL7Hrb0M2G0TVMq3PUbnTbZg3dIdto6i3DEo5my-gYZstsT2voHaWCtaFbVqfdsbpH3U6r2eiarYzvTRYc4SCB6T8LSBwZ)

# 类图
[![日志系统类图](https://mermaid.ink/img/pako:eNqVVMFq20AQ_RWxEEhbW9iyY8nCBNomhkDcSwwNRRA20lhZkLRitQ5NHd9a2l5Kv6DXQg-lkKMP_RrX_YyutLK8suzG3oM0mnnz3uzOaCfIpR4gG7kBTpITgn2GQyfSxPIIA5cTGmnDF9JzcKAtZu_nXz_NPzwsHmaHL2kY04SkmCcScU59H9gAR1i8NAc1HaQ9rdeFFQlLRjU7ZylS-pSFmHMRSrHy64xDWEX6OWYb0-ktRPw1w3FJuym1s-Bmzg2VPo9jiLxNIusJOf1qF5WMgmuPNPUpDv7ysiBZzL7_-fz778cfKnVVq3cvZC64R8f8cVyfBFBBZbJKO-Y_vy2-_FIaJGEKonev69oAkkT0X-2jzNwMP4dbCFbO_2JPAxwnsCN4eMMAe2fejvA-uQa2M1qSv8LhrtXsAT3BHIYk3HqCj_-HJBIDNcIuVJqV_efLKZ5IX7rq5VkMr0ZLew3jZ1FGKV8LFAMVXuHcTFaQZ1pA_UNd1_MKp2vlFNlKTb0evk44wy4_PlbFlHNZ1pl-7KG22uhETZJc2_IU2VKRxWGrVZbItBIbqqEQRIx44t7NmBzEbyAEB9nC9GCExwF3kBNNBRSPOb24i1xkczaGGhrHnhiP_KZG9ggHifCCRzhlg_wuT181FOMI2RP0Ftn1tqGbDaNrmFbnqN3ots0aukO20dRbhiUczZbRMcyW2Z7W0DtKBW1Dt6xOu2N1j7qdVrPRNVsZ35ssmGlO_wEqdRxk?type=png)](https://mermaid.live/edit#pako:eNqVVMFq20AQ_RWxEEhbW9iyY8nCBNomhkDcSwwNRRA20lhZkLRitQ5NHd9a2l5Kv6DXQg-lkKMP_RrX_YyutLK8suzG3oM0mnnz3uzOaCfIpR4gG7kBTpITgn2GQyfSxPIIA5cTGmnDF9JzcKAtZu_nXz_NPzwsHmaHL2kY04SkmCcScU59H9gAR1i8NAc1HaQ9rdeFFQlLRjU7ZylS-pSFmHMRSrHy64xDWEX6OWYb0-ktRPw1w3FJuym1s-Bmzg2VPo9jiLxNIusJOf1qF5WMgmuPNPUpDv7ysiBZzL7_-fz778cfKnVVq3cvZC64R8f8cVyfBFBBZbJKO-Y_vy2-_FIaJGEKonev69oAkkT0X-2jzNwMP4dbCFbO_2JPAxwnsCN4eMMAe2fejvA-uQa2M1qSv8LhrtXsAT3BHIYk3HqCj_-HJBIDNcIuVJqV_efLKZ5IX7rq5VkMr0ZLew3jZ1FGKV8LFAMVXuHcTFaQZ1pA_UNd1_MKp2vlFNlKTb0evk44wy4_PlbFlHNZ1pl-7KG22uhETZJc2_IU2VKRxWGrVZbItBIbqqEQRIx44t7NmBzEbyAEB9nC9GCExwF3kBNNBRSPOb24i1xkczaGGhrHnhiP_KZG9ggHifCCRzhlg_wuT181FOMI2RP0Ftn1tqGbDaNrmFbnqN3ots0aukO20dRbhiUczZbRMcyW2Z7W0DtKBW1Dt6xOu2N1j7qdVrPRNVsZ35ssmGlO_wEqdRxk)

```mermaid
classDiagram
    direction TB
    %% 组合关系(Composition)
    LoggerManager "1" *-- "n" Logger : 组合
    LogFormatter *-- FormatItem : 组合
    Logger *-- Logger : 组合
    LogEventWrap "1" *-- "1" LogEvent : 组合
    Logger "1" *-- "n" LogAppender : 组合
    Logger "1" *-- "1" LogFormatter : 组合
    LogAppender "1" *-- "1" LogFormatter : 组合
    
    
    %% XXAppender 继承自 LogAppender
    LogAppender <|-- StdoutLogAppender
    LogAppender <|-- FileLogAppender
    %% XXFormatItem 实现 FormatItem
    FormatItem <|.. MessageFormatItem :实现
    FormatItem <|.. LevelFormatItem:实现
    FormatItem <|.. ElapseFormatItem:实现
    FormatItem <|.. ThreadIdFormatItem:实现
    FormatItem <|.. FiberIdFormatItem:实现
    FormatItem <|.. ThreadNameFormatItem:实现
    FormatItem <|.. NameFormatItem:实现
    FormatItem <|.. DateTimeFormatItem :实现
    %% 组合关系(Composition)
    interface FormatItem
    class Logger {
        - LogFormatter m_formatter
        - Logger m_root
        - LogAppender m_appenders
        + log(...)
    }
    class LogAppender {
        <<abstract>>
        - FormatItem m_formatItems
        + log(...)
    }
    class LogFormatter {
        + format(...)
    }
    class FormatItem {
        <<interface>>
        + format(...) 
    }
```

# c复习

**程序从源程序到可执行程序经历了如下几个阶段：**

*   预处理：#define #include会在这个阶段进行替换，还会根据#ifdef #ifndef 这些条件编译指令来决定哪些代码需要被编译。

    文件后缀：`.c`（源代码文件） → `.i`（预处理文件）
*   编译：编译器会将C、C++代码转换成汇编指令，生成相应的汇编代码。

    文件后缀：`.i`（预处理文件） → `.s`（汇编代码文件）
*   汇编：汇编指令被转化成机器指令

    文件后缀：`.s`（汇编代码文件） → `.o`（目标文件）
*   链接：链接器将多个目标文件和库文件组合在一起，生成可执行文件。

    文件后缀：`.o`（目标文件） → `.out` 或 `.exe`（可执行文件）

**原码/反码/补码**

&#x20;原码，反码，补码都是计算机中描述有符合整数的方法。他们的区别在于处理负数的方式不同（正数的原码-反码-补码都相同）：

*   原码：原码的最高位表示符号位，0表示正，1表示负。例如8位二进制数，01100101表示+101，11100101表示-101。

    减法麻烦，需要对符号位进行讨论
*   反码：符号位不变，其余位按位取反。

    存在正零和负零两种表示方法，在加减乘除中要考虑进位的问题。
*   补码：反码+1

    **满足一个很重要的性质：负数的补码加上其绝对值的补码等于0，即 N + (-N) = 0。**

**printf输出控制符**

| 格式控制符                       | 说明                                                                                         |
| :-------------------------- | :----------------------------------------------------------------------------------------- |
| %c                          | 输出一个单一的字符                                                                                  |
| %hd, %d, %ld                | 以**十进制**，有符号的形式输出short，int，long类型的整数                                                       |
| %hu, %u, %lu                | 以**十进制**，无符号的形式输出short，int，long类型的整数                                                       |
| %lo、%o、%lo                  | 以**八进制**，不带前缀，无符号形式输出short, int, long 类型的整数                                                |
| %#lo、%#o、%#lo               | 以**八进制**，带前缀，无符号形式输出short, int, long 类型的整数                                                 |
| %hx、%x、%lx、%hX、%X、%lX       | 以**十六进制**、不带前缀、无符号的形式输出 short、int、long 类型的整数。如果 x 小写，那么输出的十六进制数字也小写；如果 X 大写，那么输出的十六进制数字也大写 |
| %#hx、%#x、%#lx、%#hX、%#X、%#lX | 以**十六进制**、带前缀、无符号的形式输出 short、int、long 类型的整数。如果 x 小写，那么输出的十六进制数字也小写；如果 X 大写，那么输出的十六进制数字也大写  |
| %f、%lf                      | 以十进制输出float，double类型的小数                                                                    |
| %s                          | 输出一个字符串                                                                                    |
| %p                          | 以**十六进制**输出指针类型变量的值                                                                        |
| %n                          | 将已输出的字符数量存储在一个变量中                                                                          |
|                             |                                                                                            |

**结构体内存对齐**

因为在内存中，结构体的大小必须是其最长成员的大小的整数倍。这是为了确保结构体的成员始终按照其声明的顺序正确对齐，并且可以正确地访问它们的地址。
