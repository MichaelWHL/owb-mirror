// -*- c-basic-offset: 2 -*-
/*
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NumberObject_h
#define NumberObject_h

#include "JSWrapperObject.h"

namespace KJS {

    class JSNumberCell;

    class NumberObject : public JSWrapperObject {
    public:
        NumberObject(JSObject* prototype);

        static const ClassInfo info;

    private:
        virtual const ClassInfo* classInfo() const { return &info; }

        virtual JSValue* getJSNumber();
    };

    NumberObject* constructNumber(ExecState*, JSNumberCell*);
    NumberObject* constructNumberFromImmediateNumber(ExecState*, JSValue*);

} // namespace KJS

#endif // NumberObject_h
