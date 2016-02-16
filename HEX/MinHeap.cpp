#include "MinHeap.h"

void MinHeap::SortDown(int index)
{
	int length = static_cast<int>(m_deque.size());
	int leftChildIndex = 2 * index + 1;
	int rightChildIndex = 2 * index + 2;

	if (leftChildIndex >= length)
		return;

	int minIndex = index;

	if (m_deque[index] == nullptr || m_deque[leftChildIndex] == nullptr || (&*m_MappedData)[m_deque[index]->m_GetID()].m_fCost() > (&*m_MappedData)[m_deque[leftChildIndex]->m_GetID()].m_fCost())
	{
		minIndex = leftChildIndex;
	}

	if (m_deque[minIndex] == nullptr || m_deque[rightChildIndex] == nullptr || (rightChildIndex < length) && ((&*m_MappedData)[m_deque[minIndex]->m_GetID()].m_fCost() >(&*m_MappedData)[m_deque[rightChildIndex]->m_GetID()].m_fCost()))
	{
		minIndex = rightChildIndex;
	}

	if (minIndex != index)
	{
		//swappen maar
		HexNode* temp = m_deque[index];
		m_deque[index] = m_deque[minIndex];
		m_deque[minIndex] = temp;
		SortDown(minIndex);
	}

}

void MinHeap::SortUp(int index)
{
	if (index == 0)
		return;

	int parentIndex = (index - 1) / 2;
	if (m_deque[parentIndex] == nullptr || m_deque[index] == nullptr || (&*m_MappedData)[m_deque[parentIndex]->m_GetID()].m_fCost() > (&*m_MappedData)[m_deque[index]->m_GetID()].m_fCost())
	{
		//Swappen
		HexNode* temp = m_deque[parentIndex];
		m_deque[parentIndex] = m_deque[index];
		m_deque[index] = temp;
		SortUp(parentIndex);
	}
}

void MinHeap::MakeHeapy()
{
	for (int i = static_cast<int>(m_deque.size()) - 1; i >= 0; --i)
	{
		SortDown(i);
	}
}

MinHeap::MinHeap(deque<HexNode*>& deq, unique_ptr<NodeAstarData>& mData) : m_deque(deq), m_MappedData(mData)
{
	MakeHeapy();
}

void MinHeap::Insert(HexNode* newVal)
{
	m_deque[m_deque.size()-1] = newVal;
	SortUp(static_cast<int>(m_deque.size()-1));
}