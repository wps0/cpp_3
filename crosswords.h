#ifndef CROSSWORDS_H
#define CROSSWORDS_H

#include <iostream>
#include <compare>

enum orientation_t : bool {
	H, V
};

using pos_t = std::pair<size_t, size_t>;
using dim_t = std::pair<size_t, size_t>;

constexpr char DEFAULT_CHAR = '?';
constexpr std::string DEFAULT_WORD = "?";
constexpr char CROSSWORD_BACKGROUND = '.';

class RectArea;

class Word {
	private:
		pos_t wordStart;
		orientation_t orientation;
		std::string content;
	public:
		Word(size_t x, size_t y, orientation_t wordOrientation, std::string wordContent);
		Word(const Word& word);
		Word(Word&& word);
		Word& operator=(const Word& word);
		Word& operator=(Word&& word);
		pos_t get_start_position() const;
		pos_t get_end_position() const;
		orientation_t get_orientation() const;
		char at(size_t pos) const;
		size_t length() const;
		std::weak_ordering operator<=>(const Word& word) const;
		bool operator==(const Word& word) const;
		bool operator!=(const Word& word) const;
		RectArea rect_area() const;
};

class RectArea {
	private:
		pos_t leftUpper;
		pos_t rightBottom;
		bool pointInRect(pos_t point) const;
		bool rectInRect(RectArea rectArea) const;
	public:
		RectArea(pos_t left_top, pos_t right_bottom);
		RectArea(const RectArea& rectArea);
		RectArea(RectArea&& rectArea);
		RectArea& operator=(const RectArea& rectArea);
		RectArea& operator=(RectArea&& rectArea);
		pos_t get_left_top() const;
		pos_t get_right_bottom() const;
		pos_t get_left_bottom() const;
		pos_t get_right_top() const;
		void set_left_top(pos_t point);
		void set_right_bottom(pos_t point);
		void set_left_bottom(pos_t point);
		void set_right_top(pos_t point);
		const RectArea operator*(const RectArea& rectArea) const;
		RectArea& operator*=(const RectArea& rectArea);
		dim_t size() const;
		bool empty() const;
		void embrace(pos_t point);
};

const RectArea DEFAULT_EMPTY_RECT_AREA = RectArea({1, 1}, {0, 0});

#endif