#include <iostream>
#include <algorithm>
#include <compare>
#include <vector>
#include "crosswords.h"

const RectArea DEFAULT_EMPTY_RECT_AREA = RectArea({1, 1}, {0, 0});

Word::Word(size_t x, size_t y, orientation_t wordOrientation, std::string&& wordContent) :
	wordStart({x, y}), orientation(wordOrientation), content(wordContent) {
		if (content.empty()) {
			content = DEFAULT_WORD;
		}
		std::transform(
			content.begin(),
			content.end(),
			content.begin(),
			::toupper
		);
	}
Word::Word(const Word& word):
	wordStart(word.wordStart),
	orientation(word.orientation),
	content(word.content) {}
Word::Word(Word&& word):
	wordStart(std::move(word.wordStart)),
	orientation(std::move(word.orientation)),
	content(std::move(word.content)) {}
Word& Word::operator=(const Word& word) {
	wordStart = word.wordStart;
	orientation = word.orientation;
	content = word.content;
	return *this;
}
Word& Word::operator=(Word&& word) {
	wordStart = std::move(word.wordStart);
	orientation = std::move(word.orientation);
	content = std::move(word.content);
	return *this;
}
pos_t Word::get_end_position() const {
    return pos_of_letter(content.size() - 1);
}
char Word::at(size_t pos) const {
	if (pos >= content.size()) return DEFAULT_CHAR;
	return content[pos];
}
std::weak_ordering Word::operator<=>(const Word& word) const {
	if (wordStart.first == word.wordStart.first) {
		if (wordStart.second == word.wordStart.second) {
			if (orientation == word.orientation) {
				return std::weak_ordering::equivalent;
			} else if (orientation == H) {
				return std::weak_ordering::less;
			} else {
				return std::weak_ordering::greater;
			}
		} else {
			return wordStart.second <=> word.wordStart.second;
		}
	} else {
		return wordStart.first <=> word.wordStart.first;
	}
}
bool Word::operator==(const Word& word) const {
	return ((*this <=> word) == std::weak_ordering::equivalent);
}
bool Word::operator!=(const Word& word) const {
	return !(*this == word);
}
RectArea Word::rect_area() const {
	return RectArea(get_start_position(), get_end_position());
}
std::optional<char> Word::at(pos_t pos) const {
    if (wordStart <= pos && pos <= get_end_position()) {
        size_t offset = orientation == H ? wordStart.first - pos.first
                : wordStart.second - pos.second;
        return content[offset];
    }

    return {};
}
pos_t Word::pos_of_letter(size_t offset) const {
    if (orientation == H) {
        return {wordStart.first + offset, wordStart.second};
    } else {
        return {wordStart.first, wordStart.second + offset};
    }
}


bool RectArea::pointInRect(pos_t point) const {
	return
		point.first >= leftUpper.first &&
		point.second >= leftUpper.second &&
		point.first <= rightBottom.first &&
		point.second <= rightBottom.second;
}
bool RectArea::rectInRect(RectArea rectArea) const {
	return
		pointInRect(rectArea.get_left_top()) &&
		pointInRect(rectArea.get_right_top()) &&
		pointInRect(rectArea.get_left_bottom()) &&
		pointInRect(rectArea.get_right_bottom());
}
RectArea::RectArea(pos_t left_top, pos_t right_bottom):
	leftUpper(left_top),
	rightBottom(right_bottom) {}
RectArea::RectArea(const RectArea& rectArea):
	leftUpper(rectArea.leftUpper),
	rightBottom(rectArea.rightBottom) {}
RectArea::RectArea(RectArea&& rectArea):
	leftUpper(std::move(rectArea.leftUpper)),
	rightBottom(std::move(rectArea.rightBottom)) {}
RectArea& RectArea::operator=(const RectArea& rectArea) {
	leftUpper = rectArea.leftUpper;
	rightBottom = rectArea.rightBottom;
	return *this;
}
RectArea& RectArea::operator=(RectArea&& rectArea) {
	leftUpper = std::move(rectArea.leftUpper);
	rightBottom = std::move(rectArea.rightBottom);
	return *this;
}
const RectArea RectArea::operator*(const RectArea& rectArea) const {
	return RectArea(*this) *= rectArea;
}
RectArea& RectArea::operator*=(const RectArea& rectArea) {
	if (empty()) {
		return *this;
	}
	if (rectArea.empty()) {
		set_left_top({1, 1});
		set_right_bottom({0, 0});
		return *this;
	}

	if (rectInRect(rectArea)) {
		set_left_top(rectArea.get_left_top());
		set_right_bottom(rectArea.get_right_bottom());
	}
	else if (rectArea.rectInRect(*this)) {
		set_left_top(get_left_top());
		set_right_bottom(get_right_bottom());
	}
	// rectArea on right-bottom:
	else if (pointInRect(rectArea.get_left_top())) {
		set_left_top(rectArea.get_left_top());
		set_right_bottom(get_right_bottom());
	}
	// rectArea on left-bottom:
	else if (pointInRect(rectArea.get_right_top())) {
		set_right_top(rectArea.get_right_top());
		set_left_bottom(get_left_bottom());
	}
	// rectArea on left-top:
	else if (pointInRect(rectArea.get_right_bottom())) {
		set_left_top(get_left_top());
		set_right_bottom(rectArea.get_right_bottom());
	}
	// rectArea on right-top:
	else if (pointInRect(rectArea.get_left_bottom())) {
		set_right_top(get_right_top());
		set_left_bottom(rectArea.get_left_bottom());
	} else {
		set_left_top({1, 1});
		set_right_bottom({0, 0});
	}
	
	return *this;
}
dim_t RectArea::size() const {
	if (empty()) return {0, 0};
	
	size_t width = get_right_top().first - get_left_top().first + 1;
	size_t height = get_left_bottom().second - get_left_top().second + 1;
	return {width, height};
}
void RectArea::embrace(pos_t point) {
	if (pointInRect(point)) return;

	if (empty()) {
		leftUpper = point;
		rightBottom = point;
		return;
	}

	if (point.first < leftUpper.first) {
		leftUpper.first = point.first;
	} else if (point.first > rightBottom.first) {
		rightBottom.first = point.first;
	}

	if (point.second < leftUpper.second) {
		leftUpper.second = point.second;
	} else if (point.second > rightBottom.second) {
		rightBottom.second = point.second;
	}
}

bool Crossword::are_colliding(const Word &other) const {
    return false;
}

//std::optional<char> Crossword::letter_at(pos_t pos) {
//
//    std::optional<char> letter;
//    if (h_it != h_words.end()) {
//        letter = h_it->first.at(pos);
//        if (letter.has_value())
//            return *letter;
//    }
//
//    auto v_it = v_words.upper_bound({Word(pos.first, pos.second, V, ""), 0});
//    if (v_it != v_words.begin())
//        v_it--;
//    if (v_it != v_words.end())
//        letter = v_it->first.at(pos);
//    return letter;
//}

//std::optional<Word&> Crossword::closest_word(const Word &to) const {
//    const std::set<hword_t>* word_set = to.get_orientation() == H ? &h_words : &v_words;
//    pos_t start_pos = to.get_start_position();
//    auto it = word_set->upper_bound({Word(start_pos.first, start_pos.second, to.get_orientation(), ""), 0});
//    if (it != h_words.begin())
//        it--;
//    if (it == h_words.end())
//        return std::optional<Word&>();
//}

std::vector<hword_t> Crossword::words() const {
    std::vector<hword_t> words(h_words.begin(), h_words.end());
    words.insert(words.end(), v_words.begin(), v_words.end());
    std::sort(words.begin(), words.end(), [](hword_t const& a, hword_t const& b) {
        return a.second < b.second;
    });
    return words;
}

Crossword::Crossword(Word const& first, std::initializer_list<Word> other) : h_words(), v_words(), area(DEFAULT_EMPTY_RECT_AREA) {
    insert_word(first);
    std::for_each(other.begin(), other.end(), [this](Word const& w){
        this->insert_word(w);
    });
}

bool Crossword::insert_word(const Word& w) {
    for (size_t i = 0; i < w.length(); i++) {
        std::optional<char> letter = letter_at(w.pos_of_letter(i));
        if (letter.has_value() && *letter != w.at(i))
            return false;
    }

    if (w.get_orientation() == H)
        h_words.insert({w, time++});
    else
        v_words.insert({w, time++});
    area.embrace(w.get_start_position());
    area.embrace(w.get_end_position());
    return true;
}

Crossword Crossword::operator+(const Crossword& b) const {
    Crossword a = *this; // TODO
    return a += b;
}

Crossword& Crossword::operator+=(const Crossword& b) {
    for (hword_t& w : b.words()) { // TODO
        insert_word(w.first);
    }
    return *this;
}
