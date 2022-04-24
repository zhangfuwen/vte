/*
  * Copyright © 2019 Egmont Koblinger
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

#include "debug.h"
#include "hilite.hh"
#include "vtedefines.hh"
#include "vteinternal.hh"

using namespace vte::base;

Hilite::Hilite()
{
  m_matches = g_array_new (false, false, sizeof (struct HiliteMatch));
}

Hilite::~Hilite()
{
  g_array_free (m_matches, true);
}

void
Hilite::clear()
{
  g_array_set_size (m_matches, 0);
}

/* FIXME Gotta find a regex instead */
void
Hilite::find_word(const char *haystack, const char *needle, GArray *map,
                  uint32_t foremask, uint32_t fore,
                  uint32_t backmask, uint32_t back,
                  uint32_t decomask, uint32_t deco,
                  uint32_t attrmask, uint32_t attr)
{
  int off = 0;

  const char *p;

  while ((p = strcasestr(haystack + off, needle)) != NULL) {
    HiliteMatch match;

    match.start = g_array_index (map, vte::grid::coords, p - haystack);
    match.end = g_array_index (map, vte::grid::coords, p - haystack + strlen(needle));
    match.fore = (fore & foremask);
    match.foremask = foremask;
    match.back = (back & backmask);
    match.backmask = backmask;
    match.deco = (deco & decomask);
    match.decomask = decomask;
    match.attr = (attr & attrmask);
    match.attrmask = attrmask;

    g_array_append_val (m_matches, match);

    off = p - haystack + 1;
  }
}

void
Hilite::paragraph(RingView *ringview, vte::grid::row_t start, vte::grid::row_t end)
{
  vte::grid::row_t row;
  vte::grid::column_t i;
  const VteRowData *row_data;
  const VteCell *cell;

  GString *string = g_string_new("");
  GArray *map = g_array_new(false, false, sizeof (vte::grid::coords));  // map from extracted byte offset to terminal coordinates

  for (row = start; row < end; row++) {
    row_data = ringview->get_row(row);

    for (i = 0; i < row_data->len; i++) {
      cell = _vte_row_data_get (row_data, i);
      if (cell->attr.fragment())
        continue;

      gsize prevlen = string->len;
      _vte_unistr_append_to_string (cell->c ? cell->c : ' ', string);

      vte::grid::coords pos {row, i};
      for (; prevlen < string->len; prevlen++) {
        g_array_append_val(map, pos);
      }
    }
  }

  if (G_UNLIKELY (string->len == 0))
    return;

  /* Guard */
  vte::grid::coords pos {row - 1, i};  /* {row, 0} would include empty area at the end of the line */
  g_array_append_val(map, pos);

  /* some examples to play with */
  for(auto pattern : m_patterns) {
    find_word(string->str, pattern.pattern.c_str(), map,
              pattern.foremask, pattern.fore,
              pattern.backmask, pattern.back,
              pattern.decomask, pattern.deco,
              pattern.attrmask, pattern.attr
              );
  }

  g_array_free (map, true);
  g_string_free (string, true);
}

void
Hilite::highlight(vte::grid::row_t row,
                  vte::grid::column_t col,
                  uint32_t *fore,
                  uint32_t *back,
                  uint32_t *deco,
                  uint32_t *attr) const
{
  vte::grid::coords coords {row, col};

  for (guint i = 0; i < m_matches->len; i++) {
    struct HiliteMatch *match = &g_array_index (m_matches, struct HiliteMatch, i);
    if (coords >= match->start && coords < match->end) {
      *fore &= ~match->foremask;
      *fore |= match->fore;
      *back &= ~match->backmask;
      *back |= match->back;
      *deco &= ~match->decomask;
      *deco |= match->deco;
      *attr &= ~match->attrmask;
      *attr |= match->attr;
      return;
    }
  }
}

void Hilite::add_pattern(HilitePattern &pattern) {
  m_patterns.emplace_back(pattern);
}

void Hilite::clear_patterns() {
  m_patterns.clear();
}
