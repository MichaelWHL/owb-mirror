/*
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef RectImpl_h
#define RectImpl_h

#include "Shared.h"
#include "CSSPrimitiveValue.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class RectImpl : public Shared<RectImpl> {
public:
    virtual ~RectImpl() { }

    CSSPrimitiveValue* top() const { return m_top.get(); }
    CSSPrimitiveValue* right() const { return m_right.get(); }
    CSSPrimitiveValue* bottom() const { return m_bottom.get(); }
    CSSPrimitiveValue* left() const { return m_left.get(); }

    void setTop(PassRefPtr<CSSPrimitiveValue> top) { m_top = top; }
    void setRight(PassRefPtr<CSSPrimitiveValue> right) { m_right = right; }
    void setBottom(PassRefPtr<CSSPrimitiveValue> bottom) { m_bottom = bottom; }
    void setLeft(PassRefPtr<CSSPrimitiveValue> left) { m_left = left; }

protected:
    RefPtr<CSSPrimitiveValue> m_top;
    RefPtr<CSSPrimitiveValue> m_right;
    RefPtr<CSSPrimitiveValue> m_bottom;
    RefPtr<CSSPrimitiveValue> m_left;
};

} // namespace

#endif
