#pragma once
#include <Windows.h>
#include "hjk_Atomic.h"
#include "hjk_ScopedLock.h"
/**
    A simple spin-lock class that can be used as a simple, low-overhead mutex for
    uncontended situations.

    Note that unlike a CriticalSection, this type of lock is not re-entrant, and may
    be less efficient when used in a highly contended situation, but it's very small and
    requires almost no initialisation.
    It's most appropriate for simple situations where you're only going to hold the
    lock for a very brief time.

    @see CriticalSection

    @tags{Core}
*/
class SpinLock
{
public:
    inline SpinLock() = default;
    inline ~SpinLock() = default;

    /** Acquires the lock.
        This will block until the lock has been successfully acquired by this thread.
        Note that a SpinLock is NOT re-entrant, and is not smart enough to know whether the
        caller thread already has the lock - so if a thread tries to acquire a lock that it
        already holds, this method will never return!

        It's strongly recommended that you never call this method directly - instead use the
        ScopedLockType class to manage the locking using an RAII pattern instead.
    */
    void enter() const noexcept
    {
        if (!tryEnter())
        {
            for (int i = 20; --i >= 0;)
                if (tryEnter())
                    return;

            while (!tryEnter())
              // Thread::yield();
                Sleep(0);
        }
    }

    /** Attempts to acquire the lock, returning true if this was successful. */
    inline bool tryEnter() const noexcept
    {
        return lock.compareAndSetBool(1, 0);
    }

    /** Releases the lock. */
    inline void exit() const noexcept
    {
        //@hjk debug flag
 //jassert(lock.get() == 1); // Agh! Releasing a lock that isn't currently held!
        lock = 0;
    }

    //==============================================================================
    /** Provides the type of scoped lock to use for locking a SpinLock. */
    using ScopedLockType = GenericScopedLock<SpinLock>;

    /** Provides the type of scoped unlocker to use with a SpinLock. */
    using ScopedUnlockType = GenericScopedUnlock<SpinLock>;

private:
    //==============================================================================
    mutable Atomic<int> lock;

    JUCE_DECLARE_NON_COPYABLE(SpinLock)
};
