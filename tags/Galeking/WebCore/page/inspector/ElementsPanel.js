/*
 * Copyright (C) 2007, 2008 Apple Inc.  All rights reserved.
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

WebInspector.ElementsPanel = function()
{
    WebInspector.Panel.call(this);

    this.element.addStyleClass("elements");

    this.contentElement = document.createElement("div");
    this.contentElement.id = "elements-content";
    this.contentElement.className = "outline-disclosure";

    this.treeOutline = new WebInspector.ElementsTreeOutline();
    this.treeOutline.panel = this;
    this.treeOutline.includeRootDOMNode = false;
    this.treeOutline.selectEnabled = true;

    this.treeOutline.focusedNodeChanged = function(forceUpdate)
    {
        this.panel.updateBreadcrumb(forceUpdate);

        for (var pane in this.panel.sidebarPanes)
           this.panel.sidebarPanes[pane].needsUpdate = true;

        this.panel.updateStyles(true);
        this.panel.updateMetrics();
        this.panel.updateProperties();
    };

    this.contentElement.appendChild(this.treeOutline.element);

    this.crumbsElement = document.createElement("div");
    this.crumbsElement.className = "crumbs";

    this.sidebarPanes = {};
    this.sidebarPanes.styles = new WebInspector.StylesSidebarPane();
    this.sidebarPanes.metrics = new WebInspector.MetricsSidebarPane();
    this.sidebarPanes.properties = new WebInspector.PropertiesSidebarPane();

    this.sidebarPanes.styles.onexpand = this.updateStyles.bind(this);
    this.sidebarPanes.metrics.onexpand = this.updateMetrics.bind(this);
    this.sidebarPanes.properties.onexpand = this.updateProperties.bind(this);

    this.sidebarPanes.styles.expanded = true;

    this.sidebarPanes.styles.addEventListener("style edited", this._stylesPaneEdited, this);
    this.sidebarPanes.styles.addEventListener("style property toggled", this._stylesPaneEdited, this);
    this.sidebarPanes.metrics.addEventListener("metrics edited", this._metricsPaneEdited, this);

    this.sidebarElement = document.createElement("div");
    this.sidebarElement.id = "elements-sidebar";

    this.sidebarElement.appendChild(this.sidebarPanes.styles.element);
    this.sidebarElement.appendChild(this.sidebarPanes.metrics.element);
    this.sidebarElement.appendChild(this.sidebarPanes.properties.element);

    this.sidebarResizeElement = document.createElement("div");
    this.sidebarResizeElement.className = "sidebar-resizer-vertical";
    this.sidebarResizeElement.addEventListener("mousedown", this.rightSidebarResizerDragStart.bind(this), false);

    this.element.appendChild(this.contentElement);
    this.element.appendChild(this.sidebarElement);
    this.element.appendChild(this.sidebarResizeElement);

    this._mutationMonitoredWindows = [];
    this._nodeInsertedEventListener = InspectorController.wrapCallback(this._nodeInserted.bind(this));
    this._nodeRemovedEventListener = InspectorController.wrapCallback(this._nodeRemoved.bind(this));
    this._contentLoadedEventListener = InspectorController.wrapCallback(this._contentLoaded.bind(this));

    this.reset();
}

WebInspector.ElementsPanel.prototype = {
    toolbarItemClass: "elements",

    get toolbarItemLabel()
    {
        return WebInspector.UIString("Elements");
    },

    get statusBarItems()
    {
        return [this.crumbsElement];
    },

    updateStatusBarItems: function()
    {
        this.updateBreadcrumbSizes();
    },

    show: function()
    {
        WebInspector.Panel.prototype.show.call(this);
        this.sidebarResizeElement.style.right = (this.sidebarElement.offsetWidth - 3) + "px";
        this.updateBreadcrumb();
        this.treeOutline.updateSelection();
        if (this.recentlyModifiedNodes.length)
            this._updateModifiedNodes();
    },

    hide: function()
    {
        WebInspector.Panel.prototype.hide.call(this);
        WebInspector.hoveredDOMNode = null;
        WebInspector.forceHoverHighlight = false;
    },

    resize: function()
    {
        this.treeOutline.updateSelection();
        this.updateBreadcrumbSizes();
    },

    reset: function()
    {
        this.rootDOMNode = null;
        this.focusedDOMNode = null;

        WebInspector.hoveredDOMNode = null;
        WebInspector.forceHoverHighlight = false;

        this.recentlyModifiedNodes = [];
        this.unregisterAllMutationEventListeners();

        var inspectedWindow = InspectorController.inspectedWindow();
        if (!inspectedWindow || !inspectedWindow.document)
            return;

        if (!inspectedWindow.document.firstChild) {
            function contentLoaded()
            {
                inspectedWindow.document.removeEventListener("DOMContentLoaded", contentLoadedCallback, false);

                this.reset();
            }

            var contentLoadedCallback = InspectorController.wrapCallback(contentLoaded.bind(this));
            inspectedWindow.document.addEventListener("DOMContentLoaded", contentLoadedCallback, false);
            return;
        }

        // If the window isn't visible, return early so the DOM tree isn't built
        // and mutation event listeners are not added.
        if (!InspectorController.isWindowVisible())
            return;

        this.registerMutationEventListeners(inspectedWindow);

        var inspectedRootDocument = inspectedWindow.document;
        this.rootDOMNode = inspectedRootDocument;

        var canidateFocusNode = inspectedRootDocument.body || inspectedRootDocument.documentElement;
        if (canidateFocusNode) {
            this.focusedDOMNode = canidateFocusNode;
            if (this.treeOutline.selectedTreeElement)
                this.treeOutline.selectedTreeElement.expand();
        }
    },

    inspectedWindowCleared: function(window)
    {
        if (InspectorController.isWindowVisible())
            this.updateMutationEventListeners(window);
    },

    _addMutationEventListeners: function(monitoredWindow)
    {
        monitoredWindow.document.addEventListener("DOMNodeInserted", this._nodeInsertedEventListener, true);
        monitoredWindow.document.addEventListener("DOMNodeRemoved", this._nodeRemovedEventListener, true);
        if (monitoredWindow.frameElement)
            monitoredWindow.addEventListener("DOMContentLoaded", this._contentLoadedEventListener, true);
    },

    _removeMutationEventListeners: function(monitoredWindow)
    {
        if (monitoredWindow.frameElement)
            monitoredWindow.removeEventListener("DOMContentLoaded", this._contentLoadedEventListener, true);
        if (!monitoredWindow.document)
            return;
        monitoredWindow.document.removeEventListener("DOMNodeInserted", this._nodeInsertedEventListener, true);
        monitoredWindow.document.removeEventListener("DOMNodeRemoved", this._nodeRemovedEventListener, true);
    },

    updateMutationEventListeners: function(monitoredWindow)
    {
        this._addMutationEventListeners(monitoredWindow);
    },

    registerMutationEventListeners: function(monitoredWindow)
    {
        if (!monitoredWindow || this._mutationMonitoredWindows.indexOf(monitoredWindow) !== -1)
            return;
        this._mutationMonitoredWindows.push(monitoredWindow);
        if (InspectorController.isWindowVisible())
            this._addMutationEventListeners(monitoredWindow);
    },

    unregisterMutationEventListeners: function(monitoredWindow)
    {
        if (!monitoredWindow || this._mutationMonitoredWindows.indexOf(monitoredWindow) === -1)
            return;
        this._mutationMonitoredWindows.remove(monitoredWindow);
        this._removeMutationEventListeners(monitoredWindow);
    },

    unregisterAllMutationEventListeners: function()
    {
        for (var i = 0; i < this._mutationMonitoredWindows.length; ++i)
            this._removeMutationEventListeners(this._mutationMonitoredWindows[i]);
        this._mutationMonitoredWindows = [];
    },

    get rootDOMNode()
    {
        return this.treeOutline.rootDOMNode;
    },

    set rootDOMNode(x)
    {
        this.treeOutline.rootDOMNode = x;
    },

    get focusedDOMNode()
    {
        return this.treeOutline.focusedDOMNode;
    },

    set focusedDOMNode(x)
    {
        this.treeOutline.focusedDOMNode = x;
    },

    _contentLoaded: function(event)
    {
        this.recentlyModifiedNodes.push({node: event.target, parent: event.target.defaultView.frameElement, replaced: true});
        if (this.visible)
            this._updateModifiedNodesSoon();
    },

    _nodeInserted: function(event)
    {
        this.recentlyModifiedNodes.push({node: event.target, parent: event.relatedNode, inserted: true});
        if (this.visible)
            this._updateModifiedNodesSoon();
    },

    _nodeRemoved: function(event)
    {
        this.recentlyModifiedNodes.push({node: event.target, parent: event.relatedNode, removed: true});
        if (this.visible)
            this._updateModifiedNodesSoon();
    },

    _updateModifiedNodesSoon: function()
    {
        if ("_updateModifiedNodesTimeout" in this)
            return;
        this._updateModifiedNodesTimeout = setTimeout(this._updateModifiedNodes.bind(this), 0);
    },

    _updateModifiedNodes: function()
    {
        if ("_updateModifiedNodesTimeout" in this) {
            clearTimeout(this._updateModifiedNodesTimeout);
            delete this._updateModifiedNodesTimeout;
        }

        var updatedParentTreeElements = [];
        var updateBreadcrumbs = false;

        for (var i = 0; i < this.recentlyModifiedNodes.length; ++i) {
            var replaced = this.recentlyModifiedNodes[i].replaced;
            var parent = this.recentlyModifiedNodes[i].parent;
            if (!parent)
                continue;

            var parentNodeItem = this.treeOutline.findTreeElement(parent, null, null, objectsAreSame);
            if (parentNodeItem && !parentNodeItem.alreadyUpdatedChildren) {
                parentNodeItem.updateChildren(replaced);
                parentNodeItem.alreadyUpdatedChildren = true;
                updatedParentTreeElements.push(parentNodeItem);
            }

            if (!updateBreadcrumbs && (objectsAreSame(this.focusedDOMNode, parent) || isAncestorIncludingParentFrames(this.focusedDOMNode, parent)))
                updateBreadcrumbs = true;
        }

        for (var i = 0; i < updatedParentTreeElements.length; ++i)
            delete updatedParentTreeElements[i].alreadyUpdatedChildren;

        this.recentlyModifiedNodes = [];

        if (updateBreadcrumbs)
            this.updateBreadcrumb(true);
    },

    _stylesPaneEdited: function()
    {
        this.sidebarPanes.metrics.needsUpdate = true;
        this.updateMetrics();
    },

    _metricsPaneEdited: function()
    {
        this.sidebarPanes.styles.needsUpdate = true;
        this.updateStyles(true);
    },

    updateBreadcrumb: function(forceUpdate)
    {
        if (!this.visible)
            return;

        var crumbs = this.crumbsElement;

        var handled = false;
        var foundRoot = false;
        var crumb = crumbs.firstChild;
        while (crumb) {
            if (objectsAreSame(crumb.representedObject, this.rootDOMNode))
                foundRoot = true;

            if (foundRoot)
                crumb.addStyleClass("dimmed");
            else
                crumb.removeStyleClass("dimmed");

            if (objectsAreSame(crumb.representedObject, this.focusedDOMNode)) {
                crumb.addStyleClass("selected");
                handled = true;
            } else {
                crumb.removeStyleClass("selected");
            }

            crumb = crumb.nextSibling;
        }

        if (handled && !forceUpdate) {
            // We don't need to rebuild the crumbs, but we need to adjust sizes
            // to reflect the new focused or root node.
            this.updateBreadcrumbSizes();
            return;
        }

        crumbs.removeChildren();

        var panel = this;
        var selectCrumbFunction = function(event) {
            var crumb = event.currentTarget;
            if (crumb.hasStyleClass("collapsed")) {
                // Clicking a collapsed crumb will expose the hidden crumbs.
                if (crumb === panel.crumbsElement.firstChild) {
                    // If the focused crumb is the first child, pick the farthest crumb
                    // that is still hidden. This allows the user to expose every crumb.
                    var currentCrumb = crumb;
                    while (currentCrumb) {
                        var hidden = currentCrumb.hasStyleClass("hidden");
                        var collapsed = currentCrumb.hasStyleClass("collapsed");
                        if (!hidden && !collapsed)
                            break;
                        crumb = currentCrumb;
                        currentCrumb = currentCrumb.nextSibling;
                    }
                }

                panel.updateBreadcrumbSizes(crumb);
            } else {
                // Clicking a dimmed crumb or double clicking (event.detail >= 2)
                // will change the root node in addition to the focused node.
                if (event.detail >= 2 || crumb.hasStyleClass("dimmed"))
                    panel.rootDOMNode = crumb.representedObject.parentNode;
                panel.focusedDOMNode = crumb.representedObject;
            }

            WebInspector.currentFocusElement = document.getElementById("main-panels");

            event.preventDefault();
        };

        var mouseOverCrumbFunction = function(event) {
            panel.mouseOverCrumb = true;

            WebInspector.hoveredDOMNode = this.representedObject;
            WebInspector.forceHoverHighlight = this.hasStyleClass("selected");

            if ("mouseOutTimeout" in panel) {
                clearTimeout(panel.mouseOutTimeout);
                delete panel.mouseOutTimeout;
            }
        };

        var mouseOutCrumbFunction = function(event) {
            delete panel.mouseOverCrumb;

            if (event.target === this) {
                WebInspector.hoveredDOMNode = null;
                WebInspector.forceHoverHighlight = false;
            }

            if ("mouseOutTimeout" in panel) {
                clearTimeout(panel.mouseOutTimeout);
                delete panel.mouseOutTimeout;
            }

            var timeoutFunction = function() {
                if (!panel.mouseOverCrumb)
                    panel.updateBreadcrumbSizes();
            };

            panel.mouseOutTimeout = setTimeout(timeoutFunction, 500);
        };

        foundRoot = false;
        for (var current = this.focusedDOMNode; current; current = parentNodeOrFrameElement(current)) {
            if (current.nodeType === Node.DOCUMENT_NODE)
                continue;

            if (objectsAreSame(current, this.rootDOMNode))
                foundRoot = true;

            var crumb = document.createElement("span");
            crumb.className = "crumb";
            crumb.representedObject = current;
            crumb.addEventListener("mousedown", selectCrumbFunction, false);
            crumb.addEventListener("mouseover", mouseOverCrumbFunction.bind(crumb), false);
            crumb.addEventListener("mouseout", mouseOutCrumbFunction.bind(crumb), false);

            var crumbTitle;
            switch (current.nodeType) {
                case Node.ELEMENT_NODE:
                    crumbTitle = current.nodeName.toLowerCase();

                    var nameElement = document.createElement("span");
                    nameElement.textContent = crumbTitle;
                    crumb.appendChild(nameElement);

                    var idAttribute = current.getAttribute("id");
                    if (idAttribute) {
                        var idElement = document.createElement("span");
                        crumb.appendChild(idElement);

                        var part = "#" + idAttribute;
                        crumbTitle += part;
                        idElement.appendChild(document.createTextNode(part));

                        // Mark the name as extra, since the ID is more important.
                        nameElement.className = "extra";
                    }

                    var classAttribute = current.getAttribute("class");
                    if (classAttribute) {
                        var classes = classAttribute.split(/\s+/);
                        var foundClasses = {};

                        if (classes.length) {
                            var classesElement = document.createElement("span");
                            classesElement.className = "extra";
                            crumb.appendChild(classesElement);

                            for (var i = 0; i < classes.length; ++i) {
                                var className = classes[i];
                                if (className && !(className in foundClasses)) {
                                    var part = "." + className;
                                    crumbTitle += part;
                                    classesElement.appendChild(document.createTextNode(part));
                                    foundClasses[className] = true;
                                }
                            }
                        }
                    }

                    break;

                case Node.TEXT_NODE:
                    if (isNodeWhitespace.call(current))
                        crumbTitle = WebInspector.UIString("(whitespace)");
                    else
                        crumbTitle = WebInspector.UIString("(text)");
                    break

                case Node.COMMENT_NODE:
                    crumbTitle = "<!-->";
                    break;

                case Node.DOCUMENT_TYPE_NODE:
                    crumbTitle = "<!DOCTYPE>";
                    break;

                default:
                    crumbTitle = current.nodeName.toLowerCase();
            }

            if (!crumb.childNodes.length) {
                var nameElement = document.createElement("span");
                nameElement.textContent = crumbTitle;
                crumb.appendChild(nameElement);
            }

            crumb.title = crumbTitle;

            if (foundRoot)
                crumb.addStyleClass("dimmed");
            if (objectsAreSame(current, this.focusedDOMNode))
                crumb.addStyleClass("selected");
            if (!crumbs.childNodes.length)
                crumb.addStyleClass("end");

            crumbs.appendChild(crumb);
        }

        if (crumbs.hasChildNodes())
            crumbs.lastChild.addStyleClass("start");

        this.updateBreadcrumbSizes();
    },

    updateBreadcrumbSizes: function(focusedCrumb)
    {
        if (!this.visible)
            return;

        if (document.body.offsetWidth <= 0) {
            // The stylesheet hasn't loaded yet or the window is closed,
            // so we can't calculate what is need. Return early.
            return;
        }

        var crumbs = this.crumbsElement;
        if (!crumbs.childNodes.length || crumbs.offsetWidth <= 0)
            return; // No crumbs, do nothing.

        // A Zero index is the right most child crumb in the breadcrumb.
        var selectedIndex = 0;
        var focusedIndex = 0;
        var selectedCrumb;

        var i = 0;
        var crumb = crumbs.firstChild;
        while (crumb) {
            // Find the selected crumb and index. 
            if (!selectedCrumb && crumb.hasStyleClass("selected")) {
                selectedCrumb = crumb;
                selectedIndex = i;
            }

            // Find the focused crumb index. 
            if (crumb === focusedCrumb)
                focusedIndex = i;

            // Remove any styles that affect size before
            // deciding to shorten any crumbs.
            if (crumb !== crumbs.lastChild)
                crumb.removeStyleClass("start");
            if (crumb !== crumbs.firstChild)
                crumb.removeStyleClass("end");

            crumb.removeStyleClass("compact");
            crumb.removeStyleClass("collapsed");
            crumb.removeStyleClass("hidden");

            crumb = crumb.nextSibling;
            ++i;
        }

        // Restore the start and end crumb classes in case they got removed in coalesceCollapsedCrumbs().
        // The order of the crumbs in the document is opposite of the visual order.
        crumbs.firstChild.addStyleClass("end");
        crumbs.lastChild.addStyleClass("start");

        function crumbsAreSmallerThanContainer()
        {
            var rightPadding = 20;
            var errorWarningElement = document.getElementById("error-warning-count");
            if (!WebInspector.console.visible && errorWarningElement)
                rightPadding += errorWarningElement.offsetWidth;
            return ((crumbs.totalOffsetLeft + crumbs.offsetWidth + rightPadding) < window.innerWidth);
        }

        if (crumbsAreSmallerThanContainer())
            return; // No need to compact the crumbs, they all fit at full size.

        var BothSides = 0;
        var AncestorSide = -1;
        var ChildSide = 1;

        function makeCrumbsSmaller(shrinkingFunction, direction, significantCrumb)
        {
            if (!significantCrumb)
                significantCrumb = (focusedCrumb || selectedCrumb);

            if (significantCrumb === selectedCrumb)
                var significantIndex = selectedIndex;
            else if (significantCrumb === focusedCrumb)
                var significantIndex = focusedIndex;
            else {
                var significantIndex = 0;
                for (var i = 0; i < crumbs.childNodes.length; ++i) {
                    if (crumbs.childNodes[i] === significantCrumb) {
                        significantIndex = i;
                        break;
                    }
                }
            }

            function shrinkCrumbAtIndex(index)
            {
                var shrinkCrumb = crumbs.childNodes[index];
                if (shrinkCrumb && shrinkCrumb !== significantCrumb)
                    shrinkingFunction(shrinkCrumb);
                if (crumbsAreSmallerThanContainer())
                    return true; // No need to compact the crumbs more.
                return false;
            }

            // Shrink crumbs one at a time by applying the shrinkingFunction until the crumbs
            // fit in the container or we run out of crumbs to shrink.
            if (direction) {
                // Crumbs are shrunk on only one side (based on direction) of the signifcant crumb.
                var index = (direction > 0 ? 0 : crumbs.childNodes.length - 1);
                while (index !== significantIndex) {
                    if (shrinkCrumbAtIndex(index))
                        return true;
                    index += (direction > 0 ? 1 : -1);
                }
            } else {
                // Crumbs are shrunk in order of descending distance from the signifcant crumb,
                // with a tie going to child crumbs.
                var startIndex = 0;
                var endIndex = crumbs.childNodes.length - 1;
                while (startIndex != significantIndex || endIndex != significantIndex) {
                    var startDistance = significantIndex - startIndex;
                    var endDistance = endIndex - significantIndex;
                    if (startDistance >= endDistance)
                        var index = startIndex++;
                    else
                        var index = endIndex--;
                    if (shrinkCrumbAtIndex(index))
                        return true;
                }
            }

            // We are not small enough yet, return false so the caller knows.
            return false;
        }

        function coalesceCollapsedCrumbs()
        {
            var crumb = crumbs.firstChild;
            var collapsedRun = false;
            var newStartNeeded = false;
            var newEndNeeded = false;
            while (crumb) {
                var hidden = crumb.hasStyleClass("hidden");
                if (!hidden) {
                    var collapsed = crumb.hasStyleClass("collapsed"); 
                    if (collapsedRun && collapsed) {
                        crumb.addStyleClass("hidden");
                        crumb.removeStyleClass("compact");
                        crumb.removeStyleClass("collapsed");

                        if (crumb.hasStyleClass("start")) {
                            crumb.removeStyleClass("start");
                            newStartNeeded = true;
                        }

                        if (crumb.hasStyleClass("end")) {
                            crumb.removeStyleClass("end");
                            newEndNeeded = true;
                        }

                        continue;
                    }

                    collapsedRun = collapsed;

                    if (newEndNeeded) {
                        newEndNeeded = false;
                        crumb.addStyleClass("end");
                    }
                } else
                    collapsedRun = true;
                crumb = crumb.nextSibling;
            }

            if (newStartNeeded) {
                crumb = crumbs.lastChild;
                while (crumb) {
                    if (!crumb.hasStyleClass("hidden")) {
                        crumb.addStyleClass("start");
                        break;
                    }
                    crumb = crumb.previousSibling;
                }
            }
        }

        function compact(crumb)
        {
            if (crumb.hasStyleClass("hidden"))
                return;
            crumb.addStyleClass("compact");
        }

        function collapse(crumb, dontCoalesce)
        {
            if (crumb.hasStyleClass("hidden"))
                return;
            crumb.addStyleClass("collapsed");
            crumb.removeStyleClass("compact");
            if (!dontCoalesce)
                coalesceCollapsedCrumbs();
        }

        function compactDimmed(crumb)
        {
            if (crumb.hasStyleClass("dimmed"))
                compact(crumb);
        }

        function collapseDimmed(crumb)
        {
            if (crumb.hasStyleClass("dimmed"))
                collapse(crumb);
        }

        if (!focusedCrumb) {
            // When not focused on a crumb we can be biased and collapse less important
            // crumbs that the user might not care much about.

            // Compact child crumbs.
            if (makeCrumbsSmaller(compact, ChildSide))
                return;

            // Collapse child crumbs.
            if (makeCrumbsSmaller(collapse, ChildSide))
                return;

            // Compact dimmed ancestor crumbs.
            if (makeCrumbsSmaller(compactDimmed, AncestorSide))
                return;

            // Collapse dimmed ancestor crumbs.
            if (makeCrumbsSmaller(collapseDimmed, AncestorSide))
                return;
        }

        // Compact ancestor crumbs, or from both sides if focused.
        if (makeCrumbsSmaller(compact, (focusedCrumb ? BothSides : AncestorSide)))
            return;

        // Collapse ancestor crumbs, or from both sides if focused.
        if (makeCrumbsSmaller(collapse, (focusedCrumb ? BothSides : AncestorSide)))
            return;

        if (!selectedCrumb)
            return;

        // Compact the selected crumb.
        compact(selectedCrumb);
        if (crumbsAreSmallerThanContainer())
            return;

        // Collapse the selected crumb as a last resort. Pass true to prevent coalescing.
        collapse(selectedCrumb, true);
    },

    updateStyles: function(forceUpdate)
    {
        var stylesSidebarPane = this.sidebarPanes.styles;
        if (!stylesSidebarPane.expanded || !stylesSidebarPane.needsUpdate)
            return;

        stylesSidebarPane.update(this.focusedDOMNode, null, forceUpdate);
        stylesSidebarPane.needsUpdate = false;
    },

    updateMetrics: function()
    {
        var metricsSidebarPane = this.sidebarPanes.metrics;
        if (!metricsSidebarPane.expanded || !metricsSidebarPane.needsUpdate)
            return;

        metricsSidebarPane.update(this.focusedDOMNode);
        metricsSidebarPane.needsUpdate = false;
    },

    updateProperties: function()
    {
        var propertiesSidebarPane = this.sidebarPanes.properties;
        if (!propertiesSidebarPane.expanded || !propertiesSidebarPane.needsUpdate)
            return;

        propertiesSidebarPane.update(this.focusedDOMNode);
        propertiesSidebarPane.needsUpdate = false;
    },

    handleKeyEvent: function(event)
    {
        this.treeOutline.handleKeyEvent(event);
    },

    handleCopyEvent: function(event)
    {
        // Don't prevent the normal copy if the user has a selection.
        if (!window.getSelection().isCollapsed)
            return;

        switch (this.focusedDOMNode.nodeType) {
            case Node.ELEMENT_NODE:
                var data = this.focusedDOMNode.outerHTML;
                break;

            case Node.COMMENT_NODE:
                var data = "<!--" + this.focusedDOMNode.nodeValue + "-->";
                break;

            default:
            case Node.TEXT_NODE:
                var data = this.focusedDOMNode.nodeValue;
        }

        event.clipboardData.clearData();
        event.preventDefault();

        if (data)
            event.clipboardData.setData("text/plain", data);
    },

    rightSidebarResizerDragStart: function(event)
    {
        WebInspector.elementDragStart(this.sidebarElement, this.rightSidebarResizerDrag.bind(this), this.rightSidebarResizerDragEnd.bind(this), event, "col-resize");
    },

    rightSidebarResizerDragEnd: function(event)
    {
        WebInspector.elementDragEnd(event);
    },

    rightSidebarResizerDrag: function(event)
    {
        var x = event.pageX;
        var newWidth = Number.constrain(window.innerWidth - x, Preferences.minElementsSidebarWidth, window.innerWidth * 0.66);

        this.sidebarElement.style.width = newWidth + "px";
        this.contentElement.style.right = newWidth + "px";
        this.sidebarResizeElement.style.right = (newWidth - 3) + "px";

        this.treeOutline.updateSelection();

        event.preventDefault();
    }
}

WebInspector.ElementsPanel.prototype.__proto__ = WebInspector.Panel.prototype;

