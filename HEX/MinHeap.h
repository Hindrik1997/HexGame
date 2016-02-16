#pragma once
#include <deque>
#include <memory>
#include "Hexnode.h"

using std::deque;
using std::unique_ptr;

class MinHeap
{
private:
	deque<HexNode*>& m_deque;
	unique_ptr<NodeAstarData>& m_MappedData;
	void SortDown(int index);
	void SortUp(int index);
	void MakeHeapy();
public:
	MinHeap(deque<HexNode*>& deq, unique_ptr<NodeAstarData>& mData);
	void Insert(HexNode* newVal);
	inline HexNode* GetMinFCostNode();
	void DeleteMinFCostNode();
	inline int size();
	inline deque<HexNode*>::iterator begin();
	inline deque<HexNode*>::iterator end();
	inline void SortNodesUp();
};

inline auto MinHeap::GetMinFCostNode() -> HexNode*
{
	return m_deque[0];
}

inline void MinHeap::DeleteMinFCostNode()
{
	if (static_cast<int>(m_deque.size()) == 0)
		return;

	m_deque[0] = m_deque[m_deque.size() - 1];
	m_deque.pop_back();
	SortDown(0);
}

inline int MinHeap::size() { return static_cast<int>(m_deque.size()); }
inline auto MinHeap::begin() -> deque<HexNode*>::iterator { return m_deque.begin(); }
inline auto MinHeap::end() -> deque<HexNode*>::iterator { return m_deque.end(); }
inline auto MinHeap::SortNodesUp() -> void { MakeHeapy(); }