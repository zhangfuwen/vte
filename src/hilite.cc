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

#include "hilite.hh"
#include "debug.h"
#include "vtedefines.hh"
#include "vteinternal.hh"
#include <iostream>
#include <regex>

using namespace vte::base;

Hilite::Hilite() { m_matches = g_array_new(false, false, sizeof(struct HiliteMatch)); }

Hilite::~Hilite() { g_array_free(m_matches, true); }

void Hilite::clear() { g_array_set_size(m_matches, 0); }

/* FIXME Gotta find a regex instead */
void Hilite::find_word(const char *haystack, GArray *map, const HiLitePattern &pat) {
    if (pat.regex) {
        int off = 0;
        std::smatch sm;
        std::regex *regex_pattern;
        try {
            std::regex::flag_type flag{};
            if (pat.regex_flags & PCRE2_CASELESS) {
                flag |= std::regex_constants::icase;
            }
            regex_pattern = new std::regex(pat.pattern, flag);
        } catch (std::exception &e) {
            return;
        }
        while (true) {
            auto s = std::string(haystack + off);
            bool ret = std::regex_search(s, sm, *regex_pattern);
            if (ret) {
                auto start_pos = sm.position();
                auto end_pos = start_pos + sm.length();
                HiliteMatch match;

                match.start = g_array_index(map, vte::grid::coords, start_pos);
                match.end = g_array_index(map, vte::grid::coords, end_pos);
                match.fore = (pat.style.fore & pat.style.foremask);
                match.foremask = pat.style.foremask;
                match.back = (pat.style.back & pat.style.backmask);
                match.backmask = pat.style.backmask;
                match.deco = (pat.style.deco & pat.style.decomask);
                match.decomask = pat.style.decomask;
                match.attr = (pat.style.attr & pat.style.attrmask);
                match.attrmask = pat.style.attrmask;

                g_array_append_val(m_matches, match);

                off = end_pos + 1;
            } else {
                break;
            }
        }
        delete regex_pattern;
    } else {
        int off = 0;

        const char *p;

        while ((p = strcasestr(haystack + off, pat.pattern.c_str())) != NULL) {
            HiliteMatch match;

            match.start = g_array_index(map, vte::grid::coords, p - haystack);
            match.end = g_array_index(map, vte::grid::coords, p - haystack + pat.pattern.size());
            match.fore = (pat.style.fore & pat.style.foremask);
            match.foremask = pat.style.foremask;
            match.back = (pat.style.back & pat.style.backmask);
            match.backmask = pat.style.backmask;
            match.deco = (pat.style.deco & pat.style.decomask);
            match.decomask = pat.style.decomask;
            match.attr = (pat.style.attr & pat.style.attrmask);
            match.attrmask = pat.style.attrmask;

            g_array_append_val(m_matches, match);

            off = p - haystack + 1;
        }
    }
}

void Hilite::paragraph(RingView *ringview, vte::grid::row_t start, vte::grid::row_t end) {
    vte::grid::row_t row;
    vte::grid::column_t i;
    const VteRowData *row_data;
    const VteCell *cell;

    GString *string = g_string_new("");
    GArray *map =
        g_array_new(false, false, sizeof(vte::grid::coords)); // map from extracted byte offset to terminal coordinates

    for (row = start; row < end; row++) {
        row_data = ringview->get_row(row);

        for (i = 0; i < row_data->len; i++) {
            cell = _vte_row_data_get(row_data, i);
            if (cell->attr.fragment())
                continue;

            gsize prevlen = string->len;
            _vte_unistr_append_to_string(cell->c ? cell->c : ' ', string);

            vte::grid::coords pos{row, i};
            for (; prevlen < string->len; prevlen++) {
                g_array_append_val(map, pos);
            }
        }
    }

    if (G_UNLIKELY(string->len == 0))
        return;

    /* Guard */
    vte::grid::coords pos{row - 1, i}; /* {row, 0} would include empty area at the end of the line */
    g_array_append_val(map, pos);

    /* some examples to play with */
    for (auto pattern : m_patterns) {
        find_word(string->str, map, pattern);
    }

    g_array_free(map, true);
    g_string_free(string, true);
}

void Hilite::highlight(
    vte::grid::row_t row,
    vte::grid::column_t col,
    uint32_t *fore,
    uint32_t *back,
    uint32_t *deco,
    uint32_t *attr) const {
    vte::grid::coords coords{row, col};

    for (guint i = 0; i < m_matches->len; i++) {
        struct HiliteMatch *match = &g_array_index(m_matches, struct HiliteMatch, i);
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

int Hilite::add_pattern(const HiLitePattern &pattern) {
    m_patterns.emplace_back(pattern);
    return 0;
}

int Hilite::clear_patterns() {
    m_patterns.clear();
    return 0;
}
