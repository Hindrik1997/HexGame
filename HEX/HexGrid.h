#pragma once
#include <vector>
#include "HexNode.h"
#include <algorithm>
#include <cstdlib>
#include <memory>

using std::vector;
using std::unique_ptr;
using std::min;
using std::max;
using std::abs;

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
	void CalculateCubicalCoordinates();
public:
	vector<HexNode*> FindPath(int StartNodeX, int StartNodeY, int EndNodeX, int EndNodeY); //Returns empty vector if no path is found!
	inline int GetDistance(int xStart, int yStart, int zStart, int xEnd, int yEnd, int zEnd);
	vector<HexNode*> RetracePath(HexNode* Start, HexNode* End, unique_ptr<NodeAstarData>& nData);
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


inline int HexGrid::GetDistance(int xStart, int yStart,int zStart, int xEnd, int yEnd, int zEnd)
{
	//Convert to cubical coords
	
	return (abs(xStart - xEnd) + abs(yStart - yEnd) + abs(zStart - zEnd)) / 2;


}