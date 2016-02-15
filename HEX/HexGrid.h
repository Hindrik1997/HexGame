#pragma once
#include <vector>
#include "HexNode.h"
using std::vector;

class HexGrid
{
public:
	HexGrid(unsigned int size);
	~HexGrid();

	//Operators
	const HexNode& operator()(unsigned nRow, unsigned nCol) const;
	HexNode& operator()(unsigned nRow, unsigned nCol);

	//Properties
	inline unsigned int get_Size() const;
private:
	//Private Members
	unsigned int m_Size = 0;
	vector< vector< HexNode > > m_Grid;
	void CreateGrid();
};

//Const versie van de () operator voor const Hexgrids
inline auto HexGrid::operator()(unsigned nRow, unsigned nCol) const -> const HexNode&
{
	if (nRow >= 0 && nRow < get_Size() && nCol >= 0 && nCol < get_Size())
	{
		return m_Grid[nRow][nCol];
	}
	else
		throw std::invalid_argument("Invalid index specified!");
}

//Normale variant
inline auto HexGrid::operator()(unsigned nRow, unsigned nCol) -> HexNode&
{
	if (nRow >= 0 && nRow < get_Size() && nCol >= 0 && nCol < get_Size())
	{
		return m_Grid[nRow][nCol];
	}
	else
		throw std::invalid_argument("Invalid index specified!");
}

//get_Size property
inline auto HexGrid::get_Size() const -> unsigned int
{
	return m_Size;
}

