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

#ifndef WebView_H
#define WebView_H


/**
 *  @file  WebView.h
 *  WebView description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "WebKitTypes.h"
#include <string>

class DefaultDownloadDelegate;
class DefaultPolicyDelegate;
class DOMDocument;
class WebArchive;
class WebBackForwardList;
class WebElementPropertyBag;
class WebFrame;
class WebHistoryItem;
class WebMutableURLRequest;
class WebNotificationDelegate;
class WebPreferences;
class WebScriptObject;
class WebViewPrivate;
class WebView;
class WebViewObserver;

namespace WebCore {
    class FrameView;
    class IntRect;
    class Image;
    class IntPoint;
    class KeyboardEvent;
    class Page;
    class ResourceRequest;
}
using namespace std;

/**
  * get a webview from page
  */
WebView* kit(WebCore::Page*);

/**
  * get page from a webview
  */
WebCore::Page* core(WebView*);


/**
  * cMinimumZoomMultiplier is the minimum zoom multiplier that can be set using the
  * different zoom methods (setPageSizeMultiplier, setTextSizeMultiplier, zoomIn, zoomOut...). Providing
  * a value below this threshold will be rounded up to this value.
  */
static const float cMinimumZoomMultiplier = 0.5f;

/**
  * cMaximumZoomMultiplier is the maximum zoom multiplier that can be set using the
  * different zoom methods (setPageSizeMultiplier, setTextSizeMultiplier, zoomIn, zoomOut...). Providing
  * a value above this threshold will be rounded down to this value.
  */
static const float cMaximumZoomMultiplier = 3.0f;

/**
  * When using zoomIn or zoomOut, this will be the current zoom multiplier / divisor.
  */
static const float cZoomMultiplierRatio = 1.2f;

#define WebViewProgressStartedNotification "WebProgressStartedNotification"
#define WebViewProgressEstimateChangedNotification "WebProgressEstimateChangedNotification"
#define WebViewProgressFinishedNotification "WebProgressFinishedNotification"


class MouseEventPrivate;

class WEBKIT_OWB_API WebView 
//    : public OWBAL::ObserverData
{
public:

    /**
     * create a new instance of WebView
     */
    static WebView* createInstance();

    /**
     * WebView destructor
     */
    virtual ~WebView();
protected:

    /**
     * WebView default constructor
     */
    WebView();

public:

    /**
     *  canShowMIMEType 
     * Checks if the WebKit can show content of a certain MIME type.
        @param MIMEType The MIME type to check.
        @result YES if the WebKit can show content with MIMEtype.
     */
    virtual bool canShowMIMEType(const char* mimeType);

    /**
     *  canShowMIMETypeAsHTML 
     * Checks if the the MIME type is a type that the WebKit will interpret as HTML.
        @param MIMEType The MIME type to check.
        @result true if the MIMEtype in an HTML type.
     */
    virtual bool canShowMIMETypeAsHTML(const char * mimeType);

    /**
     *  setMIMETypesShownAsHTML 
     * @discussion Sets the array of NSString MIME types that WebKit will
        attempt to render as HTML.  Typically you will retrieve the built-in
        array using MIMETypesShownAsHTML and add additional MIME types to that
        array.
     */
    virtual void setMIMETypesShownAsHTML(const char* mimeTypes, int cMimeTypes);

    /**
     *  initWithFrame 
     * The designated initializer for WebView.
        @discussion Initialize a WebView with the supplied parameters. This method will
        create a main WebFrame with the view. Passing a top level frame name is useful if you
        handle a targetted frame navigation that would normally open a window in some other
        way that still ends up creating a new WebView.
        @param frame The frame used to create the view.
        @param frameName The name to use for the top level frame. May be nil.
        @param groupName The name of the webView set to which this webView will be added.  May be nil.
        @result Returns an initialized WebView.
     */
    virtual void initWithFrame(BalRectangle& frame, const char* frameName, const char* groupName);

    /**
     *  setDownloadDelegate 
     * Set the WebView's WebDownloadDelegate.
        @discussion The download delegate is retained by WebDownload when any downloads are in progress.
        @param delegate The WebDownloadDelegate to set as the download delegate.
     */
    virtual void setDownloadDelegate(DefaultDownloadDelegate *d);

    /**
     *  downloadDelegate 
     *  Return the WebView's WebDownloadDelegate.
     *  @result The WebView's WebDownloadDelegate.
     */
    virtual DefaultDownloadDelegate* downloadDelegate();

    /**
     *  setPolicyDelegate 
     *  Set the WebView's WebPolicyDelegate delegate.
     *  @param delegate The WebPolicyDelegate to set as the delegate.
     */
    virtual void setPolicyDelegate(DefaultPolicyDelegate* d);

    /**
     *  policyDelegate 
     *  Return the WebView's WebPolicyDelegate.
     *  @result The WebView's WebPolicyDelegate.
     */
    virtual DefaultPolicyDelegate* policyDelegate();

    /**
     *  setWebNotificationDelegate
     *  Set the WebView's WebNotificationDelegate.
     */
    virtual void setWebNotificationDelegate(WebNotificationDelegate*);

    /**
     *  webNotificationDelegate
     *  get the WebView's WebNotificationDelegate.
     */
    WebNotificationDelegate* webNotificationDelegate();

    /**
     * mainFrame 
     * Return the frame that has the current focus
     */
    virtual WebFrame *mainFrame();

    /**
     * focusedFrame 
     * Return the frame that has the current focus.
     */
    virtual WebFrame *focusedFrame();

    /**
     * backForwardList 
     * The backforward list for this webView.
     */
    virtual WebBackForwardList *backForwardList();

    /**
     *  setMaintainsBackForwardList 
     * Enable or disable the use of a backforward list for this webView.
        @param flag Turns use of the back forward list on or off
     */
    virtual void setMaintainsBackForwardList(bool flag);

    /**
     *  goBack 
     * Go back to the previous URL in the backforward list.
        @result true if able to go back in the backforward list, false otherwise.
     */
    virtual bool goBack();

    /**
     * goForward 
     * Go forward to the next URL in the backforward list.
        @result true if able to go forward in the backforward list, false otherwise
     */
    virtual bool goForward();

    /**
     *  goToBackForwardItem 
     * Go back or forward to an item in the backforward list.
        @result true if able to go to the item, false otherwise.
     */
    virtual bool goToBackForwardItem(WebHistoryItem *item);

    /**
     *  setTextSizeMultiplier 
     *  Change the size of the text rendering in views managed by this webView.
     *  @param multiplier A fractional percentage value, 1.0 is 100%.
     *  @result Returns true if the zoom was set as-is, false if was rounded. 
     */
    virtual bool setTextSizeMultiplier(float multiplier);

    /**
     *  textSizeMultiplier 
     * @result The text size multipler.
     */
    virtual float textSizeMultiplier();

    /**
     *  setApplicationNameForUserAgent 
     * Set the application name.
        @discussion This name will be used in user-agent strings
        that are chosen for best results in rendering web pages.
        @param applicationName The application name
     */
    virtual void setApplicationNameForUserAgent(const char* applicationName);

    /**
     *  applicationNameForUserAgent 
     * The name of the application as used in the user-agent string.
     */
    virtual const char* applicationNameForUserAgent();

    /**
     *  setCustomUserAgent 
     * Set the user agent.
        @discussion Setting this means that the webView should use this user-agent string
        instead of constructing a user-agent string for each URL. Setting it to nil
        causes the webView to construct the user-agent string for each URL
        for best results rendering web pages.
        @param userAgentString The user agent description
     */
    virtual void setCustomUserAgent(const char* userAgentString);

    /**
     *  customUserAgent 
     * @result The custom user-agent string or nil if no custom user-agent string has been set.
     */
    virtual const char* customUserAgent();

    /**
     *  userAgentForURL 
     * Get the appropriate user-agent string for a particular URL.
        @param URL The URL.
        @result The user-agent string for the supplied URL.
     */
    virtual const char* userAgentForURL(const char* url);

    /**
     *  supportsTextEncoding 
     * Find out if the current web page supports text encodings.
        @result true if the document view of the current web page can
        support different text encodings.
     */
    virtual bool supportsTextEncoding();

    /**
     *  setCustomTextEncodingName 
     * @discussion Make the page display with a different text encoding; stops any load in progress.
        The text encoding passed in overrides the normal text encoding smarts including
        what's specified in a web page's header or HTTP response.
        The text encoding automatically goes back to the default when the top level frame
        changes to a new location.
        Setting the text encoding name to nil makes the webView use default encoding rules.
        @param encoding The text encoding name to use to display a page or nil.
     */
    virtual void setCustomTextEncodingName(const char* encodingName);

    /**
     *  customTextEncodingName 
     * @result The custom text encoding name or nil if no custom text encoding name has been set.
     */
    virtual const char* customTextEncodingName();

    /**
     *  setMediaStyle 
     * @discussion Set the media style for the WebView.  The mediaStyle will override the normal value
        of the CSS media property.  Setting the value to nil will restore the normal value.
        @param mediaStyle The value to use for the CSS media property.
     */
    virtual void setMediaStyle(const char* media);

    /**
     *  mediaStyle 
     * @result mediaStyle The value to use for the CSS media property, as set by setMediaStyle:.  It
        will be nil unless set by that method.
     */
    virtual const char* mediaStyle();

    /**
     *  stringByEvaluatingJavaScriptFromString 
     * @param script The text of the JavaScript.
        @result The result of the script, converted to a string, or nil for failure.
     */
    virtual const char* stringByEvaluatingJavaScriptFromString(const char* script);

    /**
     * windowScriptObject 
     * @discussion windowScriptObject return a WebScriptObject that represents the
        window object from the script environment.
        @result Returns the window object from the script environment.
     */
    virtual WebScriptObject *windowScriptObject();

    /**
     *  setPreferences 
     * @param preferences The preferences to use for the webView.
        @abstract Override the standard setting for the webView.
     */
    virtual void setPreferences(WebPreferences *prefs);

    /**
     * preferences 
     * @result Returns the preferences used by this webView.
        @discussion This method will return [WebPreferences standardPreferences] if no
        other instance of WebPreferences has been set.
     */
    virtual WebPreferences *preferences();

    /**
     *  setPreferencesIdentifier 
     * @param anIdentifier The string to use a prefix for storing values for this WebView in the user
        defaults database.
        @discussion If the WebPreferences for this WebView are stored in the user defaults database, the
        string set in this method will be used a key prefix.
     */
    virtual void setPreferencesIdentifier(const char* anIdentifier);

    /**
     *  preferencesIdentifier 
     * @result Returns the WebPreferences key prefix.
     */
    virtual const char* preferencesIdentifier();

    /**
     *  searchFor 
     * @abstract Searches a document view for a string and highlights the string if it is found.
        Starts the search from the current selection.  Will search across all frames.
        @param string The string to search for.
        @param forward true to search forward, false to seach backwards.
        @param caseFlag true to for case-sensitive search, false for case-insensitive search.
        @result true if found, false if not found.
     */
    virtual bool searchFor(const char* str, bool forward, bool caseFlag, bool wrapFlag);

    /**
     *  setGroupName 
     * @param groupName The name of the group for this WebView.
       @discussion JavaScript may access named frames within the same group.
     */
    virtual void setGroupName(const char*);

    /**
     *  groupName 
     * @discussion The group name for this WebView.
                   *** The string returned by this method is duplicated with strdup so it is up to the caller to free it ***
     */
    virtual const char* groupName();

    /**
     *  estimatedProgress 
     * @discussion An estimate of the percent complete for a document load.  This
        value will range from 0 to 1.0 and, once a load completes, will remain at 1.0
        until a new load starts, at which point it will be reset to 0.  The value is an
        estimate based on the total number of bytes expected to be received
        for a document, including all it's possible subresources.  For more accurate progress
        indication it is recommended that you implement a WebFrameLoadDelegate and a
        WebResourceLoadDelegate.
     */
    virtual double estimatedProgress();

    /**
     *  isLoading 
     * @discussion Returns true if there are any pending loads.
     */
    virtual bool isLoading();

    /**
     * elementAtPoint 
     * @param point A point in the coordinates of the WebView
        @result An element dictionary describing the point
     */
    virtual WebElementPropertyBag* elementAtPoint(BalPoint& point);

    /**
     *  selectedText 
     * abstract Returns the selection as a string
     */
    virtual const char* selectedText();

    /**
     *  centerSelectionInVisibleArea 
     * @abstract Centers the selected text in the WebView
     */
    virtual void centerSelectionInVisibleArea();

    /**
     *  moveDragCaretToPoint 
     * @param point A point in the coordinates of the WebView
        @discussion This method moves the caret that shows where something being dragged will be dropped. It may cause the WebView to scroll
        to make the new position of the drag caret visible.
     */
    virtual void moveDragCaretToPoint(BalPoint& point);

    /**
     *  removeDragCaret 
     * @param drawsBackround true to cause the receiver to draw a default white background, false otherwise.
        @abstract Sets whether the receiver draws a default white background when the loaded page has no background specified.
     */
    virtual void removeDragCaret();

    /**
     *  setDrawsBackground 
     * @param drawsBackround YES to cause the receiver to draw a default white background, NO otherwise.
        @abstract Sets whether the receiver draws a default white background when the loaded page has no background specified.
     */
    virtual void setDrawsBackground(bool drawsBackground);

    /**
     *  drawsBackground 
     * @result Returns true if the receiver draws a default white background, false otherwise.
     */
    virtual bool drawsBackground();

    /**
     *  setMainFrameURL 
     * @param URLString The URL to load in the mainFrame.
     */
    virtual void setMainFrameURL(const char* urlString);

    /**
     *  mainFrameURL 
     * @result Returns the main frame's current URL
     */
    virtual const char* mainFrameURL();

    /**
     *  mainFrameDocument 
     *  @result Returns the main frame's DOMDocument.
     */
    virtual DOMDocument* mainFrameDocument();

    /**
     *  mainFrameTitle 
     * @result Returns the main frame's title if any, otherwise an empty string.
     */
    virtual const char* mainFrameTitle();

    /**
     * registerURLSchemeAsLocal 
     * @discussion Adds the scheme to the list of schemes to be treated as local.
        @param scheme The scheme to register.
     */
    virtual void registerURLSchemeAsLocal(const char* scheme);


    /**
     * setSmartInsertDeleteEnabled 
     */
    virtual void setSmartInsertDeleteEnabled(bool flag);

    /**
     *  smartInsertDeleteEnabled 
     */
    virtual bool smartInsertDeleteEnabled();

    /*
     * setSelectTrailingWhitespaceEnabled
     */
    virtual void setSelectTrailingWhitespaceEnabled(bool flag);

    /*
     * isSelectTrailingWhitespaceEnabled
     */
    virtual bool isSelectTrailingWhitespaceEnabled();

    /**
     *  setContinuousSpellCheckingEnabled 
     */
    virtual void setContinuousSpellCheckingEnabled(bool flag);

    /**
     *  isContinuousSpellCheckingEnabled 
     */
    virtual bool isContinuousSpellCheckingEnabled();

    /**
     *  spellCheckerDocumentTag 
     */
    virtual int spellCheckerDocumentTag();

    /**
     * undoManager 
     */
    //virtual WebUndoManager *undoManager();
    

    /**
     * setEditingDelegate 
     */
    //virtual void setEditingDelegate(WebEditingDelegate *d);

    /**
     * editingDelegate 
     */
    //virtual WebEditingDelegate * editingDelegate();


    /**
     *  hasSelectedRange 
     */
    virtual bool hasSelectedRange();

    /**
     *  cutEnabled 
     */
    virtual bool cutEnabled();

    /**
     *  copyEnabled 
     */
    virtual bool copyEnabled();

    /**
     *  pasteEnabled 
     */
    virtual bool pasteEnabled();

    /**
     *  deleteEnabled 
     */
    virtual bool deleteEnabled();

    /**
     *  editingEnabled 
     */
    virtual bool editingEnabled();

    /**
     *  isGrammarCheckingEnabled 
     */
    virtual bool isGrammarCheckingEnabled();

    /**
     *  setGrammarCheckingEnabled 
     */
    virtual void setGrammarCheckingEnabled(bool enabled);

    /**
     *  setPageSizeMultiplier 
     *  @abstract Set a zoom factor for all views managed by this webView.
     *  @param multiplier A fractional percentage value, 1.0 is 100%. The multiplier should be in the
     *                    range [ cMinimumZoomMultipler; cMaximumZoomMultiplier ] otherwise setting the
     *                    zoom will be rounded to stay in the interval.
     *  @result Returns true if the zoom was set as-is, false if was rounded. 
     */
    virtual bool setPageSizeMultiplier(float multiplier);

    /**
     *  pageSizeMultiplier 
     *  @result The page size multipler.
     */
    virtual float pageSizeMultiplier();

    /**
     *  zoomPageIn
     *  @result Returns true if the zoom was set as-is, false if was rounded.
     */
    virtual bool zoomPageIn();

    /**
     *  zoomPageOut
     *  @result Returns true if the zoom was set as-is, false if was rounded. 
     */
    virtual bool zoomPageOut();

    /**
     *  canResetPageZoom 
     */
    virtual bool canResetPageZoom();

    /**
     *  resetPageZoom 
     */
    virtual void resetPageZoom();


    /**
     *  makeTextLarger
     *  @result Returns true if the zoom was set as-is, false if was rounded.
     */
    virtual bool makeTextLarger();

    /**
     *  makeTextSmaller
     *  @result Returns true if the zoom was set as-is, false if was rounded.
     */
    virtual bool makeTextSmaller();

    /**
     *  canMakeTextStandardSize 
     */
    virtual bool canMakeTextStandardSize();

    /**
     *  makeTextStandardSize 
     */
    virtual void makeTextStandardSize();


    /**
     *  replaceSelectionWithText 
     */
    virtual void replaceSelectionWithText(const char* text);

    /**
     *  replaceSelectionWithMarkupString 
     */
    virtual void replaceSelectionWithMarkupString(const char* markupString);

    /**
     *  replaceSelectionWithArchive 
     */
    virtual void replaceSelectionWithArchive(WebArchive *archive);

    /**
     *  deleteSelection 
     */
    virtual void deleteSelection();

    /**
     *  clearSelection 
     */
    virtual void clearSelection();


    /**
     *  clearMainFrameName
     */
    virtual void clearMainFrameName();

    /**
     *  copy 
     */
    virtual void copy();

    /**
     *  cut 
     */
    virtual void cut();

    /**
     *  paste 
     */
    virtual void paste();

    /**
     *  copyURL 
     */
    virtual void copyURL(const char* url);

    /**
     *  copyFont 
     */
    virtual void copyFont();

    /**
     *  pasteFont 
     */
    virtual void pasteFont();

    /**
     *  delete_ 
     */
    virtual void delete_();

    /**
     *  pasteAsPlainText 
     */
    virtual void pasteAsPlainText();

    /**
     *  pasteAsRichText 
     */
    virtual void pasteAsRichText();

    /**
     *  changeFont 
     */
    virtual void changeFont();

    /**
     *  changeAttributes 
     */
    virtual void changeAttributes();

    /**
     *  changeDocumentBackgroundColor 
     */
    virtual void changeDocumentBackgroundColor();

    /**
     *  changeColor 
     */
    virtual void changeColor();

    /**
     *  alignCenter 
     */
    virtual void alignCenter();

    /**
     *  alignJustified 
     */
    virtual void alignJustified();

    /**
     *  alignLeft 
     */
    virtual void alignLeft();

    /**
     *  alignRight 
     */
    virtual void alignRight();

    /**
     *  checkSpelling 
     */
    virtual void checkSpelling();

    /**
     *  showGuessPanel 
     */
    virtual void showGuessPanel();

    /**
     *  performFindPanelAction 
     */
    virtual void performFindPanelAction();

    /**
     *  startSpeaking 
     */
    virtual void startSpeaking();

    /**
     *  stopSpeaking 
     */
    virtual void stopSpeaking();


    /**
     *  setCustomDropTarget 
     */
    //virtual void setCustomDropTarget(DropTarget* dt);

    /**
     *  removeCustomDropTarget 
     */
    //virtual void removeCustomDropTarget();

    /**
     *  setInViewSourceMode 
     */
    virtual void setInViewSourceMode(bool flag);

    /**
     *  inViewSourceMode 
     */
    virtual bool inViewSourceMode();

    /**
     *  get view window 
     */
    virtual BalWidget* viewWindow();

    /**
     * set view window 
     */
    virtual void setViewWindow(BalWidget*);

    /**
     *  scrollOffset 
     */
    virtual BalPoint scrollOffset();

    /**
     *  scrollBy 
     */
    virtual void scrollBy(BalPoint offset);

    /**
     *  visibleContentRect 
     */
    virtual BalRectangle visibleContentRect();

    /**
     *  updateFocusedAndActiveState 
     */
    virtual void updateFocusedAndActiveState();

    /**
     *  executeCoreCommandByName 
     */
    virtual void executeCoreCommandByName(const char* name, const char* value);

    /**
     *  markAllMatchesForText 
     */
    virtual unsigned int markAllMatchesForText(const char* search, bool caseSensitive, bool highlight, unsigned int limit);

    /**
     *  unmarkAllTextMatches 
     */
    virtual void unmarkAllTextMatches();

    /**
     *  selectionRect 
     */
    virtual BalRectangle selectionRect();
    /*virtual  DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);*
    virtual  DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
    virtual  DragLeave();
    virtual  Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);*/


    /**
     *  canHandleRequest 
     */
    virtual bool canHandleRequest(WebMutableURLRequest *request);

    /**
     *  clearFocusNode 
     */
    virtual void clearFocusNode();

    /**
     *  setInitialFocus 
     */
    virtual void setInitialFocus(bool forward);

    /**
     *  setTabKeyCyclesThroughElements 
     */
    virtual void setTabKeyCyclesThroughElements(bool cycles);

    /**
     *  tabKeyCyclesThroughElements 
     */
    virtual bool tabKeyCyclesThroughElements();

    /**
     *  setAllowSiteSpecificHacks 
     */
    virtual void setAllowSiteSpecificHacks(bool allows);

    /**
     *  addAdditionalPluginDirectory 
     */
    virtual void addAdditionalPluginDirectory(const char* directory);

    /**
     *  loadBackForwardListFromOtherView 
     */
    virtual void loadBackForwardListFromOtherView(WebView *otherView);

    /**
     *  clearUndoRedoOperations 
     */
    virtual void clearUndoRedoOperations();

    /**
     *  shouldClose 
     * @abstract This function will fire the before unload handler for a page.
        If the user cancels the closing of a webview from the alert popped up by the
        before unload handler, then this function will return false.
     */
    virtual bool shouldClose();

    /**
     *  setProhibitsMainFrameScrolling 
     */
    virtual void setProhibitsMainFrameScrolling(bool);

    /**
     *  setShouldApplyMacFontAscentHack 
     */
    virtual void setShouldApplyMacFontAscentHack(bool);

    /**
     *  windowAncestryDidChange 
     */
    virtual void windowAncestryDidChange();

    /**
     * setCustomHTMLTokenizerTimeDelay
     */
    virtual void setCustomHTMLTokenizerTimeDelay(double timeDelay);

    /**
     * setCustomHTMLTokenizerChunkSize
     */
    virtual void setCustomHTMLTokenizerChunkSize(int chunkSize);

    /**
     * setTransparent
     */
    virtual void setTransparent(bool transparent);

    /**
     * transparent
     */
    virtual void transparent(bool* transparent);

    /**
     * set defersCallbacks
     */
    virtual void setDefersCallbacks(bool defersCallbacks);

    /**
     * get defersCallbacks
     */
    virtual bool defersCallbacks();

    /**
     * set whether cookies are enabled.
     */
    void setCookieEnabled(bool enable);

    /**
     * get whether cookies are enabled.
     */
    bool cookieEnabled();

    /**
     * set media volume
     */
    void setMediaVolume(float volume);

    /**
     * get media volume
     */
    float mediaVolume();

    /**
     * enable memory cache delegate calls
     */
    void setMemoryCacheDelegateCallsEnabled(bool enabled);
    

    /**
     *  give on expose event to the webview
     */
    void onExpose(BalEventExpose);

    /**
     *  give on keyDown  event to the webview
     */
    void onKeyDown(BalEventKey);

    /**
     *  give on keyup  event to the webview
     */
    void onKeyUp(BalEventKey);

    /**
     *  give on mouse motion  event to the webview
     */
    void onMouseMotion(BalEventMotion);

    /**
     *  give on mouse button down  event to the webview
     */
    void onMouseButtonDown(BalEventButton);

    /**
     *  give on mouse button up event to the webview
     */
    void onMouseButtonUp(BalEventButton);

    /**
     *  give on scroll event to the webview
     */
    void onScroll(BalEventScroll);

    /**
     *  give on resize event to the webview
     */
    void onResize(BalResizeEvent);

    /**
     *  give on quit  event to the webview
     */
    void onQuit(BalQuitEvent);

    /**
     *  give on user event to the webview
     */
    void onUserEvent(BalUserEvent);


    /**
     * paint 
     */
    void paint();

    /**
     *  ensureBackingStore
     */
    bool ensureBackingStore();

    /**
     *  addToDirtyRegion 
     */
    void addToDirtyRegion(const BalRectangle&);

    /**
     *  dirtyRegion 
     */
    BalRectangle dirtyRegion();

    /**
     *  clearDirtyRegion 
     */
    void clearDirtyRegion();


    /**
     *  get frame rect 
     */
    BalRectangle frameRect();

    /**
     *  closeWindow 
     */
    void closeWindow();

    /**
     *  closeWindowSoon 
     */
    void closeWindowSoon();

    /**
     *  close the webview
     */
    void close();

    /**
     * didClose 
     */
    bool didClose() const { return m_didClose; }

    /**
     * transparent
     */
    bool transparent() const { return m_transparent; }

    /**
     *  selectionChanged 
     */
    void selectionChanged();

    /**
     *  registerDragDrop 
     */
    //HRESULT registerDragDrop();

    /**
     *  revokeDragDrop 
     */
    //HRESULT revokeDragDrop();

    /**
     * get the top level frame 
     */
    // Convenient to be able to violate the rules of COM here for easy movement to the frame.
    WebFrame* topLevelFrame() const { return m_mainFrame; }

    /**
     * get user agent for url
     */
    const char* userAgentForKURL(const char* url);



    /**
     * setIsBeingDestroyed 
     */
    void setIsBeingDestroyed() { m_isBeingDestroyed = true; }

    /**
     * isBeingDestroyed 
     */
    bool isBeingDestroyed() const { return m_isBeingDestroyed; }



    /**
     * isPainting 
     */
    bool isPainting() const { return m_paintCount > 0; }


    /**
     *  setToolTip 
     */
    void setToolTip(const char*);

//#if ENABLE(ICONDATABASE)

    /**
     *  registerForIconNotification 
     */
  //  void registerForIconNotification(bool listen);

    /**
     *  dispatchDidReceiveIconFromWebFrame 
     */
    //void dispatchDidReceiveIconFromWebFrame(WebFrame*);


    /**
     *  notifyDidAddIcon 
     */
    //void notifyDidAddIcon();
//#endif

    /**
     *  notifyPreferencesChanged 
     */
    void notifyPreferencesChanged(WebPreferences*);


    /**
     *  set CacheModel 
     */
    static void setCacheModel(WebCacheModel);

    /**
     * get CacheModel 
     */
    static WebCacheModel cacheModel();

    /**
     * didSetCacheModel 
     */
    static bool didSetCacheModel();

    /**
     *  maxCacheModelInAnyInstance 
     */
    static WebCacheModel maxCacheModelInAnyInstance();

    /**
     * updateActiveStateSoon 
     */
    void updateActiveStateSoon() const;

    /**
     *  deleteBackingStoreSoon 
     */
    void deleteBackingStoreSoon();

    /**
     *  cancelDeleteBackingStoreSoon 
     */
    void cancelDeleteBackingStoreSoon();

    /**
     * get top level parent 
     */
    BalWidget* topLevelParent() const { return m_topLevelParent; }


    /**
     * update active state 
     */
    void updateActiveState();

    /**
     * parse owb config file 
     */
    void parseConfigFile(const char* url = "");

    /**
     *  AccessibleObjectFromWindow 
     */
    //static STDMETHODIMP AccessibleObjectFromWindow(HWND, DWORD objectID, REFIID, void** ppObject);

    // Implemented by the platforms.
    void runJavaScriptAlert(WebFrame*, const char*);

    void popupMenuHide();
    void popupMenuShow(void* userData);

    void fireWebKitEvents();

private:

    /**
     *  setZoomMultiplier
     *  @result Returns true if the zoom was set as-is, false if was rounded.
     */
    bool setZoomMultiplier(float multiplier, bool isTextOnly);

    /**
     *  zoomMultiplier 
     */
    float zoomMultiplier(bool isTextOnly);

    /**
     *  zoomIn
     *  @result Returns true if the zoom was set as-is, false if was rounded.
     */
    bool zoomIn(bool isTextOnly);

    /**
     *  zoomOut
     *  @result Returns true if the zoom was set as-is, false if was rounded.
     */
    bool zoomOut(bool isTextOnly);

    /**
     *  canResetZoom 
     */
    bool canResetZoom(bool isTextOnly);

    /**
     *  resetZoom 
     */
    void resetZoom(bool isTextOnly);

    /**
     *  active 
     */
    bool active();

protected:
    friend class WebViewPrivate;
    friend class WebChromeClient;
    friend class WebEditorClient;
    friend class WebFrameLoaderClient;
    friend class WebInspector;
    friend WebCore::Page* core(WebView*);

    /**
     *  scrollBackingStore 
     */
    void scrollBackingStore(WebCore::FrameView*, int dx, int dy, const BalRectangle& scrollViewRect, const BalRectangle& clipRect);

    /**
     *  updateBackingStore 
     */
    void updateBackingStore(WebCore::FrameView*, bool backingStoreCompletelyDirty = false);

    /**
     *  deleteBackingStore 
     */
    void deleteBackingStore();

    /**
     * repaint
     */
    void repaint(const WebCore::IntRect&, bool contentChanged, bool immediate = false, bool repaintContentOnly = false);

    /**
     * interpret KeyEvent 
     */
    const char* interpretKeyEvent(const WebCore::KeyboardEvent*);

    /**
     * handleEditingKeyboardEvent 
     */
    bool handleEditingKeyboardEvent(WebCore::KeyboardEvent*);

    /**
     * get page 
     */
#if PLATFORM(AMIGAOS4)
public:
    WebCore::Page* page();
protected:
#else
    WebCore::Page* page();
#endif

    /**
     *  canHandleRequest
     */
    static bool canHandleRequest(const WebCore::ResourceRequest&);

    /**
     *  getIMMContext 
     */
    //HIMC getIMMContext();

    /**
     *  releaseIMMContext 
     */
    //void releaseIMMContext(HIMC);

    /**
     * allowSiteSpecificHacks 
     */
    static bool allowSiteSpecificHacks() { return s_allowSiteSpecificHacks; } 

    /**
     *  continuousCheckingAllowed 
     */
    bool continuousCheckingAllowed();

    /**
     *  initializeToolTipWindow 
     */
    void initializeToolTipWindow();

    /**
     *  closeWindowTimerFired 
     */
//    void closeWindowTimerFired(WebCore::Timer<WebView>*);

    /**
     *  prepareCandidateWindow 
     */
    //void prepareCandidateWindow(WebCore::Frame*, HIMC);

    /**
     *  updateSelectionForIME 
     */
//     void updateSelectionForIME();

    /**
     *  onIMERequestCharPosition 
     */
    //bool onIMERequestCharPosition(WebCore::Frame*, IMECHARPOSITION*, LRESULT*);

    /**
     *  onIMERequestReconvertString 
     */
    //bool onIMERequestReconvertString(WebCore::Frame*, RECONVERTSTRING*, LRESULT*);

    /**
     * developerExtrasEnabled 
     */
    bool developerExtrasEnabled() const;

    // AllWebViewSet functions

    /**
     *  addToAllWebViewsSet 
     */
    void addToAllWebViewsSet();

    /**
     *  removeFromAllWebViewsSet 
     */
    void removeFromAllWebViewsSet();

    /**
     * Internal method to get the User-Agent string from the application name.
     */
    static const char* standardUserAgentWithApplicationName(const char*);

    /**
     *  windowReceivedMessage 
     */
    //virtual void windowReceivedMessage(HWND, UINT message, WPARAM, LPARAM);

    BalWidget *m_viewWindow;
    WebFrame* m_mainFrame;
    WebCore::Page* m_page;
    
//    OwnPtr<WebCore::Image> m_backingStoreBitmap;
//    BalPoint m_backingStoreSize;
//    BalRectangle m_backingStoreDirtyRegion;

    DefaultPolicyDelegate* m_policyDelegate;
    DefaultDownloadDelegate* m_downloadDelegate;
    WebNotificationDelegate* m_webNotificationDelegate;
    WebPreferences* m_preferences;

    bool m_userAgentOverridden;
    bool m_useBackForwardList;
    string m_userAgentCustom;
    string m_userAgentStandard;
    float m_zoomMultiplier;
    string m_overrideEncoding;
    string m_applicationName;
    bool m_mouseActivated;
    // WebCore dragging logic needs to be able to inspect the drag data
    // this is updated in DragEnter/Leave/Drop
    //COMPtr<IDataObject> m_dragData;
    //COMPtr<IDropTargetHelper> m_dropTargetHelper;
    //UChar m_currentCharacterCode;
    bool m_isBeingDestroyed;
    unsigned m_paintCount;
    bool m_hasSpellCheckerDocumentTag;
    bool m_smartInsertDeleteEnabled;
    bool m_selectTrailingWhitespaceEnabled;
    bool m_didClose;
    bool m_hasCustomDropTarget;
    unsigned m_inIMEComposition;
    BalWidget* m_toolTipHwnd;
    string m_toolTip;
    bool m_deleteBackingStoreTimerActive;

    bool m_transparent;
    bool m_isInitialized;

    static bool s_allowSiteSpecificHacks;

//    WebCore::Timer<WebView> m_closeWindowTimer;
    //OwnPtr<TRACKMOUSEEVENT> m_mouseOutTracker;

    BalWidget* m_topLevelParent;
    WebViewPrivate* d;
    WebViewObserver* m_webViewObserver;
};

#endif
