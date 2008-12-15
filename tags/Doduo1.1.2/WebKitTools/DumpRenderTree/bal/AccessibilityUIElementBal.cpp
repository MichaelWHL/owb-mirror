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

#include "AccessibilityUIElement.h"

AccessibilityUIElement::~AccessibilityUIElement() 
{
}

AccessibilityUIElement::AccessibilityUIElement(AccessibilityUIElement const&)
{
}

AccessibilityUIElement::AccessibilityUIElement(void*)
{
}

JSStringRef AccessibilityUIElement::allAttributes()
{
    return NULL;
}

JSStringRef AccessibilityUIElement::attributesOfLinkedUIElements()
{
    return NULL;
}

JSStringRef AccessibilityUIElement::attributesOfChildren()
{
    return NULL;
}

JSStringRef AccessibilityUIElement::role()
{
    return NULL;
}

JSStringRef AccessibilityUIElement::title()
{
    return NULL;
}

JSStringRef AccessibilityUIElement::description()
{
    return NULL;
}

double AccessibilityUIElement::width()
{
    return 0.0;
}

double AccessibilityUIElement::height()
{
    return 0.0;
}

double AccessibilityUIElement::intValue()
{
    return 0.0;
}

double AccessibilityUIElement::minValue()
{
    return 0.0;
}

double AccessibilityUIElement::maxValue()
{
    return 0.0;
}

int AccessibilityUIElement::insertionPointLineNumber()
{
    return -1;
}

// parameterized attributes
int AccessibilityUIElement::lineForIndex(int index)
{
    return -1;
}

AccessibilityUIElement AccessibilityUIElement::getChildAtIndex(unsigned index)
{
    AccessibilityUIElement a(NULL);
    return a;
}

AccessibilityUIElement AccessibilityUIElement::titleUIElement()
{
    AccessibilityUIElement a(NULL);
    return a;
}

JSStringRef AccessibilityUIElement::boundsForRange(unsigned location, unsigned length)
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::parameterizedAttributeNames()
{
    return JSStringRef();
}


JSStringRef AccessibilityUIElement::attributesOfColumnHeaders()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::attributesOfRowHeaders()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::attributesOfColumns()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::attributesOfRows()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::attributesOfVisibleCells()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::attributesOfHeader()
{
    return JSStringRef();
}

int AccessibilityUIElement::indexInTable()
{
    return 0;
}

JSStringRef AccessibilityUIElement::columnIndexRange()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::rowIndexRange() 
{
    return JSStringRef();
}

AccessibilityUIElement AccessibilityUIElement::cellForColumnAndRow(unsigned column, unsigned row)
{
    AccessibilityUIElement a(NULL);
    return a;
}

void AccessibilityUIElement::setSelectedTextRange(unsigned location, unsigned length) 
{
}

JSStringRef AccessibilityUIElement::attributeValue(JSStringRef attribute)
{
    return NULL;
}

bool AccessibilityUIElement::isAttributeSettable(JSStringRef attribute)
{
    return false;
}

bool AccessibilityUIElement::supportsPressAction()
{
    return false;
}

JSStringRef AccessibilityUIElement::selectedTextRange()
{
    return JSStringRef();
}

JSStringRef AccessibilityUIElement::attributesOfDocumentLinks()
{
    return JSStringRef();
}