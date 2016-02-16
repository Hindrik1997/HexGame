#pragma once
#include <vector>
#include <memory>
#include "StateAndAstar.h"

//Forward declaration vanwege circuliere dependency
class HexGrid;

using std::vector;

class HexNode
{
public:
	HexNode(int x, int y, const HexGrid& hRef,int ID);
	~HexNode();
	void CalculateConnections();
	inline int m_GetX() const;
	inline int m_GetY() const;
	inline State m_GetState() const;
	inline void m_SetState(State);
	inline const int m_GetID() const;
	inline const HexGrid* m_GetHexGrid() const;
private:
	int m_X;
	int m_Y;
	const HexGrid* m_ParentGrid;
	State m_CurrentState = State::NONE;
	const int m_ArrayID;
public:
	vector<HexNode*> m_Neighbours;
};

inline int HexNode::m_GetX() const { return m_X; }
inline int HexNode::m_GetY() const { return m_Y; }
inline State HexNode::m_GetState() const { return m_CurrentState; }
inline void HexNode::m_SetState(State state) { m_CurrentState = state; }
inline const int HexNode::m_GetID() const { return m_ArrayID; };
inline const HexGrid* HexNode::m_GetHexGrid() const { return m_ParentGrid; }