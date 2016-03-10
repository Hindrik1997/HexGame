#pragma once
#include <vector>
#include "HexNode.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <tuple>
#include <Windows.h>

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
	vector<HexNode*> FindPath(HexNode* StartNode, HexNode* EndNode); //Returns empty vector if no path is found!
	vector<HexNode*> FindBestPotentialPath(HexNode* StartNode, HexNode* EndNode); //Returns empty vector if no path is found!
	vector<HexNode*> FindBestPotentialNonWeightedPath(HexNode* StartNode, HexNode* EndNode); //Returns empty vector if no path is found!
	vector<HexNode*> RetracePath(HexNode* Start, HexNode* End, unique_ptr<NodeAstarData>& nData);

	static vector<HexNode*> GetFilteredPath(vector<HexNode*>& Path, HexNode* StartNode, HexNode* EndNode);
	static void GetConnectedNodeSet(HexNode* StartNode, vector<HexNode*>& CurrentSet, const vector<HexNode*>& TotalSet);

	inline int GetDistance(HexNode* FirstNode, HexNode* SecondNode);
	State GetVictorious();
	bool OccursInSets(HexNode* Node, vector< vector<HexNode*> >& Set);

	void PlayMove(Move move, HWND hwnd);
	std::tuple<Move, vector<HexNode*>, bool> ComputeBestMove();
	Move EvaluateComputedMove(std::tuple<Move, vector<HexNode*>, bool>);
public:
	vector<Move> PlayedMoves;
	State HumanPlayer = State::BLUE;
	vector<std::pair<int, int>> GetMoves();
	HexNode* TopNode;
	HexNode* BottomNode;
	HexNode* LeftNode;
	HexNode* RightNode;
	vector<HexNode*> PotPath;
	bool IsVisible = false;
};

//Const versie van de () operator voor const Hexgrids
inline auto HexGrid::operator()(unsigned nRow, unsigned nCol) const -> const HexNode&{
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

inline auto HexGrid::GetDistance(HexNode* FirstNode, HexNode* SecondNode) -> int
{
	return (abs(FirstNode->m_GetCubicalX() - SecondNode->m_GetCubicalX()) + abs(FirstNode->m_GetCubicalY() - SecondNode->m_GetCubicalY()) + abs(FirstNode->m_GetCubicalZ() - SecondNode->m_GetCubicalZ())) / 2;
}