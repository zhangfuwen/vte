/*
  * Copyright © 2018–2019 Egmont Koblinger
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

struct _HighlightStyle {
    uint32_t fore, foremask;
    uint32_t back, backmask;
    uint32_t deco, decomask;
    uint32_t attr, attrmask;
};

/**
 * HighlightPattern
 */
struct _HighlightPattern {
    const char * pattern;
    bool regex;
    uint32_t regex_flags;
    struct _HighlightStyle style;
};

typedef struct _HighlightPattern HighlightPattern;
typedef struct _HighlightStyle HighlightStyle ;

