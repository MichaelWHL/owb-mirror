/*
    Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <wildfox@kde.org>
                  2004, 2005 Rob Buis <buis@kde.org>
                  2005 Eric Seidel <eric.seidel@kdemail.net>

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "config.h"

#ifdef SVG_SUPPORT
#include "SVGFEImage.h"
#include "TextStream.h"

namespace WebCore {

SVGFEImage::SVGFEImage()
    : m_cachedImage(0)
{
}

SVGFEImage::~SVGFEImage()
{
    if (m_cachedImage)
        m_cachedImage->deref(this);
}

CachedImage* SVGFEImage::cachedImage() const
{
    return m_cachedImage;
}

void SVGFEImage::setCachedImage(CachedImage* image)
{
    if (m_cachedImage)
        m_cachedImage->deref(this);

    m_cachedImage = image;

    if (m_cachedImage)
        m_cachedImage->ref(this);
}

TextStream& SVGFEImage::externalRepresentation(TextStream& ts) const
{
    ts << "[type=IMAGE] ";
    SVGFilterEffect::externalRepresentation(ts);
    // FIXME: should this dump also object returned by SVGFEImage::image() ?
    return ts;

}

} // namespace WebCore

#endif // SVG_SUPPORT
