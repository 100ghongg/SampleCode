#pragma once
#include "Default.h"
#include "LogUtils.h"

#define _MEGA_BYTE           1024*1024
#define _MAX_RINGBUFSIZE     300*_MEGA_BYTE

constexpr uint64_t  max_ringbuffer_size = _MAX_RINGBUFSIZE;

template<typename T>
class CRingBuffer
{
private:
    T*   ringbuffer_ = nullptr;
    std::atomic<int64_t> head_ = 0;     // Read data from thea head
    std::atomic<int64_t> tail_ = 0;     // Write data from the tail

    uint64_t ringbuffersize_ = 0;
    uint64_t max_datasize_ = 0;
    uint64_t datasize_ = 0;
public:
    bool    Init();
    T*      GetHeadBuffer(int64_t* readable);
    void    MoveHead(int64_t readed);
    T*      GetTailBuffer(int64_t* writable);
    void    MoveTail(int64_t writed);

    inline int64_t  GetHeadIdx() const { return head_.load(std::memory_order_acquire); }
    inline void  SetHeadIdx(int64_t readed) { head_.store(readed, std::memory_order_release); }
    inline int64_t  GetTailIdx() const { return tail_.load(std::memory_order_acquire); }
    inline void  SetTailIdx(int64_t writed) { tail_.store(writed, std::memory_order_release); }
    const uint64_t GetDatasize() { return datasize_; }

// for Test
public:
    void    PrintBuffer();
    void    Read(int cnt);
    void    Write(std::string str);
public:
    explicit CRingBuffer(uint64_t size) : ringbuffersize_(size) { Init(); };
    ~CRingBuffer();
	CRingBuffer(const CRingBuffer& rhs)			 = delete;
	CRingBuffer& operator=(const CRingBuffer& rhs) = delete;
};

template <typename T>
bool CRingBuffer<T>::Init() {
    if (!ringbuffersize_)
        return false;
    try {
        ringbuffer_ = new T[ringbuffersize_]{};
    } catch(std::exception& e) {
        printf("[ERROR] Alloc RingBuffer<%s>[%u] %s\n", typeid(T).name(), ringbuffersize_, e.what());
        return false;
    }
    printf("[INFO] Alloc RingBuffer<%s>[%u]\n", typeid(T).name(), ringbuffersize_);
    SetHeadIdx(0);
    SetTailIdx(0);
    datasize_ = 0;

    return true;
}

template <typename T>
T* CRingBuffer<T>::GetHeadBuffer(int64_t* readable) 
{
    int64_t tail = GetTailIdx();
    int64_t head = GetHeadIdx();

    if (tail < head) {
        *readable = ringbuffersize_ - head;
    }
    else {
        *readable = tail - head;
    }
    
    return ringbuffer_ + head;
}

template <typename T>
void CRingBuffer<T>::MoveHead(int64_t readed)
{
    int64_t next = GetHeadIdx() + readed;
    if (next >= ringbuffersize_) {
        next = next - ringbuffersize_;
    }
    SetHeadIdx(next);
    datasize_ -= readed;
}

template <typename T>
T* CRingBuffer<T>::GetTailBuffer(int64_t* writable)
{
    int64_t tail = GetTailIdx();
    int64_t head = GetHeadIdx();

    if (tail < head) {
        *writable = head - tail - 1;    // Do not use last buffer. MaxDataSize is RingBufferSize - 1
    }
    else {
        *writable = ringbuffersize_ - tail;
        if (head == 0) {
            *writable -= 1;
        }
    }

    return ringbuffer_ + tail;
}

template <typename T>
void CRingBuffer<T>::MoveTail(int64_t writed)
{
    int64_t next = GetTailIdx() + writed;
    if (next >= ringbuffersize_) {
        next = next - ringbuffersize_;
    }
    SetTailIdx(next);
    datasize_ += writed;
}

template <typename T>
CRingBuffer<T>::~CRingBuffer()
{
    if (ringbuffer_) {
        try {
            delete[] ringbuffer_;
            ringbuffer_ = nullptr;
        } catch(std::exception& e) {
            printf("[ERROR] Free RingBuffer<%s>[%u] &s\n", typeid(T).name(), ringbuffersize_, e.what());
            return;
        }
        printf("[INFO] Free RingBuffer<%s>[%u]\n", typeid(T).name(), ringbuffersize_);
    }
}

///////////////
// Test funcs
template <typename T>
void CRingBuffer<T>::PrintBuffer()
{
    std::cout << "Data : " << datasize_ << "  Head : " << GetHeadIdx() << "  Tail : " << GetTailIdx() << '\n';
    for(int i=0; i < ringbuffersize_; i++) {
        std::cout << " " << ringbuffer_[i] << " ";
    }
    std::cout << '\n';
}

template <typename T>
void CRingBuffer<T>::Read(int cnt)
{
    std::string str;
    char* buf = nullptr;
    int64_t read = 0;
    int trys = 0;
    while(cnt > 0) {
        buf = GetHeadBuffer(&read);
        if (read == 0) {
            std::cout << ++trys << " Can not read!\n";
            break;
        }
        read = read > cnt ? cnt : read;
        for(int i=0; i< read; i++) {
            str += buf[i];
            cnt--;
        }
        MoveHead(read);
        std::cout << ++trys << " Reading : " << str << '\n';
        PrintBuffer();
    }
}

template <typename T>
void CRingBuffer<T>::Write(std::string str)
{
    char* buf = nullptr;
    int64_t write = 0;
    int len = str.length();
    int trys = 0;
    while(len > 0) {
        buf = GetTailBuffer(&write);
        write = write > len ?  len : write;
        if (write == 0) {
            std::cout << ++trys << " Can not write!\n";
            break;
        }
        std::string writed{""};
        for(int i = 0; i < write; i++) {
            buf[i] = str[str.length() - len];
            writed += buf[i];
            len--;
        }
        MoveTail(write);
        std::cout << ++trys << " Writing : " << writed << '\n';
        PrintBuffer();
    }
}
