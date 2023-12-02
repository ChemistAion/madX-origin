export module madX.Core.Singleton;

import stl.core;
import stl.memory;
import stl.threading;

export namespace madX::Core
{
    template <typename Type>
    class Singleton
    {
    private:
        static std::atomic<std::weak_ptr<Type>> singleton_;
        static std::mutex lock_;

        struct Deleter
        {
            void operator ()(Type* ptr)
            {
                delete ptr;
                ptr = nullptr;
            }
        };
        
        static Deleter deleter_;
    protected:
        Singleton() = default;
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton) = delete;

        static std::shared_ptr<Type> GetInstance();
        //  static std::size_t GetInstanceCount();
    };
}
    
module: private;

//  //TODO: C++20 has it
template <typename Type>
std::atomic<std::weak_ptr<Type>> madX::Core::Singleton<Type>::singleton_;

template <typename Type>
std::mutex madX::Core::Singleton<Type>::lock_;

template <typename Type>
typename madX::Core::Singleton<Type>::Deleter madX::Core::Singleton<Type>::deleter_;

template <typename Type>
std::shared_ptr<Type> madX::Core::Singleton<Type>::GetInstance()
{
    std::shared_ptr<Type> instance;
    
    if (instance = singleton_.load(std::memory_order_acquire).lock(); instance)
    {
        return instance;
    }
    
    const std::lock_guard<std::mutex> lock(lock_);
    
    //TODO: try/catch
    
    if (instance = singleton_.load(std::memory_order_relaxed).lock(); !instance)
    {
        instance.reset(new Type(), deleter_);
        singleton_.store(instance, std::memory_order_release);
    }
    
    return instance;
}

//	template <typename Type>
//	std::size_t Singleton<Type>::GetInstanceCount()
//	{
//	    return singleton_.load().use_count();
//	}