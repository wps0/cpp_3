#include <iostream>
#include <algorithm>
#include <compare>
#include <vector>
#include <cctype>
#include "crosswords.h"

const RectArea DEFAULT_EMPTY_RECT_AREA = RectArea({1, 1}, {0, 0});
char CROSSWORD_BACKGROUND = '.';

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

bool vertical_cmp::operator()(Word *w1, Word *w2) const {
    pos_t p1 = w1->get_start_position();
    pos_t p2 = w2->get_start_position();
    std::swap(p1.first, p1.second);
    std::swap(p2.first, p2.second);
    return p1 < p2;
}

Crossword::Crossword(Word const& first, std::initializer_list<Word> other) : h_words(), v_words(), words(), area(DEFAULT_EMPTY_RECT_AREA) {
    insert_word(first);
    std::for_each(other.begin(), other.end(), [this](Word const& w){
        this->insert_word(w);
    });
}
Crossword::~Crossword() {
    for (Word* w_ptr : words)
        delete w_ptr;
}
bool Crossword::does_collide(const Word &w) const {
    pos_t start = w.get_start_position();
    if (start.first > 0) {
        start.first--;
        if (letter_at(start).has_value())
            return true;
    }

    pos_t end = w.get_end_position();
    if (end.first < MAX_COORDINATE) {
        end.first++;
        if (letter_at(end).has_value())
            return true;
    }

    // TODO
    for (size_t i = 0; i < w.length(); i++) {
        pos_t pos = w.pos_of_letter(i);
        std::optional<char> letter = letter_at(pos);

        if (letter.has_value() && !Word::are_letters_the_same(*letter, w.at(i))) {
            return true;
        } else if (!letter.has_value()) {

            if (pos.second > 0) {
                pos.second--;
                if (letter_at(pos).has_value())
                    return true;
                pos.second++;
            }
            if (pos.second < MAX_COORDINATE) {
                pos.second++;
                if (letter_at(pos).has_value())
                    return true;
            }
        }
    }

    return false;
}
std::optional<char> Crossword::letter_at(pos_t pos) const {
    auto extract_letter = [](pos_t pos, std::optional<const Word *>&& w) {
            if (w.has_value()) {
                std::optional<char> let = w.value()->at(pos);
                if (let.has_value())
                    return let;
            }
            return std::optional<char>();
        };

    std::optional<char> l = extract_letter(pos, closest_word(pos, H));
    if (l.has_value())
        return l;
    return extract_letter(pos, closest_word(pos, V));
}
std::optional<const Word*> Crossword::closest_word(const pos_t &pos, orientation_t ori) const {
    if ((ori == H && h_words.empty()) || (ori == V && v_words.empty()))
        return {};

    Word tmp(pos.first, pos.second, ori, "");
    if (ori == H) {
        const std::set<Word*>* word_set = &h_words;
        auto it = word_set->upper_bound(&tmp);
        if (it != word_set->begin())
            it--;
        if (it == word_set->end())
            return {};
        return *it;
    } else {
        const std::set<Word*, vertical_cmp>* word_set = &v_words;
        auto it = word_set->upper_bound(&tmp);
        if (it != word_set->begin())
            it--;
        if (it == word_set->end())
            return {};
        return *it;
    }
}
// TODO: czy te slowa kopiowac??
bool Crossword::insert_word(const Word& w) {
    if (does_collide(w))
        return false;
    Word *w_ptr = new Word(w);
    insert_word_pointer(w_ptr);
}
bool Crossword::insert_word_pointer(Word *w) {
    words.push_back(w);
    if (w->get_orientation() == H)
        h_words.insert(w);
    else
        v_words.insert(w);

    area.embrace(w->get_start_position());
    area.embrace(w->get_end_position());
    return false;
}
Crossword Crossword::operator+(const Crossword& b) const {
    Crossword a = *this; // TODO
    return a += b;
}
Crossword& Crossword::operator+=(const Crossword& b) {
    for (const Word* w : b.words) { // TODO
        insert_word(*w);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const Crossword &crossword) {
    pos_t const& lt = crossword.area.get_left_top();
    pos_t const& rb = crossword.area.get_right_bottom();

    for (cord_t i = lt.second; i <= rb.second; i++) {
        pos_t cur = pos_t(lt.first, i);

        while (cur.first <= rb.first) {
            std::optional<char> letter = crossword.letter_at(cur);
            if (letter.has_value()) {
                os << isalpha(letter.value()) ? letter.value() : DEFAULT_CHAR;
            } else {
                os << CROSSWORD_BACKGROUND;
            }
            os << ' ';

            cur.first++;
        }
        os << std::endl;
    }
    return os;
}
