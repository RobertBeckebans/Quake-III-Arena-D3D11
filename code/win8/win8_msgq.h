#pragma once

#include <stdint.h>
#include <atomic>

namespace Q3Win8
{

struct MSG
{
    size_t Message;
    size_t Param0;
    size_t Param1;
};

class MessageQueue
{
public:
    MessageQueue();
    void Post( const MSG* msg );
    bool Pop( MSG* msg );

private:
    __forceinline int WrapMsgIndex( int pos ) const
    {
        return pos % c_numMsgs;
    }

    __forceinline MSG* GetMsgAt( int pos )
    {
        return &m_msgBuf[WrapMsgIndex(pos)];
    }
  
    static const int c_numMsgs = 1024;
    MSG m_msgBuf[c_numMsgs];
    std::atomic<int> m_consumerCursor;
    std::atomic<int> m_producerCursor;
    std::atomic<int> m_producerReserve;
};

}
