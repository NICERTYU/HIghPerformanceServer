//
// Created by GW on 2023/12/19.
//
#include "config.h"
namespace superG
{

//    configure::ConfigureMap configure::m_configuremap;
     ConfigVarBase::ptr configure::LookupBase(const std::string& name)
    {
        RWmutexType::ReadLock lock(getMutex());
         auto it=getDatas().find(name);
        return it==getDatas().end()? nullptr:it->second;
    }

    static void ListAllMember(const std::string& prefix,const YAML::Node& node,std::list<std::pair<std::string ,const YAML::Node>>& output)
    {
        if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz0198273645_.")!=std::string::npos)
        {
            SUPERG_LOG_ERROR(SUPERG_LOG_ROOT())<<"config invalid name:"<<"  "<<prefix<<" : "<<node;
            return;
        }
        output.push_back(std::make_pair(prefix,node));
        if(node.IsMap())
        {
            for(auto  it=node.begin();it!=node.end();it++)
            {
                ListAllMember(prefix.empty()?it->first.Scalar():prefix+"."+it->first.Scalar(),it->second,output);
            }
        }


    }
    void configure::LoadFromYaml(const YAML::Node &root) {

        std::list<std::pair<std::string ,const YAML::Node>>  all_members;
        ListAllMember("",root,all_members);
        for(auto& i:all_members)
        {
             std::string str=i.first;

            if(str.empty())
            {
                continue;
            }
           std::transform(str.begin(),str.end(),str.begin(),::tolower);
           ConfigVarBase::ptr p= LookupBase(str);

            if(p)
            {
                //单一的值
                if(i.second.IsScalar())
                {



                    p->fromString(i.second.Scalar());
                }
                else
                {
                    //复合值
                    std::stringstream ss;
                    ss<<i.second;
//                    std::cout<<ss.str()<<std::endl;
                    p->fromString(ss.str());
                }
            }
        }

    }

    void configure::Visit(std::function<void(ConfigVarBase::ptr)> cb)
    {
        RWmutexType::ReadLock lock(getMutex());
        ConfigureMap& m=getDatas();
        for(auto i=m.begin();i!=m.end();i++)
        {
            cb(i->second);
        }

    }

}
