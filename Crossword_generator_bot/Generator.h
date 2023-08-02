#pragma once
#include<iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

class Generator {

	struct GoodWord {
		size_t row;
		size_t colomn;
		size_t number_of_intersections;
	};

public:

	enum class Direction {
		VERTICALLY,
		HORIZONTALLY
	};

	explicit Generator(std::vector<std::string>& words);

	std::string GetCrossword(bool language_flag) const noexcept;
	void Generate(size_t count_used, Direction dir);

private:

	bool HorizontalVariants(const std::string& word, std::vector<GoodWord>& options) const;
	bool VerticalVariants(const std::string& word, std::vector<GoodWord>& options) const;

	void VerticalPlacement(const GoodWord& word, size_t number_of_word) noexcept;
	void HorizontalPlacement(const GoodWord& word, size_t number_of_word) noexcept;

	void HorizontalCleaning(const GoodWord& word, size_t number_of_word) noexcept;
	void VerticalCleaning(const GoodWord& word, size_t number_of_word) noexcept;

	void VerticalProcessing(std::vector<GoodWord>& options, size_t number_of_word, size_t count_used);
	void HorizontalProcessing(std::vector<GoodWord>& options, size_t number_of_word, size_t count_used);

	std::vector<std::string> words_;
	std::vector<bool> used_;
	std::vector<std::vector<char>> board_;
	std::vector<std::vector<bool>> check_intersection_;
	bool succsess = false;

	clock_t start = clock();
	clock_t delay = 300 * CLOCKS_PER_SEC;

};
