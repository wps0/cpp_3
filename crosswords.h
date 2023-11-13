#ifndef CROSSWORDS_H
#define CROSSWORDS_H

#include <iostream>
#include <compare>
#include <set>
#include <optional>

enum orientation_t : bool {
	H, V
};

class RectArea;
class Word;

using cord_t = size_t;
using pos_t = std::pair<cord_t, cord_t>;
using dim_t = std::pair<cord_t, cord_t>;
// word with history
using hword_t = std::pair<Word, size_t>;

constexpr char DEFAULT_CHAR = '?';
constexpr std::string DEFAULT_WORD = "?";
constexpr char CROSSWORD_BACKGROUND = '.';

extern const RectArea DEFAULT_EMPTY_RECT_AREA;

class Word {
	private:
		pos_t wordStart;
		orientation_t orientation;
		std::string content;
	public:
		Word(size_t x, size_t y, orientation_t wordOrientation, std::string&& wordContent);
		Word(const Word& word);
		Word(Word&& word);
		Word& operator=(const Word& word);
		Word& operator=(Word&& word);
		inline pos_t get_start_position() const {
			return wordStart;
		}
		pos_t get_end_position() const;
		inline orientation_t get_orientation() const {
			return orientation;
		}
		char at(size_t pos) const;
        std::optional<char> at(pos_t pos) const;
        pos_t pos_of_letter(size_t offset) const;
		inline size_t length() const {
			return content.size();
		}
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
		inline pos_t get_left_top() const {
			return leftUpper;
		}
		inline pos_t get_right_bottom() const {
			return rightBottom;
		}
		inline pos_t get_left_bottom() const {
			return {leftUpper.first, rightBottom.second};
		}
		inline pos_t get_right_top() const {
			return {rightBottom.first, leftUpper.second};
		}
		inline void set_left_top(pos_t point) {
			leftUpper = point;
		}
		inline void set_right_bottom(pos_t point) {
			rightBottom = point;
		}
		inline void set_left_bottom(pos_t point) {
			leftUpper.first = point.first;
			rightBottom.second = point.second;
		}
		inline void set_right_top(pos_t point) {
			rightBottom.first = point.first;
			leftUpper.second = point.second;
		}
		const RectArea operator*(const RectArea& rectArea) const;
		RectArea& operator*=(const RectArea& rectArea);
		dim_t size() const;
		inline bool empty() const {
			return
			leftUpper.first > rightBottom.first ||
			leftUpper.second > rightBottom.second;
		}
		void embrace(pos_t point);
};

class Crossword {
private:
    std::set<hword_t> h_words, v_words;
    RectArea area;
    size_t time;

    bool are_colliding(Word const& other) const;
    std::optional<char> letter_at(pos_t pos);
    Word& closest_word(Word const& to) const;
    std::vector<hword_t> words() const;

public:
    Crossword(Word const& first, std::initializer_list<Word> other);

    inline dim_t size() const {
        return area.size();
    }
    // (H, V)
    inline dim_t word_count() const {
        return {h_words.size(), v_words.size()};
    }
    bool insert_word(Word const& w);
    Crossword operator+(const Crossword& b) const;
    Crossword& operator+=(const Crossword& b);
};

#endif