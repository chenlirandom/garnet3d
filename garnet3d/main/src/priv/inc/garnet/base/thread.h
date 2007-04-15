#ifndef __GN_BASE_THREAD_H__
#define __GN_BASE_THREAD_H__
// *****************************************************************************
//! \file    base/thread.h
//! \brief   platform independent thread wrapper
//! \author  chen@@CHENLI-HOMEPC (2007.4.8)
// *****************************************************************************

namespace GN
{
    ///
    /// special float number that indicates infinite time.
    ///
    extern const float INFINITE_TIME; // = 1.0e38f;

    ///
    /// thread priority
    ///
    enum ThreadPriority
    {
        TP_REALTIME,  ///< The highest priority, for time-critical task only.
        TP_HIGH,      ///< high priority
        TP_NORMAL,    ///< normal priority, suitable for most of application.
        TP_LOW,       ///< low priority
        TP_IDLE,      ///< The lowest priority
        NUM_THREAD_PRIORITIES, ///< number of thread priorities
    };

    ///
    /// thread procedure functor
    ///
    typedef Delegate1<UInt32,void*> ThreadProcedure;

    ///
    /// Mutex lock, the lightest sync object.
    ///
    class Mutex
    {
        UInt32 mInternal[16]; ///< store platform dependata data here, no runtime heap allocation

    public:

        //@{
        Mutex();
        ~Mutex();
        //@}

        //@{
        bool trylock();
        void lock();
        void unlock();
        //@}
    };

    ///
    /// Abstract semaphore interface
    ///    
    struct Semaphore : public NoCopy
    {
        virtual bool wait( float seconds = INFINITE_TIME ) = 0; ///< block calling thread, until the semaphore is available.
        virtual void wake() = 0; ///< wake up one thread that is waiting for this semaphore.

        /// \name aliases for P/V operations
        //@{
        bool P( float seconds = INFINITE_TIME ) { return wait( seconds ); }
        void V() { return wake(); }
        //@}
    };

    ///
    /// abstract thread interface
    ///
    struct Thread : public NoCopy
    {
        /// \name thread properties
        //@{
        virtual ThreadPriority getPriority() const = 0;
        virtual void           setPriority( ThreadPriority ) = 0;
        virtual bool           isCurrentThread() const = 0;
        //@}

        /// \name thread operations
        //@{
        virtual void suspend() = 0;
        virtual void resume() = 0; //< resume suspended and sleeping thread.

        ///
        /// wait for termination of the thread.
        ///
        virtual bool waitForTermination( float seconds = INFINITE_TIME, UInt32 * threadProcReturnValue = 0 ) = 0;

        ///
        /// signal of thread termination
        ///
        Signal0<void> sigTermination;

        //@}
    };

    //@{

    Semaphore * createSemaphore( size_t maxcount, size_t initialcount, const char * name = 0 );

    Thread * createThread(
        const ThreadProcedure & proc,
        void * param,
        ThreadPriority priority,
        bool initialSuspended = false,
        const char * name = 0 );

    void sleepCurrentThread( float seconds );

    // Thread * getCurrentThread();

    //@}

    /*// \name atomic operations
    //@{

    atomread32(...);
    atomwrite32(...);
    atominc32(...);
    atomdec32(...);
    atomxchg32(...);
    atomcx32(...);

    //@}*/
}

// *****************************************************************************
//                           End of thread.h
// *****************************************************************************
#endif // __GN_BASE_THREAD_H__