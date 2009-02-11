/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
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


#include "config.h"
#include "Identifier.h"

#include "bal_class.h"
#include "bal_instance.h"
#include "bal_runtime.h"

#include <WebObject.h>
#include "JSLock.h"

namespace JSC {
namespace Bindings {


BalClass::~BalClass()
{
    JSLock lock(false);

    deleteAllValues(m_methods);
    m_methods.clear();

    deleteAllValues(m_fields);
    m_fields.clear();
}
    
typedef HashMap<const WebObject*, BalClass*> ClassesByWebObject;
static ClassesByWebObject* classesByWebObject = 0;

BalClass* BalClass::classForObject(WebObject* o)
{
    if (!classesByWebObject)
        classesByWebObject = new ClassesByWebObject;

    const WebObject* mo = o;
    BalClass* aClass = classesByWebObject->get(mo);
    if (!aClass) {
        aClass = new BalClass();
        classesByWebObject->set(mo, aClass);
    }

    return aClass;
}

const char* BalClass::name() const
{
    return "";
}

MethodList BalClass::methodsNamed(const Identifier& identifier, Instance* instance) const
{
    MethodList methodList;

    Method* method = m_methods.get(identifier.ustring().rep());
    if (method) {
        methodList.append(method);
        return methodList;
    }

    const char *ident = identifier.ascii();
    const BalInstance* inst = static_cast<const BalInstance*>(instance);
    WebObject* obj = inst->getObject();
    if( obj->hasMethod( ident ) )
    {
        Method* aMethod = new BalMethod(ident); // deleted in the CClass destructor
        {
            JSLock lock(false);
            m_methods.set(identifier.ustring().rep(), aMethod);
        }
        methodList.append(aMethod);
    }

    return methodList;
}


Field* BalClass::fieldNamed(const Identifier& identifier, Instance *instance) const
{
    Field* aField = m_fields.get(identifier.ustring().rep());
    if (aField)
        return aField;
    const char *ident = identifier.ascii();
    const BalInstance* inst = static_cast<const BalInstance*>(instance);
    
    WebObject* obj = inst->getObject();
    if( obj->hasProperty( ident ) )
    {
        aField = new BalField(ident); // deleted in the CClass destructor
        {
            JSLock lock(false);
            m_fields.set(identifier.ustring().rep(), aField);
        }
    }
    return aField;
}

}
}

