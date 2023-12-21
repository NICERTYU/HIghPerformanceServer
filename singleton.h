//
// Created by GW on 2023/12/19.
//

#ifndef HIGHPERFORMANCESERVER_SINGLETON_H
#define HIGHPERFORMANCESERVER_SINGLETON_H
#include <string>
namespace superG
{
    template<class T,class X=void,int N=0>
    class Singleton
    {
    public:
        static T* GetInstance()
        {
            static T v;
            return &v;
        }
    };

    template<class T,class X=void,int N=0>
    class SingletonPtr
    {
    public:
        static std::shared_ptr<T> GetInstance()
        {
            static std::shared_ptr<T> p(new T);
            return p;
        }
    };
}

#endif //HIGHPERFORMANCESERVER_SINGLETON_H
