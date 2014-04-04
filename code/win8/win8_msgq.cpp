#include "win8_msgq.h"

#include <atomic>

namespace Q3Win8
{
    static const int c_numMsgs = 1024;
    static MSG g_msgBuf[c_numMsgs];
    static std::atomic<int> g_consumerCursor = 0;
    static std::atomic<int> g_producerCursor = 0;
    static std::atomic<int> g_producerReserve = 0;

    static __forceinline int WrapMsgIndex( int pos )
    {
        return pos % c_numMsgs;
    }

    static __forceinline MSG* GetMsgAt( int pos )
    {
        return &g_msgBuf[WrapMsgIndex(pos)];
    }

    void PostMessage( const MSG* msg )
    {
        int reservedSlot = g_producerReserve.fetch_add(1);
        if ( reservedSlot == WrapMsgIndex(g_consumerCursor) )
        {
            // Uh oh, we wrapped
            assert(0);
        }

        // Copy the message into the ringbuffer
        MSG* dst = GetMsgAt( reservedSlot );
        *dst = *msg;

        // Unlock the ringbuffer. Only the thread with the correct view of the 
        // producer cursor can unlock.
        int producerCursorView = reservedSlot - 1;
        while (!g_producerCursor.compare_exchange_strong(producerCursorView, reservedSlot)) 
        {
            assert(producerCursorView < reservedSlot);
            producerCursorView = reservedSlot - 1;
        }
    }

    bool PopMessage( MSG* msg )
    {
        // Empty queue
        if ( g_consumerCursor == g_producerCursor )
            return false;

        *msg = *GetMsgAt( g_consumerCursor.fetch_add(1) );
        return true;
    }
}
