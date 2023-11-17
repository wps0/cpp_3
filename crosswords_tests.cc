/*
 * File:        crosswords_example.cc
 * Created on:  31/10/2023
 * Author:      Przemysław Rutka
 */

#include <cassert>
#include <iostream>
#include <utility>
#include "crosswords.h"

#define WORD_BASIC_ASSERTS(w, sp, ep, o, ci, c, l) \
do { \
    assert(w.get_start_position() == sp); \
    assert(w.get_end_position() == ep); \
    assert(w.get_orientation() == o); \
    assert(w.at(ci) == c); \
    assert(w.length() == l); \
} while (0)

#define RECT_AREA_BASE_ASSERTS(ra, lt, rb, s, e) \
do { \
    assert(ra.get_left_top() == lt); \
    assert(ra.get_right_bottom() == rb); \
    assert(ra.size() == s); \
    assert(ra.empty() == e); \
} while(0)

#define CROSSWORD_DIM_ASSERTS(cr, sz, cnt) \
do { \
    assert(cr.size() == sz); \
    assert(cr.word_count() == cnt); \
} while(0)

namespace {
    using orientation_t::H;
    using orientation_t::V;
    using std::cout;

    void crossword_tests() {
        Word w1(0, 0, H, "syrop");
        Word w2(0, 0, V, "Ssak");
        Word w3(4, 0, V, "pies");
        Word w4(10, 10, V, "Klepa minecraft");

        Crossword cr1(w1, {w2});
        Crossword cr2(w3, {});
        Crossword cr3(w4, {});
        cout << cr1 << '\n';
        cout << cr2 << '\n';
        cout << cr3 << '\n';

        cr2 += cr1;
        cr1 = std::move(cr3);
        cr2 += cr1;
        cout << cr2 << std::endl;
        cr3 += cr2;
        cout << cr3 << std::endl;
    }
}   /* anonymous namespace */

int main() {
    crossword_tests();
}
