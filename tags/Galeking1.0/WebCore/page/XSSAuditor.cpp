/*
 * Copyright (C) 2008, 2009 Daniel Bates (dbates@intudata.com)
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
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
#include "XSSAuditor.h"

#include <wtf/StdLibExtras.h>
#include <wtf/Vector.h>

#include "CString.h"
#include "DocumentLoader.h"
#include "DOMWindow.h"
#include "Frame.h"
#include "KURL.h"
#include "PreloadScanner.h"
#include "ResourceResponseBase.h"
#include "ScriptSourceCode.h"
#include "Settings.h"
#include "TextResourceDecoder.h"

using namespace WTF;

#if ENABLE(INSPECTOR)
#include "Console.h"
#endif

namespace WebCore {

static bool isNonNullControlCharacter(UChar c)
{
    return (c > '\0' && c < ' ') || c == 127;
}

XSSAuditor::XSSAuditor(Frame* frame)
    : m_frame(frame)
{
}

XSSAuditor::~XSSAuditor()
{
}

bool XSSAuditor::isEnabled() const
{
    Settings* settings = m_frame->settings();
    return (settings && settings->xssAuditorEnabled());
}

bool XSSAuditor::canEvaluate(const String& sourceCode) const
{
    if (!isEnabled())
        return true;

    if (findInRequest(sourceCode, false, true, false)) {
#if ENABLE(INSPECTOR)
        DEFINE_STATIC_LOCAL(String, consoleMessage, ("Refused to execute a JavaScript script. Source code of script found within request.\n"));
        m_frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage, 1, String());
#endif
        return false;
    }
    return true;
}

bool XSSAuditor::canEvaluateJavaScriptURL(const String& code) const
{
    if (!isEnabled())
        return true;

    if (findInRequest(code, false, false, true, true)) {
#if ENABLE(INSPECTOR)
        DEFINE_STATIC_LOCAL(String, consoleMessage, ("Refused to execute a JavaScript script. Source code of script found within request.\n"));
        m_frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage, 1, String());
#endif
        return false;
    }
    return true;
}

bool XSSAuditor::canCreateInlineEventListener(const String&, const String& code) const
{
    if (!isEnabled())
        return true;

    if (findInRequest(code)) {
#if ENABLE(INSPECTOR)
        DEFINE_STATIC_LOCAL(String, consoleMessage, ("Refused to execute a JavaScript script. Source code of script found within request.\n"));
        m_frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage, 1, String());
#endif
        return false;
    }
    return true;
}

bool XSSAuditor::canLoadExternalScriptFromSrc(const String& context, const String& url) const
{
    if (!isEnabled())
        return true;

    if (findInRequest(context + url)) {
#if ENABLE(INSPECTOR)
        DEFINE_STATIC_LOCAL(String, consoleMessage, ("Refused to execute a JavaScript script. Source code of script found within request.\n"));
        m_frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage, 1, String());
#endif
        return false;
    }
    return true;
}

bool XSSAuditor::canLoadObject(const String& url) const
{
    if (!isEnabled())
        return true;

    if (findInRequest(url, false, false)) {
#if ENABLE(INSPECTOR)
        DEFINE_STATIC_LOCAL(String, consoleMessage, ("Refused to execute a JavaScript script. Source code of script found within request"));
        m_frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage, 1, String());
#endif
        return false;
    }
    return true;
}

bool XSSAuditor::canSetBaseElementURL(const String& url) const
{
    if (!isEnabled())
        return true;
    
    KURL baseElementURL(m_frame->document()->url(), url);
    if (m_frame->document()->url().host() != baseElementURL.host() && findInRequest(url)) {
#if ENABLE(INSPECTOR)
        DEFINE_STATIC_LOCAL(String, consoleMessage, ("Refused to execute a JavaScript script. Source code of script found within request"));
        m_frame->domWindow()->console()->addMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage, 1, String());
#endif
        return false;
    }
    return true;
}

String XSSAuditor::decodeURL(const String& str, const TextEncoding& encoding, bool allowNullCharacters,
                             bool allowNonNullControlCharacters, bool decodeHTMLentities, bool leaveUndecodableHTMLEntitiesUntouched)
{
    String result;
    String url = str;

    url.replace('+', ' ');
    result = decodeURLEscapeSequences(url);
    String decodedResult = encoding.decode(result.utf8().data(), result.length());
    if (!decodedResult.isEmpty())
        result = decodedResult;
    if (decodeHTMLentities)
        result = decodeHTMLEntities(result, leaveUndecodableHTMLEntitiesUntouched);
    if (!allowNullCharacters)
        result = StringImpl::createStrippingNullCharacters(result.characters(), result.length());
    if (!allowNonNullControlCharacters) {
        decodedResult = result.removeCharacters(&isNonNullControlCharacter);
        if (!decodedResult.isEmpty())
            result = decodedResult;
    }
    return result;
}

String XSSAuditor::decodeHTMLEntities(const String& str, bool leaveUndecodableHTMLEntitiesUntouched)
{
    SegmentedString source(str);
    SegmentedString sourceShadow;
    Vector<UChar> result;
    
    while (!source.isEmpty()) {
        UChar cc = *source;
        source.advance();
        
        if (cc != '&') {
            result.append(cc);
            continue;
        }
        
        if (leaveUndecodableHTMLEntitiesUntouched)
            sourceShadow = source;
        bool notEnoughCharacters = false;
        unsigned entity = PreloadScanner::consumeEntity(source, notEnoughCharacters);
        // We ignore notEnoughCharacters because we might as well use this loop
        // to copy the remaining characters into |result|.

        if (entity > 0xFFFF) {
            result.append(U16_LEAD(entity));
            result.append(U16_TRAIL(entity));
        } else if (entity && (!leaveUndecodableHTMLEntitiesUntouched || entity != 0xFFFD)){
            result.append(entity);
        } else {
            result.append('&');
            if (leaveUndecodableHTMLEntitiesUntouched)
                source = sourceShadow;
        }
    }
    
    return String::adopt(result);
}

bool XSSAuditor::findInRequest(const String& string, bool matchNullCharacters, bool matchNonNullControlCharacters,
                               bool decodeHTMLentities, bool leaveUndecodableHTMLEntitiesUntouched) const
{
    bool result = false;
    Frame* parentFrame = m_frame->tree()->parent();
    if (parentFrame && m_frame->document()->url() == blankURL())
        result = findInRequest(parentFrame, string, matchNullCharacters, matchNonNullControlCharacters, 
                               decodeHTMLentities, leaveUndecodableHTMLEntitiesUntouched);
    if (!result)
        result = findInRequest(m_frame, string, matchNullCharacters, matchNonNullControlCharacters, 
                               decodeHTMLentities, leaveUndecodableHTMLEntitiesUntouched);
    return result;
}

bool XSSAuditor::findInRequest(Frame* frame, const String& string, bool matchNullCharacters, bool matchNonNullControlCharacters,
                               bool decodeHTMLentities, bool leaveUndecodableHTMLEntitiesUntouched) const
{
    ASSERT(frame->document());
    String pageURL = frame->document()->url().string();

    if (!frame->document()->decoder()) {
        // Note, JavaScript URLs do not have a charset.
        return false;
    }

    if (protocolIs(pageURL, "data"))
        return false;

    if (string.isEmpty())
        return false;

    if (string.length() < pageURL.length()) {
        // The string can actually fit inside the pageURL.
        String decodedPageURL = decodeURL(pageURL, frame->document()->decoder()->encoding(), matchNullCharacters,
                                          matchNonNullControlCharacters, decodeHTMLentities, leaveUndecodableHTMLEntitiesUntouched);
        if (decodedPageURL.find(string, 0, false) != -1)
           return true;  // We've found the smoking gun.
    }

    FormData* formDataObj = frame->loader()->documentLoader()->originalRequest().httpBody();
    if (formDataObj && !formDataObj->isEmpty()) {
        String formData = formDataObj->flattenToString();
        if (string.length() < formData.length()) {
            // Notice it is sufficient to compare the length of the string to
            // the url-encoded POST data because the length of the url-decoded
            // code is less than or equal to the length of the url-encoded
            // string.
            String decodedFormData = decodeURL(formData, frame->document()->decoder()->encoding(), matchNullCharacters,
                                               matchNonNullControlCharacters, decodeHTMLentities, leaveUndecodableHTMLEntitiesUntouched);
            if (decodedFormData.find(string, 0, false) != -1)
                return true;  // We found the string in the POST data.
        }
    }

    return false;
}

} // namespace WebCore

