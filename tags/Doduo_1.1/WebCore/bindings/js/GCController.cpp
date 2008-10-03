/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "GCController.h"

#include "JSDOMWindow.h"
#include <kjs/JSGlobalData.h>
#include <kjs/JSLock.h>
#include <kjs/collector.h>

#if USE(PTHREADS)
#include <pthread.h>
#endif

#include <wtf/TimeCounter.h>
static WTF::TimeCounter garbageCollectorCounter("garbage collector", true);

using namespace KJS;

namespace WebCore {

#if USE(PTHREADS)

static void* collect(void*)
{
    garbageCollectorCounter.startCounting();
    JSLock lock(false);
    JSDOMWindow::commonJSGlobalData()->heap->collect();
    garbageCollectorCounter.stopCounting();
    return 0;
}

#endif

GCController& gcController()
{
    static GCController staticGCController;
    return staticGCController;
}

GCController::GCController()
    : m_GCTimer(this, &GCController::gcTimerFired)
{
}

void GCController::garbageCollectSoon()
{
    if (!m_GCTimer.isActive())
        m_GCTimer.startOneShot(0);
}

void GCController::gcTimerFired(Timer<GCController>*)
{
    garbageCollectorCounter.startCounting();
    JSLock lock(false);
    JSDOMWindow::commonJSGlobalData()->heap->collect();
    garbageCollectorCounter.stopCounting();
}

void GCController::garbageCollectNow()
{
    garbageCollectorCounter.startCounting();
    JSLock lock(false);
    JSDOMWindow::commonJSGlobalData()->heap->collect();
    garbageCollectorCounter.stopCounting();
}

void GCController::garbageCollectOnAlternateThreadForDebugging(bool waitUntilDone)
{
#if USE(PTHREADS)
    pthread_t thread;
    pthread_create(&thread, NULL, collect, NULL);

    if (waitUntilDone)
        pthread_join(thread, NULL);
#endif
}

} // namespace WebCore
