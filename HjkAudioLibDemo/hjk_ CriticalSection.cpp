#include "hjk_ CriticalSection.h"
#include <windows.h>

CriticalSection::CriticalSection() noexcept
{
    // (just to check the MS haven't changed this structure and broken things...)
    static_assert (sizeof(CRITICAL_SECTION) <= sizeof(lock),
        "win32 lock array too small to hold CRITICAL_SECTION: please report this JUCE bug!");

    InitializeCriticalSection((CRITICAL_SECTION*)lock);
}

CriticalSection::~CriticalSection() noexcept { DeleteCriticalSection((CRITICAL_SECTION*)lock); }
void CriticalSection::enter() const noexcept { EnterCriticalSection((CRITICAL_SECTION*)lock); }
bool CriticalSection::tryEnter() const noexcept { return TryEnterCriticalSection((CRITICAL_SECTION*)lock) != FALSE; }
void CriticalSection::exit() const noexcept { LeaveCriticalSection((CRITICAL_SECTION*)lock); }
