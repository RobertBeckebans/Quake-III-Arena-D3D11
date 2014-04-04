#include "win8_msgq.h"

#include <assert.h>

namespace Win8
{
    MessageQueue::MessageQueue()
    {
    }

    void MessageQueue::Post( const MSG* msg )
    {
        assert( msg->TimeStamp != 0 );
        m_msgs.push( *msg );
    }

    bool MessageQueue::Pop( MSG* msg )
    {
        return m_msgs.try_pop( *msg );
    }
}
