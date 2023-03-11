#include "Log.h"
namespace sylar {
LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern) {
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger,
                                 LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for (auto& i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

// std::ostream& LogFormatter::format(std::ostream& ofs,
//                                    std::shared_ptr<Logger> logger,
//                                    LogLevel::Level level, LogEvent::ptr
//                                    event) {
//     for (auto& i : m_items) {
//         i->format(ofs, logger, level, event);
//     }
//     return ofs;
// }

// %xxx %xxx{xxx} %%
void LogFormatter::init() {
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }
        // m_pattern[i] == '%'
        size_t n = i + 1;
        if (n < m_pattern.size() && m_pattern[n] == '%') {
            nstr.append(1, '%');
            continue;
        }
        // m_pattern[i] == '%' && m_pattern[i+1] != '%'
        int fmt_status = 0;  // 0: 解析str 1:解析格式
        size_t fmt_begin = 0;
        std::string str;
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' &&
                                m_pattern[n] != '}')) {
                // fmt_status == 0 && m_pattern[n]不是字母也不是{}
                // 遇到空格或%, 将i后面的字符串到m_pattern[n]之前的这一段形成str
                str = m_pattern.substr(i + 1, n - i - 1);
                // std::cout << "str:*" << str << std::endl;
                break;
            }
            if (fmt_status == 0) {
                if (m_pattern[n] == '{') {
                    str = m_pattern.substr(
                        i + 1, n - i - 1);  // substr返回子串[pos, pos+count]
                    // std::cout << "str:*" << str << std::endl;
                    fmt_status = 1;  // 解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    // std::cout << "fmt:#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if (n == m_pattern.size()) {
                if (str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }
        // fmt_status == 0, str = "d", nstr = "%"

        if (fmt_status == 0) {
            if (!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if (fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - "
                      << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    // for (auto i : vec) {
    //     std::cout << "nstr: " << std::get<1>(i) << "-";
    // }
    // std::cout << std::endl;
    if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string,
                    std::function<FormatItem::ptr(const std::string& str)>>
        s_format_items = {
#define XX(str, C)                                                             \
    {                                                                          \
#str,                                                                  \
            [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); } \
    }

            XX(m, MessageFormatItem),  // m:消息
            XX(p, LevelFormatItem),    // p:日志级别
            // XX(r, ElapseFormatItem),    // r:累计毫秒数
            XX(c, NameFormatItem),      // c:日志名称
            XX(t, ThreadIdFormatItem),  // t:线程id
            XX(n, NewLineFormatItem),   // n:换行
            XX(d, DateTimeFormatItem),  // d:时间
            XX(f, FilenameFormatItem),  // f:文件名
            XX(l, LineFormatItem),      // l:行号
            XX(T, TabFormatItem),               //T:Tab
            XX(F, FiberIdFormatItem),           //F:协程id
            XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
        };

    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {  // 非字母
            m_items.push_back(
                FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(
                    "<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i)
        //           << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    // std::cout << m_items.size() << std::endl;
}
}