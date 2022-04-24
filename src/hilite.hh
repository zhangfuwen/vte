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

#include <glib.h>

#include <vector>

#include "ring.hh"
#include "ringview.hh"
#include "vterowdata.hh"
#include "vtetypes.hh"
#include "vteunistr.h"

namespace vte {

namespace base {

class RingView;

struct HiliteMatch {
  vte::grid::coords start;
  vte::grid::coords end; /* exclusive */
  uint32_t fore, foremask;
  uint32_t back, backmask;
  uint32_t deco, decomask;
  uint32_t attr, attrmask;
};

struct HilitePattern {
  std::string pattern;
  uint32_t fore, foremask;
  uint32_t back, backmask;
  uint32_t deco, decomask;
  uint32_t attr, attrmask;
};

class Hilite {
public:
  Hilite();
  ~Hilite();

  // prevent accidents
  Hilite(Hilite &o) = delete;
  Hilite(Hilite const &o) = delete;
  Hilite(Hilite &&o) = delete;
  Hilite &operator=(Hilite &o) = delete;
  Hilite &operator=(Hilite const &o) = delete;
  Hilite &operator=(Hilite &&o) = delete;

  void clear();
  void paragraph(RingView *ringview, vte::grid::row_t start,
                 vte::grid::row_t end);

  void highlight(vte::grid::row_t row, vte::grid::column_t col, uint32_t *fore,
                 uint32_t *back, uint32_t *deco, uint32_t *attr) const;

  void add_pattern(HilitePattern &pattern);
  void clear_patterns();

private:
  GArray *m_matches; /* array of HiliteMatch entries */
  std::vector<HilitePattern> m_patterns; /* array of pattern entries to hilight */

  void find_word(const char *haystack, const char *needle, GArray *map,
                 uint32_t foremask, uint32_t fore, uint32_t backmask,
                 uint32_t back, uint32_t decomask, uint32_t deco,
                 uint32_t attrmask, uint32_t attr);
};

}; /* namespace base */

} /* namespace vte */