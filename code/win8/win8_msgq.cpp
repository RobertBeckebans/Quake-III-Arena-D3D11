#include "win8_msgq.h"

namespace Q3Win8
{
    MessageQueue::MessageQueue()
        : m_consumerCursor(0)
        , m_producerCursor(0)
        , m_producerReserve(0)
    {
    }

    void MessageQueue::Post( const MSG* msg )
    {
        assert( msg->TimeStamp > 0 );

        int reservedSlot = m_producerReserve.fetch_add(1);

        // Copy the message into the ringbuffer
        MSG* dst = GetMsgAt( reservedSlot );
        *dst = *msg;

        // Unlock the ringbuffer. Only the thread with the correct view of the 
        // producer cursor can unlock.
        int producerCursorView = reservedSlot;
        while (!m_producerCursor.compare_exchange_strong(producerCursorView, reservedSlot + 1)) 
        {
            assert(producerCursorView < reservedSlot);
            producerCursorView = reservedSlot + 1;
        }
    }

    bool MessageQueue::Pop( MSG* msg )
    {
        // Empty queue
        if ( m_consumerCursor == m_producerCursor )
            return false;

        *msg = *GetMsgAt( m_consumerCursor.fetch_add(1) );
        return true;
    }
}
