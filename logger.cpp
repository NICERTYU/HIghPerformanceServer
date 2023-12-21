//
// Created by GW on 2023/12/16.
//
#include"logger.h"
#include "functional"
namespace superG
{
    LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char *m_file, int32_t m_line, int32_t m_threadid, uint32_t m_fiberid, uint32_t m_elapse
                       , uint64_t m_time):m_file(m_file),m_time(m_time),m_elapse(m_elapse),m_fiberid(m_fiberid),m_threadid(m_threadid)
                       ,m_line(m_line)
                       ,m_level(level),m_logger(logger){

    }

    void LogEvent::format(const char *fmt,...) {
        va_list vaList;
        va_start(vaList, fmt);
        char * buf= nullptr;

        int len= vasprintf(&buf,fmt,vaList);
        if(len!=-1)
        {
            m_ss<<std::string(buf,len);
//            std::cout<<std::string (buf,len);
            free(buf);
        }
        va_end(vaList);
    }
   Logger::Logger(const std::string& name):m_name(name),m_level(LogLevel::DEBUG)
   {
     m_formmatter.reset(new LogFormmatter("%t  %d %T  %p   %f  %F  %l   %m  %T %n"));
   }
    void Logger::log( LogEvent::ptr event,LogLevel::Level level)
    {

       if(level>=m_level)
       {
           auto self=shared_from_this();
          for(auto& i : m_appenders)
          {
              i->log(self,level,event);
          }
       }

    }
    void Logger::debug(LogEvent::ptr event)
    {
       log(event,LogLevel::DEBUG);
    }
    void Logger::info(LogEvent::ptr event)
    {
        log(event,LogLevel::INFO);

    }
    void Logger::warn(LogEvent::ptr event)
    {
        log(event,LogLevel::WARN);

    }
    void Logger::fatal(LogEvent::ptr event)
    {
        log(event,LogLevel::FATAL);

    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
       if(!appender->getFormmatter())
       {
           appender->setFormmater(m_formmatter);
       }
     m_appenders.push_back(appender);
    }
    void Logger::deleteAppender(LogAppender::ptr appender)
    {
      for(auto i=m_appenders.begin();i!=m_appenders.end();i++)
      {
          if(*i==appender)
          {
              m_appenders.erase(i);
              break;
          }
      }
    }
   LogEventWrap::LogEventWrap(LogEvent::ptr p)
   :m_ptr(p){

    }

    std::stringstream &LogEventWrap::getSS() {
        return m_ptr->getSS();
    }
    LogEventWrap::~LogEventWrap()
    {
      m_ptr->getLogger()->log(m_ptr,m_ptr->getLevel());
    }
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr logEvent) {
       if(level>=m_level)
       {
           std::cout<<m_formmatter->format(logger,level,logEvent);
       }

    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr logEvent) {
       if(level>=m_level)
       {
           m_filestream<<m_formmatter->format(logger,level,logEvent);
       }

    }

    FileLogAppender::FileLogAppender(const std::string& filename) :m_name(filename){
        reopen();

    }

    bool FileLogAppender::reopen() {
       if(m_filestream)
       {
           m_filestream.close();
       }
       m_filestream.open(m_name);
       return !m_filestream;

    }

    LogFormmatter::LogFormmatter(const std::string & pattern) :m_pattern(pattern){
        init();

    }

    std::string LogFormmatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) {
       std::stringstream ss;
       for(auto &i:m_items)
       {
           i->format(logger,level,ss,event);
       }
       return ss.str();

    }

    void LogFormmatter::init() {
        std::vector<std::tuple<std::string, std::string, int>> vec;
//        std::cout<<m_pattern;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); i++) {
            if (m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            if ((i + 1 )< m_pattern.size()) {
                if (m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }
            size_t n = i + 1;
            int fmt_stats = 0;
            size_t fmt_begin = 0;
            std::string str;
            std::string fmt;
            while (n < m_pattern.size()) {


                if(!fmt_stats&&(!isalpha(m_pattern[n])&&m_pattern[n]!='{'&&m_pattern[n]!='}'))
                {
                    break;
                }

                if (fmt_stats == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_stats = 1;
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }

                if (fmt_stats == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_stats = 2;
                        break;
                    }
                }
                ++n;
            }
            if (fmt_stats == 0) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                str = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));

                i = n;


            } else if (fmt_stats == 1) {
//                std::cout << "Pattren Parse Error" << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<error>", fmt, 0));

            } else if (fmt_stats == 2) {
                if(!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                    nstr.clear();

                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }

        }
        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));

            nstr.clear();
        }


        static std::map<std::string, std::function<LogFormmatter::FormatItem::ptr(const std::string str)>> s_formmater_item = {
#define XX(str, C) \
        {#str, [](const std::string& format){ return LogFormmatter::FormatItem::ptr(new C(format));}} \

                XX(t, ThreadIdFormatItem) ,
                XX(d, DateTimeFormatItem) ,
                XX(r, ElapseFormatItem) ,
                XX(f, FileNameFormatItem) ,
                XX(p, LogLevelFormatItem) ,
                XX(n, NewLineFormatItem) ,
                XX(l, LineFormatItem) ,
                XX(m,MessageFormatItem),
                XX(T,TabFormatItem),
                XX(F,FiberidFormatItem)



#undef XX

        };

        for(auto& i : vec)
        {
//            std::cout<<vec.size();
//            std::cout<<std::get<0>(i)<<std::endl;
//            std::cout<<std::get<1>(i)<<std::endl;
//            std::cout<<std::get<2>(i)<<std::endl;


            if(std::get<2>(i)==0)
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));

            }
            else
            {
                auto it=s_formmater_item.find(std::get<0>(i));

                if(it==s_formmater_item.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error formmate"+std::get<0>(i)+">>")));

                }
                else
                {
                    m_items.push_back(it->second(std::get<1>(i)));

                }
            }
//            std::cout<<std::get<0>(i)<<" ---"<<std::get<1>(i)<<"--- "<<std::get<2>(i);
        }



    };



    const char *LogLevel::ToString(LogLevel::Level level) {
        switch(level)
        {
//            case LogLevel::DEBUG:
//                return "DEBUG";
//            case LogLevel::WARN:
//                return "WARN";
//            case LogLevel::INFO:
//                return "INFO";
//            case LogLevel::FATAL:
//                return "FATAL";
//            case LogLevel::ERROR:
//                return "ERROR";
//
#define  XX(name) \
            case LogLevel::name: \
                return #name;    \
                break;
                XX(DEBUG);
                XX(FATAL);
                XX(INFO);
                XX(WARN);
                XX(ERROR);
#undef XX

            default:
                return "UNKNOWN";
                break;
        }

        return "UNKNOWN";


    }

    LogManager::LogManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr (new StdoutLogAppender));

    }
    Logger::ptr LogManager::getLogger(const std::string &name) {
        auto it=m_loggers.find(name);
        return it==m_loggers.end()?m_root:it->second;

    }
}
