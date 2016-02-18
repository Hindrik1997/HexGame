#include "MinHeap.h"

auto MinHeap::SortDown(int index) -> void
{
	int length = static_cast<int>(m_vector.size());
	int leftChildIndex = 2 * index + 1;
	int rightChildIndex = 2 * index + 2;

	if (leftChildIndex >= length)
		return;

	int minIndex = index;

	if (m_vector[index] == nullptr || m_vector[leftChildIndex] == nullptr || (&*m_MappedData)[m_vector[index]->m_GetID()].m_fCost() > (&*m_MappedData)[m_vector[leftChildIndex]->m_GetID()].m_fCost())
	{
		minIndex = leftChildIndex;
	}

	if (m_vector[minIndex] == nullptr || m_vector[rightChildIndex] == nullptr || (rightChildIndex < length) && ((&*m_MappedData)[m_vector[minIndex]->m_GetID()].m_fCost() >(&*m_MappedData)[m_vector[rightChildIndex]->m_GetID()].m_fCost()))
	{
		minIndex = rightChildIndex;
	}

	if (minIndex != index)
	{
		//swappen maar
		HexNode* temp = m_vector[index];
		m_vector[index] = m_vector[minIndex];
		m_vector[minIndex] = temp;
		SortDown(minIndex);
	}

}

auto MinHeap::SortUp(int index) -> void
{
	if (index == 0)
		return;

	int parentIndex = (index - 1) / 2;
	if (m_vector[parentIndex] == nullptr || m_vector[index] == nullptr || (&*m_MappedData)[m_vector[parentIndex]->m_GetID()].m_fCost() > (&*m_MappedData)[m_vector[index]->m_GetID()].m_fCost())
	{
		//Swappen
		HexNode* temp = m_vector[parentIndex];
		m_vector[parentIndex] = m_vector[index];
		m_vector[index] = temp;
		SortUp(parentIndex);
	}
}

auto MinHeap::MakeHeapy() -> void
{
	for (int i = static_cast<int>(m_vector.size()) - 1; i >= 0; --i)
	{
		SortDown(i);
	}
}

MinHeap::MinHeap(vector<HexNode*>& deq, unique_ptr<NodeAstarData>& mData) : m_vector(deq), m_MappedData(mData)
{
	MakeHeapy();
}

auto MinHeap::Insert(HexNode* newVal) -> void
{
	m_vector[m_vector.size()-1] = newVal;
	SortUp(static_cast<int>(m_vector.size()-1));
}

auto MinHeap::SortNodeUp(HexNode * Node) -> void
{
	for (int i = 0; i < m_vector.size(); ++i)
	{
		if (m_vector[i] == Node)
		{
			SortUp(i);
			return;
		}
	}
}