#include "generator.h"

Generator::Generator(std::vector<std::string>& w) : words_(std::move(w)), used_(words_.size(), false),
board_(50, std::vector<char>(50, '_')), check_intersection_(50, std::vector<bool>(50, false)) {

	std::sort(words_.begin(), words_.end(), [](const std::string& s1, const std::string& s2) {
		if (s1.size() > s2.size())
		return true;
		else if (s1.size() < s2.size())
			return false;
	return s1 < s2;
		});

	HorizontalPlacement({ 25, 25 - words_.size() / 2, 0 }, 0);
	used_[0] = true;

}

std::string Generator::GetCrossword(bool language_flag) const noexcept {

	if (!succsess)
		if (language_flag)
			return "I couldn't make a crossword puzzle out of these words. \xF0\x9F\x98\x94\nTry a different set of words.";
		else
			return "Я не смог составить кроссворд из данных слов. \xF0\x9F\x98\x94\nПопробуйте другой набор слов.";

	std::string crossword(
		"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
		"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
		"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
		"\t<head>\n"
		"\t<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"/>\n"
		"\t\t<style type = \"text/css\">\n"
		"\t\t\tTABLE{"
		"\t\t\t\tborder:4px double #399;\n"
		"\t\t\t}\n"
		"\t\t\tTD{\n"
		"\t\t\t\tfont-family:\"Courier New\";\n"
		"\t\t\t\tbackground:#fc0;\n"
		"\t\t\t\tborder:1px solid #333;\n"
		"\t\t\t\tempty-cells:hide;\n"
		"\t\t\t\tpadding:5px;\n"
		"\t\t\t}\n"
		"\t\t</style>\n"
		"\t</head>\n"
		"\t<body>\n"
		"\t\t<table>\n"
	);

	size_t left_border = 51, right_border = 0, upper_border = 51, bottom_border = 0;

	for (size_t row = 0; row < board_.size(); ++row) {
		for (size_t column = 0; column < board_.size(); ++column) {
			if (board_[row][column] != '_') {
				left_border = std::min(left_border, column);
				right_border = std::max(right_border, column);
				upper_border = std::min(upper_border, row);
				bottom_border = std::max(bottom_border, row);
			}
		}
	}

	for (size_t row = upper_border; row <= bottom_border; row++) {
		crossword += "\t\t\t\n<tr>\n";
		for (size_t column = left_border; column <= right_border; ++column) {
			if (board_[row][column] != '_') {
				crossword += "\t\t\t\t<td>";
				crossword += board_[row][column];
				crossword += "</td>\n";
			}
			else {
				crossword += "\t\t\t\t<td style=\"background : white\">";
				crossword += "</td>\n";
			}
		}
		crossword += "\t\t\t\n</tr>\n";
	}

	crossword += "</table>\n</body>\n</html>\n";

	return crossword;

}

void Generator::Generate(size_t count_used, Direction dir) {

	if (clock() - start > delay) return;

	if (count_used == words_.size())
		succsess = true;

	if (succsess)
		return;

	for (size_t number_of_word = 0; number_of_word < used_.size(); ++number_of_word) {

		std::vector<GoodWord> options;

		if (used_[number_of_word]) continue;

		if (dir == Direction::VERTICALLY) {
			if (VerticalVariants(words_[number_of_word], options))
				VerticalProcessing(options, number_of_word, count_used);

			else if (HorizontalVariants(words_[number_of_word], options))
				HorizontalProcessing(options, number_of_word, count_used);
		}
		else {
			if (HorizontalVariants(words_[number_of_word], options))
				HorizontalProcessing(options, number_of_word, count_used);

			else if (VerticalVariants(words_[number_of_word], options))
				VerticalProcessing(options, number_of_word, count_used);
		}
	}

}

bool Generator::HorizontalVariants(const std::string& word, std::vector<GoodWord>& options) const {

	size_t match, intersection;

	for (size_t row = 1; row < board_.size() - 1; ++row) {
		for (size_t column = 1; column < board_.size() - 1 - word.size(); ++column) {

			if (!(board_[row][column - 1] == '_' && board_[row][column + word.size()] == '_'))
				continue;

			match = 2;
			intersection = 0;

			for (size_t s = 0; s < word.size(); ++s) {

				if (board_[row][column + s] == '_')
					++match;
				else if (board_[row][column + s] == word[s] && board_[row][column + s + 1] == '_'
					&& board_[row][column + s - 1] == '_') {

					match += 3;
					++intersection;

				}

				if (board_[row + 1][column + s] == '_' && board_[row - 1][column + s] == '_')
					match += 2;

			}

			if (match == 3 * word.size() + 2 && intersection)
				options.push_back({ row, column, intersection });
		}
	}
	return options.size();

}

bool Generator::VerticalVariants(const std::string& word, std::vector<GoodWord>& options) const {

	size_t match, intersection;

	for (size_t column = 1; column < board_.size() - 1; ++column) {
		for (size_t row = 1; row < board_.size() - 1 - word.size(); ++row) {

			if (!(board_[row - 1][column] == '_' && board_[row + word.size()][column] == '_'))
				continue;

			match = 2;
			intersection = 0;

			for (size_t s = 0; s < word.size(); ++s) {

				if (board_[row + s][column] == '_')
					++match;
				else if (board_[row + s][column] == word[s] && board_[row + s + 1][column] == '_'
					&& board_[row + s - 1][column] == '_') {

					match += 3;
					++intersection;

				}
				else break;

				if (board_[row + s][column + 1] == '_' && board_[row + s][column - 1] == '_')
					match += 2;

			}
			if (match == 3 * word.size() + 2 && intersection)
				options.push_back({ row, column, intersection });

		}
	}
	return options.size();

}

void Generator::VerticalPlacement(const GoodWord& word, size_t number_of_word) noexcept {

	for (int i = 0; i < static_cast<int>(words_[number_of_word].size()); ++i) {

		if (board_[word.row + i][word.colomn] != '_')
			check_intersection_[word.row + i][word.colomn] = true;
		else
			board_[word.row + i][word.colomn] = words_[number_of_word][i];
	}
}

void Generator::HorizontalPlacement(const GoodWord& word, size_t number_of_word) noexcept {

	for (int i = 0; i < static_cast<int>(words_[number_of_word].size()); ++i) {

		if (board_[word.row][word.colomn + i] != '_')
			check_intersection_[word.row][word.colomn + i] = true;
		else
			board_[word.row][word.colomn + i] = words_[number_of_word][i];
	}
}

void Generator::HorizontalCleaning(const GoodWord& word, size_t number_of_word) noexcept {

	for (int i = 0; i < static_cast<int>(words_[number_of_word].size()); ++i) {

		if (check_intersection_[word.row][word.colomn + i]) {
			check_intersection_[word.row][word.colomn + i] = false;
			continue;
		}

		board_[word.row][word.colomn + i] = '_';
	}

}

void Generator::VerticalCleaning(const GoodWord& word, size_t number_of_word) noexcept {

	for (int i = 0; i < static_cast<int>(words_[number_of_word].size()); ++i) {

		if (check_intersection_[word.row + i][word.colomn]) {
			check_intersection_[word.row + i][word.colomn] = false;
			continue;
		}

		board_[word.row + i][word.colomn] = '_';

	}

}

void Generator::HorizontalProcessing(std::vector<GoodWord>& options, size_t number_of_word, size_t count_used) {

	used_[number_of_word] = true;

	std::sort(options.begin(), options.end(), [](const GoodWord& w1, const GoodWord& w2) {
		return w1.number_of_intersections > w2.number_of_intersections;
		});

	for (int i = 0; i < static_cast<int>(options.size()); ++i) {

		HorizontalPlacement(options[i], number_of_word);
		Generate(count_used + 1, Direction::VERTICALLY);

		if (succsess)
			return;

		HorizontalCleaning(options[i], number_of_word);
	}

	used_[number_of_word] = false;

}

void Generator::VerticalProcessing(std::vector<GoodWord>& options, size_t number_of_word, size_t count_used) {

	used_[number_of_word] = true;

	std::sort(options.begin(), options.end(), [](const GoodWord& w1, const GoodWord& w2) {
		return w1.number_of_intersections > w2.number_of_intersections;
		});

	for (int i = 0; i < static_cast<int>(options.size()); ++i) {

		VerticalPlacement(options[i], number_of_word);
		Generate(count_used + 1, Direction::HORIZONTALLY);

		if (succsess)
			return;

		VerticalCleaning(options[i], number_of_word);
	}

	used_[number_of_word] = false;

}