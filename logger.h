//
// Created by GW on 2023/12/16.
//

#ifndef HIGHPERFORMANCESERVER_LOGGER_H
#define HIGHPERFORMANCESERVER_LOGGER_H

#include<string>
#include "stdint.h"
#include<memory>
#include<list>
#include <fstream>
#include <iostream>
#include <sstream>
#include<map>
#include <time.h>
#include "singleton.h"
#include "util.h"
#define SUPERG_LOG_LEVEL(logger,level) \
        if(logger->getLevel()<=level)    \
           superG:: LogEventWrap(superG::LogEvent::ptr(new superG::LogEvent(logger,level,__FILE_NAME__,__LINE__,superG::GetThreadId(),superG::GetFiberd(),0,time(0)))) \
             .getSS()


#define    SUPERG_LOG_DEBUG(logger)  SUPERG_LOG_LEVEL(logger,superG::LogLevel::DEBUG)
#define    SUPERG_LOG_FATAL(logger)  SUPERG_LOG_LEVEL(logger,superG::LogLevel::FATAL)
#define    SUPERG_LOG_INFO(logger)  SUPERG_LOG_LEVEL(logger,superG::LogLevel::INFO)
#define    SUPERG_LOG_WARN(logger)  SUPERG_LOG_LEVEL(logger,superG::LogLevel::WARN)
#define    SUPERG_LOG_ERROR(logger)  SUPERG_LOG_LEVEL(logger,superG::LogLevel::ERROR)


#define SUPERG_LOG_FMT_LEVEL(logger,level,fmt,...) \
        if(logger->getLevel()<=level)    \
           superG:: LogEventWrap(superG::LogEvent::ptr(new superG::LogEvent(logger,level,__FILE_NAME__,__LINE__,superG::GetThreadId(),superG::GetFiberd(),0,time(0)))) \
             .getEvent()->format(fmt,__VA_ARGS__)


#define    SUPERG_LOG_FMT_DEBUG(logger,fmt,...)  SUPERG_LOG_FMT_LEVEL(logger,superG::LogLevel::DEBUG,fmt,__VA_ARGS__)
#define    SUPERG_LOG_FMT_FATAL(logger,fmt,...)  SUPERG_LOG_FMT_LEVEL(logger,superG::LogLevel::FATAL,fmt,__VA_ARGS__)
#define    SUPERG_LOG_FMT_INFO(logger,fmt,...)  SUPERG_LOG_FMT_LEVEL(logger,superG::LogLevel::INFO,fmt,__VA_ARGS__)
#define    SUPERG_LOG_FMT_WARN(logger,fmt,...)  SUPERG_LOG_FMT_LEVEL(logger,superG::LogLevel::WARN,fmt,__VA_ARGS__)
#define    SUPERG_LOG_FMT_ERROR(logger,fmt,...)  SUPERG_LOG_FMT_LEVEL(logger,superG::LogLevel::ERROR,fmt,__VA_ARGS__)

#define    SUPERG_LOG_ROOT()  superG::LogMng::GetInstance()->getRoot()
namespace superG
{
    class Logger;
    class LogLevel {
    public:
        enum Level {
            UNKNOWN=0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5

        };
        static const char* ToString(LogLevel::Level level);

    };

    class Logger;
    //日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(
                std::shared_ptr<Logger> logger,
                LogLevel::Level level,
                const char* m_file,
        int32_t m_line,
        int32_t m_threadid,
        uint32_t m_fiberid,
        uint32_t m_elapse,
        uint64_t m_time);
        int32_t getLine() const {return m_line;};
        int32_t getThreadId() const {return m_threadid;};
        uint32_t getFiberid() const {return m_fiberid;};
        uint32_t getElapse() const {return m_elapse;};
        std::string getContent() const {return m_ss.str();};
        uint64_t getTime() const {return m_time;};
        const char * getFile() const {return m_file;};


       std::stringstream& getSS(){return m_ss;};
       std::shared_ptr<Logger> getLogger() const { return m_logger;};
        LogLevel::Level getLevel() const { return m_level;};

       void format(const char* fmt,...);

    private:
        const char* m_file= nullptr;
        int32_t m_line=0;
        int32_t m_threadid=0;
        uint32_t m_fiberid=0;
        uint32_t m_elapse=0;
        std::stringstream m_ss;
        uint64_t m_time;
        std::shared_ptr<Logger> m_logger;
        LogLevel::Level m_level;
    };

    class LogEventWrap
    {
    public:
        LogEventWrap(LogEvent::ptr  p);
        std::stringstream& getSS();
        ~LogEventWrap();
        LogEvent::ptr getEvent() const {return m_ptr;};

    private:
        LogEvent::ptr m_ptr;
    };



    //日志格式
    class LogFormmatter
    {
    public:
        typedef std::shared_ptr<LogFormmatter> ptr;
        std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event);
        LogFormmatter(const std::string& pattern);

        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem(){};
            FormatItem(const std::string& str=""):m_string(str){};
            virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)=0;

        protected:
            std::string  m_string;

        };
    private:
        std::string m_pattern;
        void init();
        std::vector<FormatItem::ptr> m_items;
    };
    //日志目的地
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;

        virtual ~LogAppender(){};
        virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr logEvent)=0;
        void setFormmater(LogFormmatter::ptr formmatter){m_formmatter=formmatter;};
        LogFormmatter::ptr getFormmatter() const {return m_formmatter;};
        LogLevel::Level getLevel() const {return m_level;};
        void setLevel(LogLevel::Level level){m_level=level;};


    protected:
        LogLevel::Level m_level=LogLevel::DEBUG;
        LogFormmatter::ptr m_formmatter;

    };


    //日志输出器
class Logger:public std::enable_shared_from_this<Logger>
    {
    public:
        typedef std::shared_ptr<Logger> ptr;


        Logger(const std::string& name="root");
        void log( LogEvent::ptr event,LogLevel::Level level);
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        void addAppender(LogAppender::ptr appender);
        void deleteAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const
        {
            return m_level;
        }
      void setLevel(LogLevel::Level level)
        {
            m_level=level;
        }

        const std::string& getname(){ return m_name;};

    private:
        std::string m_name;
        std::list<LogAppender::ptr> m_appenders;
        LogLevel::Level m_level;
        LogFormmatter::ptr m_formmatter;

    };
    class StdoutLogAppender:public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
         void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr logEvent) override;


    };
    class FileLogAppender:public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr logEvent) override;
        FileLogAppender(const std::string & filename);
        bool reopen();
    private:
        std::string m_name;
        std::ofstream m_filestream;


    };
    class MessageFormatItem:public LogFormmatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string str): LogFormmatter::FormatItem(str){};
        typedef std::shared_ptr<MessageFormatItem> ptr;
        virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)
        {
            os<<event->getContent();
        }
    };
    class DateTimeFormatItem:public LogFormmatter::FormatItem
    {
    public:
        typedef std::shared_ptr<DateTimeFormatItem> ptr;
        DateTimeFormatItem(const std::string formmat="%Y:%m:%d %H:%M:%S"):m_formmat(formmat),FormatItem(formmat){
            if(m_formmat.empty())
            {
                m_formmat="%Y:%m:%d %H:%M:%S";
            }
        };
        virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)
        {
            struct tm tm;
            time_t time=event->getTime();
            localtime_r(&time, &tm);
            char buf [64];
            strftime(buf,sizeof(buf),m_formmat.c_str(),&tm);
            os<<buf;
        }
    private:
        std::string m_formmat;
    };
    class ThreadIdFormatItem:public LogFormmatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string str):FormatItem(str){};

        typedef std::shared_ptr<ThreadIdFormatItem> ptr;
        virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)
        {
            os<<event->getThreadId();
        }
    };
    class ElapseFormatItem:public LogFormmatter::FormatItem
    {
    public:
        typedef std::shared_ptr<ElapseFormatItem> ptr;
        ElapseFormatItem(const std::string str): LogFormmatter::FormatItem(str){};

        virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)
        {
            os<<event->getElapse();
        }
    };

    class  FileNameFormatItem:public LogFormmatter::FormatItem
    {
    public:
        typedef std::shared_ptr<FileNameFormatItem> ptr;
        FileNameFormatItem(const std::string str): LogFormmatter::FormatItem(str){};

        virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)
        {
            os<<event->getFile();
        }
    };
    class FiberidFormatItem:public LogFormmatter::FormatItem
    {
    public:
        typedef std::shared_ptr<FiberidFormatItem> ptr;
        FiberidFormatItem(const std::string str): LogFormmatter::FormatItem(str){};

        virtual void format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr event)
        {
            os<<event->getFiberid();
        }
    };
    class LogLevelFormatItem:public LogFormmatter::FormatItem {
    public:
        typedef std::shared_ptr<LogLevelFormatItem> ptr;
        LogLevelFormatItem(const std::string str): LogFormmatter::FormatItem(str){};

        virtual void
        format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream &os, LogEvent::ptr event) {
            os << LogLevel::ToString(level);
        }
    };
    class LineFormatItem:public LogFormmatter::FormatItem {
    public:
        typedef std::shared_ptr<LineFormatItem> ptr;
        LineFormatItem(const std::string str): LogFormmatter::FormatItem(str){};

        virtual void
        format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream &os, LogEvent::ptr event) {
            os << event->getLine();
        }
    };
    class NewLineFormatItem:public LogFormmatter::FormatItem {
    public:
        typedef std::shared_ptr<NewLineFormatItem> ptr;
        NewLineFormatItem(const std::string str): LogFormmatter::FormatItem(str){};

        virtual void
        format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream &os, LogEvent::ptr event) {
            os <<std::endl;
        }
    };
    class StringFormatItem:public LogFormmatter::FormatItem {
    public:
        typedef std::shared_ptr<StringFormatItem> ptr;
         StringFormatItem(const std::string& str):m_string(str),FormatItem(str){};
        virtual void
        format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream &os, LogEvent::ptr event) {
            os <<m_string;
        }
    private:
        std::string  m_string;
    };
    class TabFormatItem:public LogFormmatter::FormatItem {
    public:
        typedef std::shared_ptr<TabFormatItem> ptr;
        TabFormatItem(const std::string& str):m_string(str),FormatItem(str){};
        virtual void
        format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream &os, LogEvent::ptr event) {
            os <<"\t";
        }
    private:
        std::string  m_string;
    };

class  LogManager
{
public:
    LogManager();
    Logger::ptr getLogger(const std::string& name);
    Logger::ptr getRoot()const {return m_root;};

private:
    std::map<std::string ,Logger::ptr > m_loggers;
    Logger::ptr  m_root;

};


typedef superG::Singleton<LogManager> LogMng;


    };






#endif //HIGHPERFORMANCESERVER_LOGGER_H
