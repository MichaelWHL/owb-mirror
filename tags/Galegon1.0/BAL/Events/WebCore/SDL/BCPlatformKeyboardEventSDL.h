/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PlatformKeyboardEvent_h
#define PlatformKeyboardEvent_h

#include "PlatformString.h"
#include <wtf/Platform.h>
#include "BALBase.h"

namespace WebCore {

    class PlatformKeyboardEvent {
    public:
        enum Type {
            // KeyDown is sent by platforms such as Mac OS X, gtk and Qt, and has information about both physical pressed key, and its translation.
            // For DOM processing, it needs to be disambiguated as RawKeyDown or Char event.
            KeyDown,

            // KeyUp is sent by all platforms.
            KeyUp,

            // These events are sent by platforms such as Windows and wxWidgets. RawKeyDown only has information about a physical key, and Char
            // only has information about a character it was translated into.
            RawKeyDown,
            Char
        };

        enum ModifierKey {
            AltKey = 1 << 0,
            CtrlKey = 1 << 1,
            MetaKey = 1 << 2,
            ShiftKey = 1 << 3,
        };

        Type type() const { return m_type; }
        void disambiguateKeyDownEvent(Type, bool backwardCompatibilityMode = false); // Only used on platforms that need it, i.e. those that generate KeyDown events.

        // Text as as generated by processing a virtual key code with a keyboard layout
        // (in most cases, just a character code, but the layout can emit several
        // characters in a single keypress event on some platforms).
        // This may bear no resemblance to the ultimately inserted text if an input method
        // processes the input.
        // Will be null for KeyUp and RawKeyDown events.
        String text() const { return m_text; }

        // Text that would have been generated by the keyboard if no modifiers were pressed
        // (except for Shift); useful for shortcut (accelerator) key handling.
        // Otherwise, same as text().
        String unmodifiedText() const { return m_unmodifiedText; }

        // Most compatible Windows virtual key code associated with the event.
        // Zero for Char events.
        int windowsVirtualKeyCode() const { return m_windowsVirtualKeyCode; }
        void setWindowsVirtualKeyCode(int code) { m_windowsVirtualKeyCode = code; }

        String keyIdentifier() const { return m_keyIdentifier; }
        bool isAutoRepeat() const { return m_autoRepeat; }
        void setIsAutoRepeat(bool in) { m_autoRepeat = in; }
        bool isKeypad() const { return m_isKeypad; }
        bool shiftKey() const { return m_shiftKey; }
        bool ctrlKey() const { return m_ctrlKey; }
        bool altKey() const { return m_altKey; }
        bool metaKey() const { return m_metaKey; }
        unsigned modifiers() const {
            return (altKey() ? AltKey : 0)
                | (ctrlKey() ? CtrlKey : 0)
                | (metaKey() ? MetaKey : 0)
                | (shiftKey() ? ShiftKey : 0);
        }

        static bool currentCapsLockState();

        PlatformKeyboardEvent(BalEventKey*);
        BalEventKey* balEventKey() const;

    private:
        Type m_type;
        String m_text;
        String m_unmodifiedText;
        String m_keyIdentifier;
        bool m_autoRepeat;
        int m_windowsVirtualKeyCode;
        bool m_isKeypad;
        bool m_shiftKey;
        bool m_ctrlKey;
        bool m_altKey;
        bool m_metaKey;

        BalEventKey* m_balEventKey;
    };

} // namespace WebCore

#endif // PlatformKeyboardEvent_h
