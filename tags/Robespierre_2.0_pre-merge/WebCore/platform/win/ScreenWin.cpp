/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
#include "Screen.h"

#include "IntRect.h"
#include "FloatRect.h"
#include "Frame.h"
#include "FrameView.h"
#include "Page.h"
#include <windows.h>

namespace WebCore {

FloatRect scaleScreenRectToPageCoordinates(const FloatRect& rect, const Page*)
{
    return rect;
}

FloatRect scalePageRectToScreenCoordinates(const FloatRect& rect, const Page*)
{
    return rect;
}

static MONITORINFOEX monitorInfo(Widget* widget)
{
    HMONITOR monitor = MonitorFromWindow(widget->containingWindow(), MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEX info;
    info.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(monitor, &info);
    return info;
}

FloatRect screenRect(Widget* widget)
{
    return monitorInfo(widget).rcMonitor;
}

FloatRect screenAvailableRect(Widget* widget)
{
    // FIXME: I have no idea if this is correct
    return monitorInfo(widget).rcWork;
}

int screenDepth(Widget*)
{
    DEVMODE deviceInfo;
    deviceInfo.dmSize = sizeof(DEVMODE);
    deviceInfo.dmDriverExtra = 0;
    EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &deviceInfo);
    return deviceInfo.dmBitsPerPel;
}

} // namespace WebCore
