#pragma once
#include <vector>
#include <memory>
#include "Hexnode.h"

using std::vector;
using std::unique_ptr;

class MinHeap
{
private:
	vector<HexNode*>& m_vector;
	unique_ptr<NodeAstarData>& m_MappedData;
	void SortDown(int index);
	void SortUp(int index);
	void MakeHeapy();
public:
	MinHeap(vector<HexNode*>& deq, unique_ptr<NodeAstarData>& mData);
	void Insert(HexNode* newVal);
	inline HexNode* GetMinFCostNode();
	void DeleteMinFCostNode();
	inline int size();
	inline vector<HexNode*>::iterator begin();
	inline vector<HexNode*>::iterator end();
	void SortNodeUp(HexNode* Node);
};

inline auto MinHeap::GetMinFCostNode() -> HexNode*
{
	return m_vector[0];
}

inline void MinHeap::DeleteMinFCostNode()
{
	if (static_cast<int>(m_vector.size()) == 0)
		return;

	m_vector[0] = m_vector[m_vector.size() - 1];
	m_vector.pop_back();
	SortDown(0);
}

inline int MinHeap::size() { return static_cast<int>(m_vector.size()); }
inline auto MinHeap::begin() -> vector<HexNode*>::iterator { return m_vector.begin(); }
inline auto MinHeap::end() -> vector<HexNode*>::iterator { return m_vector.end(); }