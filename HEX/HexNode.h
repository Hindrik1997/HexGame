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
	inline int m_GetCubicalX() const;
	inline int m_GetCubicalY() const;
	inline int m_GetCubicalZ() const;

	inline void m_SetCubicalX(int x);
	inline void m_SetCubicalY(int y);
	inline void m_SetCubicalZ(int z);
	inline void m_SetCubicalCoords(int x, int y, int z);

	inline State m_GetState() const;
	inline void m_SetState(State);
	inline const int m_GetID() const;
	inline const HexGrid* m_GetHexGrid() const;
private:
	int m_X;
	int m_Y;
	int m_XC;
	int m_YC;
	int m_ZC;
	const HexGrid* m_ParentGrid;
	State m_CurrentState = State::NONE;
	const int m_ArrayID;
public:
	vector<HexNode*> m_Neighbours;
};

inline int HexNode::m_GetX() const { return m_X; }
inline int HexNode::m_GetY() const { return m_Y; }

inline int HexNode::m_GetCubicalX() const { return m_XC; }
inline int HexNode::m_GetCubicalY() const { return m_YC; }
inline int HexNode::m_GetCubicalZ() const { return m_ZC; }

inline void HexNode::m_SetCubicalX(int x) { m_XC = x; }
inline void HexNode::m_SetCubicalY(int y) { m_YC = y; }
inline void HexNode::m_SetCubicalZ(int z) { m_ZC = z; }
inline void HexNode::m_SetCubicalCoords(int x, int y, int z) { m_XC = x; m_YC = y; m_ZC = z; }

inline State HexNode::m_GetState() const { return m_CurrentState; }
inline void HexNode::m_SetState(State state) { m_CurrentState = state; }
inline const int HexNode::m_GetID() const { return m_ArrayID; };
inline const HexGrid* HexNode::m_GetHexGrid() const { return m_ParentGrid; }