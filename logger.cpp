//
// Created by GW on 2023/12/16.
//
#include"logger.h"
#include "functional"
#include "config.h"
namespace superG
{
    LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char *m_file, int32_t m_line, int32_t m_threadid, uint32_t m_fiberid, uint32_t m_elapse
                       , uint64_t m_time
                       ,const std::string& thread_name):m_file(m_file),m_time(m_time),m_elapse(m_elapse),m_fiberid(m_fiberid),m_threadid(m_threadid)
                       ,m_line(m_line)
                       ,m_level(level),m_logger(logger),m_thread_name(thread_name){

    }

    void LogEvent::format(const char *fmt,...) {
        va_list vaList;
        va_start(vaList, fmt);
        char * buf= nullptr;

        int len= vasprintf(&buf,fmt,vaList);
        if(len!=-1)
        {
            m_ss<<std::string(buf,len);
            free(buf);
        }
        va_end(vaList);
    }
   Logger::Logger(const std::string& name):m_name(name),m_level(LogLevel::DEBUG)
   {
     m_formmatter.reset(new LogFormmatter("%t  %d %T  %p   %f  %F  %l   %m  %T %n %N"));


   }
    void Logger::log( LogEvent::ptr event,LogLevel::Level level)
    {

       if(level>=m_level)
       {
           auto self=shared_from_this();
           MutexType::Lock lock(m_mutex);
           if(!m_appenders.empty()) {
               for (auto &i: m_appenders) {
                   i->log(self, level, event);
               }

           }
           else
           {
               m_root->log(event,level);
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
        MutexType::Lock lock(m_mutex);
       if(!appender->getFormmatter())
       {
//           Mutex::Lock lock1(appender->getMutex());
           appender->setFormmater(m_formmatter);
           appender->setHasFormatter(false);
       }
     m_appenders.push_back(appender);
    }
    void Logger::deleteAppender(LogAppender::ptr appender)
    {
        MutexType::Lock lock(m_mutex);
      for(auto i=m_appenders.begin();i!=m_appenders.end();i++)
      {
          if(*i==appender)
          {
              m_appenders.erase(i);
              break;
          }
      }
    }


    void Logger::setFormmater(const  std::string& val)
    {
        MutexType::Lock lock(m_mutex);
        LogFormmatter::ptr new_val(new superG::LogFormmatter(val));
        if(!new_val->isError())
        {
            m_formmatter=new_val;
            for (auto &a: m_appenders) {
//                Mutex::Lock lock1(a->getMutex());
                if (!a->getHasFormatter()) {
                    a->setFormmater(m_formmatter);
                }
            }

        }
        else
        {
            std::cout<<"Logger set formmater name"<<" "<<m_name<<" value="<<val<<" invalid args"<<std::endl;
        }
    }
    void Logger::setFormmater(LogFormmatter::ptr val)
    {
        MutexType::Lock lock(m_mutex);
        if(val) {
            m_formmatter = val;
            for (auto &a: m_appenders) {
//                Mutex::Lock lock1(a->getMutex());
                if (!a->getHasFormatter()) {
                    a->setFormmater(m_formmatter);
                }
            }
        }
    }

    LogFormmatter::ptr Logger::getFormmater()
    {
        return m_formmatter;
    }
    std::string Logger::toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["name"]=m_name;
        node["level"]=LogLevel::ToString(m_level);
        if(m_formmatter)
        {
            node["formatter"]=m_formmatter->getPattern();
        }

            for (auto &i: m_appenders) {
                node["appenders"].push_back(i->toYamlString());
            }

        std::stringstream ss;
        ss<<node;
        return ss.str();
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
        MutexType::Lock lock(m_mutex);
       if(level>=m_level)
       {
           std::cout<<m_formmatter->format(logger,level,logEvent);
       }

    }

    void LogAppender::setFormmater(LogFormmatter::ptr formmatter)
    {
        MutexType::Lock lock(m_mutex);
        m_formmatter=formmatter;
        if(formmatter)
        {
            has_formatter=true;
        }
        else
        {
            has_formatter=false;
        }
    }

    LogFormmatter::ptr LogAppender::getFormmatter()
    {
        MutexType::Lock lock(m_mutex);
        return m_formmatter;
    };

    std::string StdoutLogAppender::toYamlString() {
        MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"]="StdoutLogAppender";
    node["level"]=
    LogLevel::ToString(m_level);
    if(m_formmatter&&has_formatter) {
    node["formatter"] = m_formmatter->getPattern();
}

    std::stringstream ss;
    ss<<node;
    return ss.str();
}


    void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr logEvent) {

       if(level>=m_level)
       {
           uint64_t now= time(0);
           if(now!=m_time)
           {
               m_time=now;
               reopen();
           }
           MutexType::Lock lock(m_mutex);
           m_filestream<<m_formmatter->format(logger,level,logEvent);
       }

    }
    std::string FileLogAppender:: toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["name"]="FileLogAppender";
        node["level"]=LogLevel::ToString(m_level);
        if(m_formmatter&&has_formatter) {
            node["formatter"] = m_formmatter->getPattern();
        }
        node["file"]=m_name;
        std::stringstream ss;
       ss<<node;
       return ss.str();

    }


    FileLogAppender::FileLogAppender(const std::string& filename) :m_name(filename){
        reopen();

    }

    bool FileLogAppender::reopen() {
        MutexType::Lock lock(m_mutex);
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
                m_error=true;

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
                XX(F,FiberidFormatItem),
                XX(k,NameFormatItem),
                XX(N,ThreadNameFormatItem)



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
                    m_error=true;

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




    }

    LogLevel::Level LogLevel::fromString(const std::string& str)
    {
#define XX(name,xxxx) \
        if(str==#xxxx) \
        {   \
          return LogLevel::name;\
        }
        XX(DEBUG,debug);
        XX(FATAL,fatal);
        XX(INFO,info);
        XX(WARN,warn);
        XX(ERROR,error);
        XX(DEBUG,DEBUG);
        XX(FATAL,FATAL);
        XX(INFO,INFO);
        XX(WARN,WARN);
        XX(ERROR,ERROR);

        return LogLevel::UNKNOWN;
#undef XX
    }


    LogManager::LogManager() {
        m_root.reset(new Logger);
        m_root->setFormmater("%t  %d %T  %p   %f  %F  %l   %m  %T   %N %n");
        m_root->addAppender(LogAppender::ptr (new StdoutLogAppender));
        m_loggers[m_root->m_name]=m_root;


        init();

    }
    Logger::ptr LogManager::getLogger(const std::string &name) {
        MutexType::Lock lock(m_mutex);
        auto it=m_loggers.find(name);
        if(it!=m_loggers.end())
        {
            return it->second;
        }

        Logger::ptr logger;
        logger.reset(new Logger(name));
        m_loggers[name]=logger;
        logger->m_root=m_root;
        return logger;



    }
    struct appenderDefine
    {
        std::string file;
        int type=0;
        std::string formatter;
        LogLevel::Level level=LogLevel::UNKNOWN;
        bool operator==(appenderDefine& appender)
        {
            return file==appender.file
            && type==appender.type
            &&formatter==appender.formatter
            &&level==appender.level;
        }

    };
    struct LogDefine
    {
        std::string name;
        LogLevel::Level level=LogLevel::UNKNOWN;
        std::string formatter;
        std::vector<appenderDefine> appenders;
        bool operator==(const LogDefine& d) const
        {
            return name==d.name
            &&formatter==d.formatter&&level==d.level;
        }
        bool operator<(const LogDefine& logDefine) const
        {
            return name<logDefine.name;
        }
    };
    template<>
    class LexicalCast<LogDefine,std::string>
    {
    public:
        LogDefine operator ()(const std::string& val) {
            std::stringstream ss;
            YAML::Node node = YAML::Load(val);
            LogDefine p;
            if(node["name"].IsDefined()) {

                p.name = node["name"].as<std::string>();
            }
            if(node["level"].IsDefined()) {
                p.level = LogLevel::fromString(node["level"].as<std::string>());
            }
            if(node["formatter"].IsDefined()) {

                p.formatter = node["formatter"].as<std::string>();
            }
            if (node["appenders"].IsDefined())
            {
                for(int i=0;i<node["appenders"].size();i++)
                {

                    if(!node["appenders"][i]["type"].IsDefined())
                    {
                        std::cout<<"Log Appenders is no define type"<<std::endl;
                        continue;
                    }
                    appenderDefine appender;
                    if(node["appenders"][i]["type"].as<std::string>()=="StdoutLogAppender")
                    {
                        appender.type=2;
                    }
                    else if(node["appenders"][i]["type"].as<std::string>()== "FileLogAppender")
                    {
                        appender.type=0;
                        if(!node["appenders"][i]["file"].IsDefined())
                        {
                            std::cout<<"Log Appenders is no define filepath"<<std::endl;
                            continue;

                        }
                        appender.file=node["appenders"][i]["file"].as<std::string >();


                    }
                    if(node["appenders"][i]["formatter"].IsDefined())
                    {
                        appender.formatter=node["appenders"][i]["formatter"].as<std::string>();
//                        std::cout<<node["appenders"][i]["formatter"]<<std::endl;
//                        std::cout<<appender.formatter<<std::endl;
                    }
                    if(node["appenders"][i]["level"].IsDefined())
                    {
                        appender.level=LogLevel::fromString(node["appenders"][i]["level"].as<std::string>());
                    }
                    p.appenders.push_back(appender);
                }
            }
            return p;





        }
    };
    template<>
    class LexicalCast<std::string ,LogDefine>
    {
    public:
        std::string operator ()(const LogDefine& lst)
        {

            YAML::Node node;
            node["name"]=lst.name;
           node["level"]=LogLevel::ToString(lst.level);
           if(!lst.formatter.empty()) {
               node["formatter"] = lst.formatter;
           }
           for(auto &i:lst.appenders)
           {

               YAML::Node na;
               if(i.type==0)
               {
                   na["type"]="FileLogAppender";
                   na["file"]=i.file;

               }
               else if(i.type==2)
               {
                   na["type"]="StdoutLogAppender";
               }
               else
               {
                   continue;
               }
               if(!i.formatter.empty())
               {

                   na["formatter"]=i.formatter;
               }

               na["level"]=LogLevel::ToString(i.level);
               node["appenders"].push_back(na);
           }



            std::stringstream ss;
            ss<<node;
            return ss.str();


        }
    };
    superG::ConfigVar<std::set<LogDefine>>::ptr  p_to_logs=superG::configure::Lookup("logs",std::set
    <LogDefine>(),"logs configure");
    struct LogIniter
    {
        LogIniter()
        {


            p_to_logs->addListener([](const std::set<LogDefine>& old_value,const std::set<LogDefine>& new_value)
            {

                for(auto& i:new_value)
                {

                    auto it=old_value.find(i);
                    if(it==old_value.end())
                    {

                        superG::Logger::ptr logger= SUPERG_LOG_NAME(i.name);
                        logger->setLevel(i.level);

                        if(!i.formatter.empty())
                        {
                            logger->setFormmater(i.formatter);
                        }
                        else
                        {
                            std::cout<<"logger"<<"  "<<i.name<<"  formatter ERROR"<<i.formatter<<std::endl;

                        }

                        for(auto &a:i.appenders)
                        {


                            superG::LogAppender::ptr appender;
                            if(a.type==0)
                            {
                               appender.reset(new FileLogAppender(a.file));
                            }
                            else if(a.type==2)
                            {
                                appender.reset(new StdoutLogAppender);
                            }
                            else
                            {
                                continue;
                            }
                            if(!a.formatter.empty())
                            {
                                LogFormmatter::ptr logFormmatter(new LogFormmatter(a.formatter));
                                if(!logFormmatter->isError())
                                {
                                    appender->setFormmater(logFormmatter);
                                }
                                else
                                {
                                    std::cout<<"logger"<<"  "<<a.file<<"  formatter ERROR"<<a.formatter<<std::endl;
                                }
//                                appender->setFormmater(a.formatter);
                            }
                            appender->setLevel(a.level);

                            logger->addAppender(appender);

                        }

                    }
                    else
                    {
                        if(!(i==*it))
                        {
                        Logger::ptr lp= SUPERG_LOG_NAME(i.name);
                        lp->setLevel(i.level);
                            if(!i.formatter.empty())
                            {
                                lp->setFormmater(i.formatter);
                            }
                            lp->clearAppenders();
                            for(auto &a:i.appenders)
                            {
                                superG::LogAppender::ptr appender;
                                if(a.type==0)
                                {
                                    appender.reset(new FileLogAppender(a.file));
                                }
                                else if(a.type==2)
                                {
                                    appender.reset(new StdoutLogAppender);
                                }
                                else
                                {
                                    continue;
                                }

                                    appender->setLevel(a.level);

                                    LogFormmatter::ptr logFormmatter(new LogFormmatter(a.formatter));
                                    if(!logFormmatter->isError())
                                    {
                                        appender->setFormmater(logFormmatter);
                                    }
                                    lp->addAppender(appender);
                                }

                            }


                        }
                    }


                for(auto &i:old_value)
                {
                    auto it=new_value.find(i);
                    if(it==new_value.end())
                    {
                        auto logger=SUPERG_LOG_NAME(i.name);
                        logger->setLevel((LogLevel::Level)100);
                        logger->clearAppenders();



                    }
                }

            });
        }
    };
    std::string LogManager::toYamlString()
    {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        for(auto &i:m_loggers)
        {
            node.push_back(i.second->toYamlString());

        }
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
    static LogIniter _log_init;
    void LogManager::init() {

    }
}
