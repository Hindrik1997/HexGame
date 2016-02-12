#pragma once
#include <vector>
#include <memory>

//Forward declaration vanwege circuliere dependency
class HexGrid;

using std::vector;

class HexNode
{
public:
	HexNode(int x, int y, const HexGrid& hRef);
	~HexNode();
	void CalculateConnections();
private:
	vector<HexNode*> m_Neighbours;
	int m_X;
	int m_Y;
	const HexGrid* m_ParentGrid;
};

