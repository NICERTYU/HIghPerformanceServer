#include <iostream>
#include "superG.h"
//#define LOG(...) std::cout << __VA_ARGS__ << std::endl
static superG::Logger::ptr logger_root=SUPERG_LOG_ROOT();
#define _LIBCPP_NO_EXCEPTIONS


static int count=0;
void testyaml() {
    superG::ConfigVar<int>::ptr p_int_port = superG::configure::Lookup("system.port", (int) 8080,
                                                                       "system_port_configure");
    superG::ConfigVar<float>::ptr p_intX_port = superG::configure::Lookup("system.port", (float) 8080,
                                                                       "system_port_configure");
    superG::ConfigVar<int>::ptr p_int_value = superG::configure::Lookup("system.value", (int) 14,
                                                                        "system_port_configure");
    superG::ConfigVar<std::vector<int>>::ptr p_int_vector_value = superG::configure::Lookup("system.vec",
                                                                                            std::vector<int>{1, 3, 5},
                                                                                            "system_port_configure");
    superG::ConfigVar<std::list<int>>::ptr p_int_list_value = superG::configure::Lookup("system.list",
                                                                                        std::list<int>{1, 3, 5},
                                                                                        "system_port_configure");
    superG::ConfigVar<std::set<int>>::ptr p_int_set_value = superG::configure::Lookup("system.set",
                                                                                        std::set<int>{5,2,0},
                                                                                        "system_port_configure");
    superG::ConfigVar<std::unordered_set<int>>::ptr p_int_unorderd_set_value = superG::configure::Lookup("system.unorderedset",
                                                                                      std::unordered_set<int>{5,2,0},
                                                                                      "system_port_configure");
    superG::ConfigVar<std::map<std::string ,int>>::ptr p_int_map_value = superG::configure::Lookup("system.map",
                                                                                                         std::map<std::string ,int>{{"k",520}},
                                                                                                         "system_port_configure");
    superG::ConfigVar<std::unordered_map<std::string ,int>>::ptr p_int_unordered_map_value = superG::configure::Lookup("system.unorderedmap",
                                                                                                   std::unordered_map<std::string ,int>{{"k",520}},
                                                                                                   "system_port_configure");



    SUPERG_LOG_INFO(SUPERG_LOG_ROOT()) << "before" << " :" << p_int_value->getValue() << std::endl;
    SUPERG_LOG_INFO(SUPERG_LOG_ROOT()) << "before" << " :" << p_int_port->getValue() << std::endl;
#define XX(g_var,name,prefix) \
{                            \
     auto v=g_var->getValue();\
       SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<" :"<<#name<<" "<<#prefix<<" "<<std::endl<<g_var->toString();                                \
       for(auto& i: v)        \
       {                     \
       SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<" :"<<#name<<" "<<#prefix<<" "<<i<<std::endl; \
                             \
       }\
                             \
                             \
}

#define XX_M(g_var,name,prefix) \
{                            \
     auto v=g_var->getValue();\
       SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<" :"<<#name<<" "<<#prefix<<" "<<std::endl<<g_var->toString();                                \
       for(auto& i: v)        \
       {                     \
       SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<" :"<<#name<<" "<<#prefix<<" "<<"{"<<i.first<<"-"<<i.second<<"}"<<std::endl; \
                             \
       }\
                             \
                             \
}
//    XX(p_int_vector_value,int_vec,before);
//    XX(p_int_list_value,int_lst,before);
//    XX(p_int_set_value,int_set,before);
//  XX(p_int_unorderd_set_value,int_unordered_set,before);
//  XX_M(p_int_map_value,int_map,before);
    XX_M(p_int_unordered_map_value,int_unmap,before);




    YAML::Node root=YAML::LoadFile("/Users/gw/CLionProjects/HighPerformanceServer/YAML/test.yml");
    superG::configure::LoadFromYaml(root);

//    XX(p_int_vector_value,int_vec,after);
//    XX(p_int_list_value,int_lst,after);
//    XX(p_int_set_value,int_set,after);
//    XX(p_int_unorderd_set_value,int_unordered_set,after);

//    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<"after"<<" :"<<p_int_value->getValue()<<std::endl;
//    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<"after"<<" :"<<p_int_port->getValue()<<std::endl;
//    XX_M(p_int_map_value,int_map,after);
    XX_M(p_int_unordered_map_value,int_unmap,after);



}
class Person
{
public:
    std::string m_name;
    short m_age;
    bool m_sex;
    std::string toString() const
    {
        std::stringstream ss;
        ss<<"["<<m_name<<"-"<<m_age<<"-"<<m_sex<<"]";
        return ss.str();
    }
    bool operator==(const Person& oth) const
    {
        return m_name==oth.m_name
               &&m_sex==oth.m_sex
               &&m_age==oth.m_age;
    }
};
namespace superG
{
    template<>
    class LexicalCast<Person,std::string>
    {
    public:
        Person operator ()(const std::string& val)
        {
            std::stringstream ss;
            YAML::Node node=YAML::Load(val);
            Person p;
            p.m_name=node["name"].as<std::string>();
            p.m_age=node["age"].as<short>();
            p.m_sex=node["sex"].as<bool>();
            return p;





        }
    };
    template<>
    class LexicalCast<std::string ,Person>
    {
    public:
        std::string operator ()(const Person& lst)
        {

            YAML::Node node;
            node["name"]=lst.m_name;
            node["age"]=lst.m_age;
            node["sex"]=lst.m_sex;

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
}
void test_class() {
    superG::ConfigVar<Person>::ptr p_person_value = superG::configure::Lookup("class.person", Person(),
                                                                              "system person");
    superG::ConfigVar<std::map<std::string, Person>>::ptr p_person_map_value = superG::configure::Lookup(
            "class.personmap", std::map<std::string, Person>(), "system person map");
    superG::ConfigVar<std::map<std::string, std::vector<Person>>>::ptr p_person_mapvec_value = superG::configure::Lookup(
            "class.personmapvec", std::map<std::string, std::vector<Person>>(), "system person mapvec");
#define XX_P(p_person_map_value, prefix){ \
    auto p_map=p_person_map_value->getValue();\
    for(auto& i:p_map)\
    {\
        SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<#prefix<<i.first<<" - "<<i.second.toString();\
    }                                   \
  SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_map.size();                                      \
                                         \
                }




    p_person_value->addListener([](const Person& old_value,const Person& new_value)
    {
        SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<"old_value:"<<old_value.toString()<<"  new_value:"<<new_value.toString();
    });
    XX_P(p_person_map_value,"before");
    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_person_map_value->toString();
    YAML::Node root=YAML::LoadFile("/Users/gw/CLionProjects/HighPerformanceServer/YAML/test.yml");
    superG::configure::LoadFromYaml(root);
    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_person_map_value->toString();


    XX_P(p_person_map_value,"after");
//    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_person_value->toString();

//    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_person_value->toString();
//    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_person_value->getValue().toString();

}
void test_logs() {
    static superG::Logger::ptr logger=superG::LogMng::GetInstance()->getLogger("system");
//    SUPERG_LOG_INFO(logger)<<"hello system";
//    std::cout<<superG::LogMng::GetInstance()->toYamlString()<<std::endl;
    YAML::Node root=YAML::LoadFile("/Users/gw/CLionProjects/HighPerformanceServer/YAML/log.yml");
    superG::configure::LoadFromYaml(root);
//    std::cout<<superG::LogMng::GetInstance()->toYamlString()<<std::endl;

//    SUPERG_LOG_INFO(logger)<<"hello system";

//    logger->setFormmater("%m --- %d");

//    SUPERG_LOG_INFO(logger)<<"hello system";


}

superG::ReadWriteMutex readWriteMutex;
superG::Mutex    mutex;
void fun1()
{
    SUPERG_LOG_INFO(logger_root)<<"thread_id:"<<pthread_self()<<"  "<<"name:"<<superG::Thread::GetName();
    for(int i=0;i<10000000;i++)
    {
//        superG::ReadWriteMutex::WriteLock lock(readWriteMutex);
        superG::Mutex::Lock  lock(mutex);
        SUPERG_LOG_INFO(logger_root)<<count<<"  "<<superG::Thread::GetName();


        count++;
    }


}
void test_assert()
{
//    SUPERG_LOG_INFO(logger_root)<<superG::BacktraceToString(10,"  ");
//    SUPERG_ASSERT(false);
      SUPERG_ASSERT1(false,"kkk");

}
void fun2()
{

}
void teatThread()
{

}

void runinFiber()
{
    SUPERG_LOG_INFO(logger_root)<<"hhhhhh";
    superG::Fiber::YieldToHold();
    SUPERG_LOG_INFO(logger_root)<<"jjjjj";


}
int main() {

//    superG::Logger::ptr logger(new superG::Logger);
//    logger->addAppender(superG::LogAppender::ptr (new superG::StdoutLogAppender));
////    superG::LogEvent::ptr event(new superG::LogEvent(logger,superG::LogLevel::DEBUG,__FILE_NAME__,__LINE__,superG::GetThreadId(),superG::GetFiberd(),0,
////                                                     time(0)));
////    event->getSS()<<"hello Gw";
////    logger->log(event,superG::LogLevel::DEBUG);
////    std::cout<<"0000";
//

//
//    SUPERG_LOG_LEVEL(logger,superG::LogLevel::DEBUG)<<"hello Gw";
//    SUPERG_LOG_ERROR(logger)<<"hello Gw";
//    SUPERG_LOG_INFO(logger)<<"hello Gw";
//    SUPERG_LOG_FATAL(logger)<<"hello Gw";
//    SUPERG_LOG_WARN(logger)<<"hello Gw";
//
//    superG::FileLogAppender::ptr p(new superG::FileLogAppender("/Users/gw/CLionProjects/HighPerformanceServer/LogDirectory/Log.txt"));
//    p->setLevel(superG::LogLevel::ERROR);
//
//    logger->addAppender(p);
//
//
//    SUPERG_LOG_FMT_DEBUG(logger,"Test DEBUG PRINT %s","HHHH");
//    SUPERG_LOG_FMT_DEBUG(logger,"Test DEBUG PRINT %s","AAAA");
//
//    SUPERG_LOG_FMT_ERROR(logger,"Test DEBUG PRINT %s","OOOO");
//
//    auto i=superG::LogMng::GetInstance()->getLogger("xx");
//    SUPERG_LOG_INFO(i)<<"xxx";

//   SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_int_value->getValue();
//    SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<p_int_value->toString();


//testyaml();






// testyaml();
//
//test_class();
//    test_logs();
//    superG::configure::Visit([](superG::ConfigVarBase::ptr pu)
//                             {
//                                 SUPERG_LOG_INFO(SUPERG_LOG_ROOT())<<pu->getName()<<"   "<<pu->getDescription();
//
//
//                             });

//    std::vector<superG::Thread::ptr> threadPool;
//    for (int i = 0; i < 5; i++) {
//        superG::Thread::ptr thread(new superG::Thread(&fun1, "name" + std::to_string(i)));
//        threadPool.push_back(thread);
//    }
//    for (int i = 0; i < 5; i++) {
//        threadPool[i]->join();
//    }
//
//   std::cout<<count<<std::endl;




//       test_assert();
        superG::Fiber::GetThis();
        SUPERG_LOG_INFO(logger_root)<<"ppppp";

       superG::Fiber::ptr  fiber(new superG::Fiber(runinFiber));
       fiber->SwapIn();
       fiber->SwapIn();
        return 0;



}

//#include <iostream>
//
//#define LOG(...)  printf(__VA_ARGS__);
//
//int main()
//{
//    LOG("score is %d\n",96);
//
//    getchar();
//    return 0;
//}

