#include "HexGrid.h"
#include "WindowFunctions.h"
#include <deque>
using std::deque;

//Creërt een grid. Called HexGrid::CreateGrid()
HexGrid::HexGrid(unsigned int size) : m_Size(size)
{
	if (m_Size <= 0 || m_Size > 25)
	{
		MessageBox(NULL, L"Ongeldige grootte gespecifiseerd!", L"Fout!", MB_ICONEXCLAMATION | MB_OK);
		throw std::invalid_argument("Ongeldige grid grootte"); //Lekker een exception gooien :)
	}
	CreateGrid();
}


HexGrid::~HexGrid()
{
}


//Creëert de werkelijke grid. Wordt standaard door de constructor gecalled.
auto HexGrid::CreateGrid() -> void
{
	if (m_Size <= 0)
	{
		MessageBox(NULL, L"Ongeldige grootte gespecifiseerd!", L"Fout!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	m_Grid.clear();
	for (unsigned iRow = 0; iRow < m_Size; ++iRow)
	{
		vector<HexNode> row;
		for (unsigned iColumn = 0; iColumn < m_Size; ++iColumn)
		{
			row.push_back(HexNode(iRow,iColumn,(*this)));
		}
		m_Grid.push_back(row);
	}
	for (unsigned i = 0; i < get_Size(); ++i)
	{
		for (unsigned j = 0; j < get_Size(); ++j)
		{
			m_Grid[i][j].CalculateConnections();
		}
	}

}

auto HexGrid::FindPath(int StartNodeX, int StartNodeY, int EndNodeX, int EndNodeY) -> vector<HexNode*>
{
	deque<HexNode*> OpenSet;
	deque<HexNode*> ClosedSet;
	HexNode* StartNode = &m_Grid[StartNodeX][StartNodeY];
	HexNode* EndNode = &m_Grid[EndNodeX][EndNodeY];
	OpenSet.push_back(StartNode);

	while (OpenSet.size() > 0)
	{
		HexNode* CurrentLocation = OpenSet.front();
		int IndexStorer = 0;
		for (int i = 0; i < OpenSet.size(); ++i)
		{
			if (OpenSet[i]->m_fCost() < CurrentLocation->m_fCost() || OpenSet[i]->m_fCost() == CurrentLocation->m_fCost() && OpenSet[i]->m_hCost < CurrentLocation->m_hCost)
			{
				CurrentLocation = OpenSet[i];
				IndexStorer = i;
			}
		}
		OpenSet.erase(OpenSet.begin() + IndexStorer);
		ClosedSet.push_back(CurrentLocation);

		if (CurrentLocation == EndNode)
		{
			//Retrace path
			return RetracePath(StartNode, EndNode);
		}
		for (auto it = CurrentLocation->m_Neighbours.begin(); it != CurrentLocation->m_Neighbours.end(); it++)
		{
			HexNode* NghBor = *it;
			if (NghBor->m_GetState() != StartNode->m_GetState() && NghBor->m_GetState() != State::NONE || std::find(ClosedSet.begin(), ClosedSet.end(), NghBor)!= ClosedSet.end() )
			{
				continue;
			}
			int newMoveCostToNgh = NghBor->m_gCost + GetDistance(CurrentLocation->m_GetX(), CurrentLocation->m_GetY(),NghBor->m_GetX(), NghBor->m_GetY());
			if (newMoveCostToNgh < NghBor->m_gCost || std::find(OpenSet.begin(), OpenSet.end(), NghBor) == OpenSet.end())
			{
				NghBor->m_gCost = newMoveCostToNgh;
				NghBor->m_hCost = GetDistance(NghBor->m_GetX(), NghBor->m_GetY(), EndNode->m_GetX(), EndNode->m_GetY());
				NghBor->m_Parent = CurrentLocation;

				if (std::find(OpenSet.begin(), OpenSet.end(), NghBor) == OpenSet.end())
				{
					OpenSet.push_back(NghBor);
				}
			}
		}
	}
	return vector<HexNode*>();
}

auto  HexGrid::RetracePath(HexNode * Start, HexNode * End) -> vector<HexNode*>
{
	vector<HexNode*> path;
	HexNode* CurrentObserved = End;
	while (CurrentObserved != Start)
	{
		path.push_back(CurrentObserved);
		CurrentObserved = CurrentObserved->m_Parent;
	}
	std::reverse(path.begin(), path.end());
	return path;
}