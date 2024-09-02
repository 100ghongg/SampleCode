#pragma once

#include <memory>
#include <mutex>

/*
    * Singleton ver Ghong.
    ! requrie : c++ minimun version 11
*/
template<typename T>
class Singleton
{
private:
    static std::unique_ptr<T> instance;
    static std::once_flag     once;
    
    void Init();
    void Release();

// C++ default
public:
    Singleton(){ Init(); };
    ~Singleton(){ Release(); };
    
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;

// Functions
static std::unique_ptr<T>& getInstance();
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance;
template<typename T>
std::once_flag Singleton<T>::once;

template<typename T>
std::unique_ptr<T>& Singleton<T>::getInstance()
{
    std::call_once(once, []() {
        Singleton<T>::instance.reset(new T);
        std::cout << "Singleton<" << typeid(T).name() << "> Init\n";
    });
    return Singleton<T>::instance;
}

template<typename T>
void Singleton<T>::Init()
{
    std::cout << "Singleton<" << typeid(T).name() << "> Init\n";
}

template<typename T>
void Singleton<T>::Release()
{
    std::cout << "Singleton<" << typeid(T).name() << "> Release\n";
}
