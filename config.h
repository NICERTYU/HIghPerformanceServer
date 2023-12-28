//
// Created by GW on 2023/12/19.
//

#ifndef HIGHPERFORMANCESERVER_CONFIG_H
#define HIGHPERFORMANCESERVER_CONFIG_H
#include<memory>
#include<sstream>
#include<boost/lexical_cast.hpp>
#include<string>
#include "logger.h"
#include "yaml-cpp/yaml.h"
#include "yaml.h"
#include <unordered_set>
#include <functional>
#include "logger.h"
namespace superG
{
    template<class T,class F>
            class LexicalCast
            {
            public:
                T operator ()(const F& val)
                {

                 return   boost::lexical_cast<T>(val);

                }
            };

    template<class T>
            class LexicalCast<std::vector<T>,std::string>
            {
            public:
                std::vector<T> operator ()(const std::string& val)
                {
                    std::stringstream ss;
                    YAML::Node node=YAML::Load(val);
                    typename std::vector<T> vec;
                    for(int i=0;i<node.size();i++)
                    {
                        ss.str("");
                        ss<<node[i];
                        vec.push_back(LexicalCast<T,std::string>()(ss.str()));
                    }
                    return vec;



                }
            };
    template<class T>
    class LexicalCast<std::string ,std::vector<T> >
    {
    public:
        std::string operator ()(const std::vector<T>& val)
        {

            YAML::Node node;
            for(int i=0;i<val.size();i++)
            {
                node.push_back( YAML::Load(LexicalCast<std::string,T>()(val[i])));
            }

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
    template<class T>
    class LexicalCast<std::list<T>,std::string>
    {
    public:
        std::list<T> operator ()(const std::string& val)
        {
            std::stringstream ss;
            YAML::Node node=YAML::Load(val);
            typename std::list<T> vec;
            for(int i=0;i<node.size();i++)
            {
                ss.str("");
                ss<<node[i];
                vec.push_back(LexicalCast<T,std::string>()(ss.str()));
            }
            return vec;



        }
    };
    template<class T>
    class LexicalCast<std::string ,std::list<T> >
    {
    public:
        std::string operator ()(const std::list<T>& lst)
        {

            YAML::Node node;
            for(auto &i : lst) {
                node.push_back(YAML::Load(LexicalCast<std::string, T>()(i)));
            }

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
    template<class T>
    class LexicalCast<std::set<T>,std::string>
    {
    public:
        std::set<T> operator ()(const std::string& val)
        {
            std::stringstream ss;
            YAML::Node node=YAML::Load(val);
            typename std::set<T> vec;
            for(int i=0;i<node.size();i++)
            {
                ss.str("");
                ss<<node[i];
                vec.insert(LexicalCast<T,std::string>()(ss.str()));
            }
            return vec;



        }
    };
    template<class T>
    class LexicalCast<std::string ,std::set<T> >
    {
    public:
        std::string operator ()(const std::set<T>& lst)
        {

            YAML::Node node;
            for(auto &i : lst) {
                node.push_back(YAML::Load(LexicalCast<std::string, T>()(i)));
            }

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
    template<class T>
    class LexicalCast<std::unordered_set<T>,std::string>
    {
    public:
        std::unordered_set<T> operator ()(const std::string& val)
        {
            std::stringstream ss;
            YAML::Node node=YAML::Load(val);
            typename std::unordered_set<T> vec;
            for(int i=0;i<node.size();i++)
            {
                ss.str("");
                ss<<node[i];
                vec.insert(LexicalCast<T,std::string>()(ss.str()));
            }
            return vec;



        }
    };
    template<class T>
    class LexicalCast<std::string ,std::unordered_set<T> >
    {
    public:
        std::string operator ()(const std::unordered_set<T>& lst)
        {

            YAML::Node node;
            for(auto &i : lst) {
                node.push_back(YAML::Load(LexicalCast<std::string, T>()(i)));
            }

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
    template<class T>
    class LexicalCast<std::map<std::string,T>,std::string>
    {
    public:
        std::map<std::string ,T> operator ()(const std::string& val)
        {
            std::stringstream ss;
            YAML::Node node=YAML::Load(val);
            typename std::map<std::string,T> vec;
            for(auto it=node.begin();it!=node.end();it++) {


                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<T, std::string>()(ss.str())));
            }

            return vec;



        }
    };
    template<class T>
    class LexicalCast<std::string ,std::map<std::string,T> >
    {
    public:
        std::string operator ()(const std::map<std::string,T>& lst)
        {

            YAML::Node node;
            for(auto &i : lst) {
                node[i.first]=LexicalCast<std::string ,T>()(i.second);
            }

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
    template<class T>
    class LexicalCast<std::unordered_map<std::string,T>,std::string>
    {
    public:
        std::unordered_map<std::string ,T> operator ()(const std::string& val)
        {
            std::stringstream ss;
            YAML::Node node=YAML::Load(val);
            typename std::unordered_map<std::string,T> vec;
            for(auto it=node.begin();it!=node.end();it++) {


                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),LexicalCast<T, std::string>()(ss.str())));
            }

            return vec;



        }
    };
    template<class T>
    class LexicalCast<std::string ,std::unordered_map<std::string,T> >
    {
    public:
        std::string operator ()(const std::unordered_map<std::string,T>& lst)
        {

            YAML::Node node;
            for(auto &i : lst) {
                node[i.first]=LexicalCast<std::string ,T>()(i.second);
            }

            std::stringstream ss;
            ss<<node;
            return ss.str();





        }
    };
    class ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        ConfigVarBase(const std::string& name,const std::string& description):m_name(name),m_description(description){

            std::transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);
        };
        virtual ~ConfigVarBase(){};
        virtual std::string toString()=0;
        virtual std::string getTypeName()=0;

        virtual bool fromString(const std::string& val)=0;
        const std::string& getName()const {return m_name;};
        const std::string& getDescription()const {return m_description;};



    protected:
        std::string m_name;
        std::string m_description;

    };

    template<class T,class StrToV=LexicalCast<T,std::string>,class VToStr=LexicalCast<std::string,T>>
    class ConfigVar:public ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVar> ptr;
        typedef std::function<void (const T& old_value,const T& new_value)> on_change_callback;
        ConfigVar(const std::string& name,const std::string& description,const T& t): ConfigVarBase(name,description),m_val(t){};
         std::string toString () override
         {
             try {
                 return VToStr()(m_val);

             }
             catch(std::exception& e)
             {

                 SUPERG_LOG_ERROR(SUPERG_LOG_ROOT())<<"ConfigVar::toString Exception"
                 <<e.what()<<"convert::"<< typeid(m_val).name()<<" to std::string";
             }
             return "";
         };
         bool fromString(const std::string& val) override{

             try {
                 T temp=StrToV()(val);
                 setValue(temp);
                 m_val=temp;


                 return true;

             }
             catch(std::exception& e)
             {
                 SUPERG_LOG_ERROR(SUPERG_LOG_ROOT())<<"ConfigVar::toString Exception"
                                                    <<e.what()<<"convert:: string to "<< typeid(m_val).name();
             }

             return false;

         };
         T getValue() const{ return m_val;};
         void setValue(const T& t){


             if(m_val==t)
             {
             return;
             }


             for(auto& it:m_cbs)
             {
                 it.second(m_val,t);
             }


         };
        virtual std::string getTypeName() override{ return typeid(T).name();};
        void addListener(uint64_t  key,on_change_callback cb)
        {
            m_cbs[key]=cb;
        }
        on_change_callback getListener(uint64_t key)
        {
            auto it=m_cbs.find(key);
             return    it==m_cbs.end()? nullptr:it->second;
        }
        void removeListener(uint64_t key)
        {
            m_cbs.erase(key);
        }


    private:

        T m_val;
        std::map<uint64_t ,on_change_callback > m_cbs;
    };
    class configure
    {
    public:
        typedef std::map<std::string ,ConfigVarBase::ptr > ConfigureMap;
        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string& name,const T& default_value
        ,const std::string& description)
        {
//            auto it= Lookup<T>(name);
            auto it=getDatas().find(name);


            if(it!=getDatas().end())
            {
                auto val=std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                if(val) {
                    SUPERG_LOG_INFO(SUPERG_LOG_ROOT()) << "Lookup  " << name << "  exists";
                    return val;
                }
                else
                {
                    SUPERG_LOG_ERROR(SUPERG_LOG_ROOT()) << "Lookup  " << name << "  exists but type not right  "<<" the real type is "
                    << it->second->getTypeName();
                    return nullptr;

                }
            }

            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz0198273645_.")!=std::string::npos)
            {
                SUPERG_LOG_ERROR(SUPERG_LOG_ROOT())<<"Lookup name invalid"<<name;
                throw std::invalid_argument(name);

            }

            typename ConfigVar<T>::ptr p(new ConfigVar<T>(name,description,default_value));
            getDatas()[name]=p;
            return p;
        }



        template<class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string& name)
        {
            auto it=getDatas().find(name);
            if(it==getDatas().end())
            {
                return nullptr;
            }

            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
        static void LoadFromYaml(const YAML::Node& root);



         static ConfigVarBase::ptr LookupBase(const std::string& name);

    private:
        static ConfigureMap& getDatas()
                {


                      static ConfigureMap m_configuremap;
                        return m_configuremap;
                }
    };

}



#endif //HIGHPERFORMANCESERVER_CONFIG_H
