/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "WebContextMenuClient.h"

#import "WebElementDictionary.h"
#import "WebFrame.h"
#import "WebFrameInternal.h"
#import "WebHTMLView.h"
#import "WebHTMLViewInternal.h"
#import "WebKitVersionChecks.h"
#import "WebNSPasteboardExtras.h"
#import "WebUIDelegate.h"
#import "WebUIDelegatePrivate.h"
#import "WebView.h"
#import "WebViewFactory.h"
#import "WebViewInternal.h"
#import <WebCore/ContextMenu.h>
#import <WebCore/KURL.h>
#import <WebKit/DOMPrivate.h>

using namespace WebCore;

@interface NSApplication (AppKitSecretsIKnowAbout)
- (void)speakString:(NSString *)string;
@end

WebContextMenuClient::WebContextMenuClient(WebView *webView) 
    : m_webView(webView)
{
}

void WebContextMenuClient::contextMenuDestroyed()
{
    delete this;
}

static BOOL isAppleMail(void)
{
    return [[[NSBundle mainBundle] bundleIdentifier] isEqualToString:@"com.apple.mail"];
}

static BOOL isPreVersion3Client(void)
{
    static BOOL preVersion3Client = !WebKitLinkedOnOrAfter(WEBKIT_FIRST_VERSION_WITH_3_0_CONTEXT_MENU_TAGS);
    return preVersion3Client;
}

static BOOL isPreInspectElementTagClient(void)
{
    static BOOL preInspectElementTagClient = !WebKitLinkedOnOrAfter(WEBKIT_FIRST_VERSION_WITH_INSPECT_ELEMENT_MENU_TAG);
    return preInspectElementTagClient;
}

static NSMutableArray *fixMenusToSendToOldClients(NSMutableArray *defaultMenuItems)
{
    NSMutableArray *savedItems = nil;

    unsigned defaultItemsCount = [defaultMenuItems count];

    if (isPreInspectElementTagClient() && defaultItemsCount >= 2) {
        NSMenuItem *secondToLastItem = [defaultMenuItems objectAtIndex:defaultItemsCount - 2];
        NSMenuItem *lastItem = [defaultMenuItems objectAtIndex:defaultItemsCount - 1];

        if ([secondToLastItem isSeparatorItem] && [lastItem tag] == WebMenuItemTagInspectElement) {
            savedItems = [NSMutableArray arrayWithCapacity:2];
            [savedItems addObject:secondToLastItem];
            [savedItems addObject:lastItem];

            [defaultMenuItems removeObject:secondToLastItem];
            [defaultMenuItems removeObject:lastItem];
            defaultItemsCount -= 2;
        }
    }

    BOOL preVersion3Client = isPreVersion3Client();
    if (!preVersion3Client)
        return savedItems;
        
    BOOL isMail = isAppleMail();
    for (unsigned i = 0; i < defaultItemsCount; ++i) {
        NSMenuItem *item = [defaultMenuItems objectAtIndex:i];
        int tag = [item tag];
        int oldStyleTag = tag;
        
        if (preVersion3Client && isMail && tag == WebMenuItemTagOpenLink) {
            // Tiger Mail changes our "Open Link in New Window" item to "Open Link"
            // and doesn't expect us to include an "Open Link" item at all. (5011905)
            [defaultMenuItems removeObjectAtIndex:i];
            i--;
            defaultItemsCount--;
            continue;
        }
        
        if (tag >= WEBMENUITEMTAG_WEBKIT_3_0_SPI_START) {
            // Change all editing-related SPI tags listed in WebUIDelegatePrivate.h to WebMenuItemTagOther
            // to match our old WebKit context menu behavior.
            oldStyleTag = WebMenuItemTagOther;
        } else {
            // All items are expected to have useful tags coming into this method.
            ASSERT(tag != WebMenuItemTagOther);
            
            // Use the pre-3.0 tags for the few items that changed tags as they moved from SPI to API. We
            // do this only for old clients; new Mail already expects the new symbols in this case.
            if (preVersion3Client) {
                switch (tag) {
                    case WebMenuItemTagSearchInSpotlight:
                        oldStyleTag = OldWebMenuItemTagSearchInSpotlight;
                        break;
                    case WebMenuItemTagSearchWeb:
                        oldStyleTag = OldWebMenuItemTagSearchWeb;
                        break;
                    case WebMenuItemTagLookUpInDictionary:
                        oldStyleTag = OldWebMenuItemTagLookUpInDictionary;
                        break;
                    default:
                        break;
                }
            }
        }

        if (oldStyleTag != tag)
            [item setTag:oldStyleTag];
    }

    return savedItems;
}

static void fixMenusReceivedFromOldClients(NSMutableArray *newMenuItems, NSMutableArray *savedItems)
{   
    if (savedItems)
        [newMenuItems addObjectsFromArray:savedItems];

    BOOL preVersion3Client = isPreVersion3Client();
    if (!preVersion3Client)
        return;
    
    // Restore the modern tags to the menu items whose tags we altered in fixMenusToSendToOldClients. 
    unsigned newItemsCount = [newMenuItems count];
    for (unsigned i = 0; i < newItemsCount; ++i) {
        NSMenuItem *item = [newMenuItems objectAtIndex:i];
        
        int tag = [item tag];
        int modernTag = tag;
        
        if (tag == WebMenuItemTagOther) {
            // Restore the specific tag for items on which we temporarily set WebMenuItemTagOther to match old behavior.
            NSString *title = [item title];
            if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagOpenLink]])
                modernTag = WebMenuItemTagOpenLink;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagIgnoreGrammar]])
                modernTag = WebMenuItemTagIgnoreGrammar;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagSpellingMenu]])
                modernTag = WebMenuItemTagSpellingMenu;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagShowSpellingPanel:true]]
                     || [title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagShowSpellingPanel:false]])
                modernTag = WebMenuItemTagShowSpellingPanel;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagCheckSpelling]])
                modernTag = WebMenuItemTagCheckSpelling;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagCheckSpellingWhileTyping]])
                modernTag = WebMenuItemTagCheckSpellingWhileTyping;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagCheckGrammarWithSpelling]])
                modernTag = WebMenuItemTagCheckGrammarWithSpelling;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagFontMenu]])
                modernTag = WebMenuItemTagFontMenu;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagShowFonts]])
                modernTag = WebMenuItemTagShowFonts;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagBold]])
                modernTag = WebMenuItemTagBold;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagItalic]])
                modernTag = WebMenuItemTagItalic;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagUnderline]])
                modernTag = WebMenuItemTagUnderline;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagOutline]])
                modernTag = WebMenuItemTagOutline;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagStyles]])
                modernTag = WebMenuItemTagStyles;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagShowColors]])
                modernTag = WebMenuItemTagShowColors;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagSpeechMenu]])
                modernTag = WebMenuItemTagSpeechMenu;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagStartSpeaking]])
                modernTag = WebMenuItemTagStartSpeaking;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagStopSpeaking]])
                modernTag = WebMenuItemTagStopSpeaking;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagWritingDirectionMenu]])
                modernTag = WebMenuItemTagWritingDirectionMenu;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagDefaultDirection]])
                modernTag = WebMenuItemTagDefaultDirection;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagLeftToRight]])
                modernTag = WebMenuItemTagLeftToRight;
            else if ([title isEqualToString:[[WebViewFactory sharedFactory] contextMenuItemTagRightToLeft]])
                modernTag = WebMenuItemTagRightToLeft;
            else {
            // We don't expect WebMenuItemTagOther for any items other than the ones we explicitly handle.
            // There's nothing to prevent an app from applying this tag, but they are supposed to only
            // use tags in the range starting with WebMenuItemBaseApplicationTag=10000
                ASSERT_NOT_REACHED();
            }
        } else if (preVersion3Client) {
            // Restore the new API tag for items on which we temporarily set the old SPI tag. The old SPI tag was
            // needed to avoid confusing clients linked against earlier WebKits; the new API tag is needed for
            // WebCore to handle the menu items appropriately (without needing to know about the old SPI tags).
            switch (tag) {
                case OldWebMenuItemTagSearchInSpotlight:
                    modernTag = WebMenuItemTagSearchInSpotlight;
                    break;
                case OldWebMenuItemTagSearchWeb:
                    modernTag = WebMenuItemTagSearchWeb;
                    break;
                case OldWebMenuItemTagLookUpInDictionary:
                    modernTag = WebMenuItemTagLookUpInDictionary;
                    break;
                default:
                    break;
            }
        }
        
        if (modernTag != tag)
            [item setTag:modernTag];        
    }
}

NSMutableArray* WebContextMenuClient::getCustomMenuFromDefaultItems(ContextMenu* defaultMenu)
{
    id delegate = [m_webView UIDelegate];
    SEL selector = @selector(webView:contextMenuItemsForElement:defaultMenuItems:);
    if (![delegate respondsToSelector:selector])
        return defaultMenu->platformDescription();

    NSDictionary *element = [[[WebElementDictionary alloc] initWithHitTestResult:defaultMenu->hitTestResult()] autorelease];

    BOOL preVersion3Client = isPreVersion3Client();
    if (preVersion3Client) {
        DOMNode *node = [element objectForKey:WebElementDOMNodeKey];
        if ([node isKindOfClass:[DOMHTMLInputElement class]] && [(DOMHTMLInputElement *)node _isTextField])
            return defaultMenu->platformDescription();
        if ([node isKindOfClass:[DOMHTMLTextAreaElement class]])
            return defaultMenu->platformDescription();
    }

    NSMutableArray *defaultMenuItems = defaultMenu->platformDescription();

    unsigned defaultItemsCount = [defaultMenuItems count];
    for (unsigned i = 0; i < defaultItemsCount; ++i)
        [[defaultMenuItems objectAtIndex:i] setRepresentedObject:element];

    NSMutableArray *savedItems = [fixMenusToSendToOldClients(defaultMenuItems) retain];
    NSArray *delegateSuppliedItems = CallUIDelegate(m_webView, selector, element, defaultMenuItems);
    NSMutableArray *newMenuItems = [delegateSuppliedItems mutableCopy];
    fixMenusReceivedFromOldClients(newMenuItems, savedItems);
    [savedItems release];
    return [newMenuItems autorelease];
}

void WebContextMenuClient::contextMenuItemSelected(ContextMenuItem* item, const ContextMenu* parentMenu)
{
    id delegate = [m_webView UIDelegate];
    SEL selector = @selector(webView:contextMenuItemSelected:forElement:);
    if ([delegate respondsToSelector:selector]) {
        NSDictionary *element = [[WebElementDictionary alloc] initWithHitTestResult:parentMenu->hitTestResult()];
        NSMenuItem *platformItem = item->releasePlatformDescription();

        CallUIDelegate(m_webView, selector, platformItem, element);

        [element release];
        [platformItem release];
    }
}

void WebContextMenuClient::downloadURL(const KURL& url)
{
    [m_webView _downloadURL:url];
}

void WebContextMenuClient::searchWithSpotlight()
{
    [m_webView _searchWithSpotlightFromMenu:nil];
}

void WebContextMenuClient::searchWithGoogle(const Frame*)
{
    [m_webView _searchWithGoogleFromMenu:nil];
}

void WebContextMenuClient::lookUpInDictionary(Frame* frame)
{
    WebHTMLView* htmlView = (WebHTMLView*)[[kit(frame) frameView] documentView];
    if(![htmlView isKindOfClass:[WebHTMLView class]])
        return;
    [htmlView _lookUpInDictionaryFromMenu:nil];
}

void WebContextMenuClient::speak(const String& string)
{
    [NSApp speakString:[[(NSString*)string copy] autorelease]];
}

void WebContextMenuClient::stopSpeaking()
{
    [NSApp stopSpeaking];
}