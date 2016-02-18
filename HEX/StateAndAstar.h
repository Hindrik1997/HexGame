#pragma once
#include <memory>

using std::unique_ptr;

enum class State : char {
	NONE,
	RED,
	BLUE
};

class HexNode;

struct NodeAstarData {
	int m_hCost;
	int m_gCost;
	HexNode* m_Parent;
	inline int m_fCost() { return m_hCost + m_gCost; }
	bool m_isInClosedSet = false;
	bool m_isInOpenSet = false;
	NodeAstarData(int hCost = 0, int gCost = 0, HexNode* parent = nullptr) : m_hCost(hCost), m_gCost(gCost), m_Parent(parent) {}
};