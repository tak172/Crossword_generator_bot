#pragma once
#include<iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include "Utf8.h"

class Generator 
{
	struct GoodWord 
	{
		size_t row;
		size_t colomn;
		size_t number_of_intersections;
	};
	enum class Direction
	{
		VERTICALLY,
		HORIZONTALLY
	};

public:
	std::string GetCrossword( bool ) const;
	void Generate( const std::vector<std::string> & );

private:
	void Preprocessing(  const std::vector<std::string> & );
	void Processing( size_t, Direction );
	bool HorizontalVariants( const std::wstring &, std::vector<GoodWord> & ) const;
	bool VerticalVariants( const std::wstring &, std::vector<GoodWord> & ) const;
	void VerticalPlacement( const GoodWord &, size_t );
	void HorizontalPlacement( const GoodWord &, size_t );
	void HorizontalCleaning( const GoodWord &, size_t );
	void VerticalCleaning( const GoodWord &, size_t );
	void VerticalProcessing( std::vector<GoodWord> &, size_t, size_t );
	void HorizontalProcessing( std::vector<GoodWord> &, size_t, size_t );

	std::vector<std::wstring> words_;
	std::vector<bool> used_;
	std::vector<std::vector<wchar_t>> board_;
	std::vector<std::vector<bool>> check_intersection_;
	bool success = false;

	clock_t start = clock();
	clock_t delay = 300 * CLOCKS_PER_SEC;
};
