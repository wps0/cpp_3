#include <iostream>
#include <algorithm>
#include <compare>
#include "crosswords.h"

Word::Word(size_t x, size_t y, orientation_t wordOrientation, std::string wordContent) :
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
pos_t Word::get_start_position() const {
	return wordStart;
}
pos_t Word::get_end_position() const {
	size_t length = content.size() - 1;
	if (orientation == H) {
		return {wordStart.first + length, wordStart.second};
	} else {
		return {wordStart.first, wordStart.second + length};
	}
}
orientation_t Word::get_orientation() const {
	return orientation;
}
char Word::at(size_t pos) const {
	if (pos >= content.size()) return DEFAULT_CHAR;
	return content[pos];
}
size_t Word::length() const {
	return content.size();
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
	RectArea ra(get_start_position(), get_end_position());
	return ra;
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
pos_t RectArea::get_left_top() const {
	return leftUpper;
}
pos_t RectArea::get_right_bottom() const {
	return rightBottom;
}
pos_t RectArea::get_left_bottom() const {
	return {leftUpper.first, rightBottom.second};
}
pos_t RectArea::get_right_top() const {
	return {rightBottom.first, leftUpper.second};
}
void RectArea::set_left_top(pos_t point) {
	leftUpper = point;
}
void RectArea::set_right_bottom(pos_t point) {
	rightBottom = point;
}
void RectArea::set_left_bottom(pos_t point) {
	leftUpper.first = point.first;
	rightBottom.second = point.second;
}
void RectArea::set_right_top(pos_t point) {
	rightBottom.first = point.first;
	leftUpper.second = point.second;
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
bool RectArea::empty() const {
	return
		leftUpper.first > rightBottom.first ||
		leftUpper.second > rightBottom.second;
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