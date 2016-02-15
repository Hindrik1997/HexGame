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
	inline int m_GetX();
	inline int m_GetY();
private:
	vector<HexNode*> m_Neighbours;
	int m_X;
	int m_Y;
	const HexGrid* m_ParentGrid;
};

inline int HexNode::m_GetX() { return m_X; }
inline int HexNode::m_GetY() { return m_Y; }