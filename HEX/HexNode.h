#pragma once
#include <vector>
#include <memory>
#include "State.h"

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
	inline State m_GetState();
	inline void m_SetState(State);
private:
	int m_X;
	int m_Y;
	const HexGrid* m_ParentGrid;
	State m_CurrentState = State::NONE;
public:
	int m_gCost = 0;
	int m_hCost = 0;
	vector<HexNode*> m_Neighbours;
	HexNode* m_Parent;
	inline int m_fCost();
};

inline int HexNode::m_GetX() { return m_X; }
inline int HexNode::m_GetY() { return m_Y; }
inline State HexNode::m_GetState() { return m_CurrentState; }
inline void HexNode::m_SetState(State state) { m_CurrentState = state; }
inline int HexNode::m_fCost() { return m_gCost + m_hCost; }