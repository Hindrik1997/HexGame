#include "HexGrid.h"
#include "WindowFunctions.h"
#include "StateAndAstar.h"
#include "MinHeap.h"
#include <unordered_set>

using std::unique_ptr;
using std::unordered_set;
/*
struct Comparator
{
	Comparator(unique_ptr<NodeAstarData>& mappedData) : m_MappedData(mappedData) {}
	unique_ptr<NodeAstarData>& m_MappedData;
	bool operator() (const HexNode* s1, const HexNode* s2)
	{
		if (s1 == nullptr || s2 == nullptr)
			return false;
		if ((&*m_MappedData)[s1->m_GetID()].m_fCost() < (&*m_MappedData)[s2->m_GetID()].m_fCost())
			return true;
		return false;
	}
};*/

void CalculateCoordsAndNeighbours(HexNode* ThisNode, HexNode* StartNode, vector<HexNode*>& EvaluatedSet)
{
	EvaluatedSet.push_back(ThisNode);
	//x + y + z = 0
	ThisNode->m_SetCubicalX(ThisNode->m_GetX() - StartNode->m_GetX());
	ThisNode->m_SetCubicalY(ThisNode->m_GetY() - StartNode->m_GetY());
	ThisNode->m_SetCubicalZ((-1*ThisNode->m_GetCubicalX()) - (ThisNode->m_GetCubicalY()));

	for (int i = 0; i < ThisNode->m_Neighbours.size(); ++i)
	{
		if(std::find(EvaluatedSet.begin(),EvaluatedSet.end(),ThisNode->m_Neighbours[i]) == EvaluatedSet.end())
			CalculateCoordsAndNeighbours(ThisNode->m_Neighbours[i], StartNode,EvaluatedSet);
	}
}

//Creërt een grid. Called HexGrid::CreateGrid()
HexGrid::HexGrid(unsigned int size) : m_Size(size)
{
	if (m_Size <= 0 || m_Size > 25 || m_Size % 2 == 0)
	{
		MessageBox(NULL, L"Ongeldige grootte gespecifiseerd!", L"Fout!", MB_ICONEXCLAMATION | MB_OK);
		throw std::invalid_argument("Ongeldige grid grootte"); //Lekker een exception gooien :)
	}
	CreateGrid();
	CalculateCubicalCoordinates();
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
	int IDKeeper = 0;
	for (unsigned iRow = 0; iRow < m_Size; ++iRow)
	{
		vector<HexNode> row;
		for (unsigned iColumn = 0; iColumn < m_Size; ++iColumn)
		{
			row.push_back(HexNode(iRow,iColumn,(*this), IDKeeper));
			IDKeeper++;
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

void HexGrid::CalculateCubicalCoordinates()
{
	vector<HexNode*> EvaluatedSet;
	int MiddleX = m_Size / 2;
	int MiddleY = m_Size / 2;

	HexNode* MiddleNode = &(*this)(MiddleX, MiddleY);
	MiddleNode->m_SetCubicalCoords(0, 0, 0);
	CalculateCoordsAndNeighbours(MiddleNode, MiddleNode, EvaluatedSet);
}


auto HexGrid::FindPath(int StartNodeX, int StartNodeY, int EndNodeX, int EndNodeY) -> vector<HexNode*>
{
	
	HexNode* StartNode = &m_Grid[StartNodeX][StartNodeY];
	HexNode* EndNode = &m_Grid[EndNodeX][EndNodeY];

	StartNodeX = StartNode->m_GetCubicalX();
	StartNodeY = StartNode->m_GetCubicalY();
	int StartNodeZ = StartNode->m_GetCubicalZ();

	EndNodeX = EndNode->m_GetCubicalX();
	EndNodeY = EndNode->m_GetCubicalY();
	int EndNodeZ = EndNode->m_GetCubicalZ();


	vector<HexNode*> OpenSet;
	unordered_set<HexNode*> ClosedSet; //unordered_set blijkt uit tests consistenter lagere timings te geven dan andere STL containers die ik getest heb, oa wanneer de grootte toeneemt
	unique_ptr<NodeAstarData> mappedData(new NodeAstarData[(StartNode)->m_GetHexGrid()->get_Size() * (StartNode)->m_GetHexGrid()->get_Size()]); 	//Ik sla de extra data op in een array. Ik had eerst een map met een key value pair, maar alle hexes naar een array mappen bleek twee maal zo snel. Dit zorgt voor kleine een vertraging, echter hierdoor kan ik wel meerdere paden tegelijk doen. (Multithreaden)

	OpenSet.push_back(StartNode);
	(&*mappedData)[StartNode->m_GetID()].m_isInOpenSet = true;

	NodeAstarData start;
	(&*mappedData)[StartNode->m_GetID()] = start;
	(&*mappedData)[EndNode->m_GetID()] = start;

	while (OpenSet.size() > 0)
	{
		HexNode* CurrentLocation = OpenSet.front();
		int IndexStorer = 0;
		for (int i = 0; i < OpenSet.size(); ++i)
		{
			if ((&*mappedData)[OpenSet[i]->m_GetID()].m_fCost() < (&*mappedData)[CurrentLocation->m_GetID()].m_fCost() || (&*mappedData)[OpenSet[i]->m_GetID()].m_fCost() == (&*mappedData)[CurrentLocation->m_GetID()].m_fCost() && (&*mappedData)[OpenSet[i]->m_GetID()].m_hCost < (&*mappedData)[CurrentLocation->m_GetID()].m_hCost)
			{
				CurrentLocation = OpenSet[i];
				IndexStorer = i;
			}
		}
		OpenSet.erase(OpenSet.begin() + IndexStorer);
		ClosedSet.insert(CurrentLocation);
		(&*mappedData)[CurrentLocation->m_GetID()].m_isClosed = true;

		if (CurrentLocation == EndNode)
		{
			//Traceer path
			return RetracePath(StartNode, EndNode, mappedData);
		}

		for (auto it = CurrentLocation->m_Neighbours.begin(); it != CurrentLocation->m_Neighbours.end(); it++)
		{
			HexNode* NghBor = *it;
			if (NghBor->m_GetState() != StartNode->m_GetState() && NghBor->m_GetState() != State::NONE || (&*mappedData)[NghBor->m_GetID()].m_isClosed)
			{
				continue;
			}

			int newMoveCostToNgh = (&*mappedData)[NghBor->m_GetID()].m_gCost + GetDistance(CurrentLocation->m_GetCubicalX(), CurrentLocation->m_GetCubicalY(),CurrentLocation->m_GetCubicalZ(), NghBor->m_GetCubicalX(), NghBor->m_GetCubicalY(), NghBor->m_GetCubicalZ());
			bool isInOpenSet = (&*mappedData)[NghBor->m_GetID()].m_isInOpenSet;
			
			if (!(&*mappedData)[NghBor->m_GetID()].m_isInOpenSet)
			{
				NodeAstarData nData;
				nData.m_gCost = newMoveCostToNgh;
				nData.m_hCost = GetDistance(NghBor->m_GetCubicalX(), NghBor->m_GetCubicalY(),NghBor->m_GetCubicalZ(), EndNodeX, EndNodeY,EndNodeZ);
				nData.m_Parent = CurrentLocation;
				OpenSet.push_back(NghBor);
				(&*mappedData)[NghBor->m_GetID()] = nData;
				(&*mappedData)[NghBor->m_GetID()].m_isInOpenSet = true;

			}
			else if (newMoveCostToNgh >= (&*mappedData)[NghBor->m_GetID()].m_gCost)
				continue;
			else
			{
				(&*mappedData)[NghBor->m_GetID()].m_Parent = CurrentLocation;
				(&*mappedData)[NghBor->m_GetID()].m_gCost = newMoveCostToNgh;
				(&*mappedData)[NghBor->m_GetID()].m_hCost = GetDistance(NghBor->m_GetCubicalX(),NghBor->m_GetCubicalY(),NghBor->m_GetCubicalZ(),EndNodeX,EndNodeY,EndNodeZ);
			}
			
		}
	}
	return vector<HexNode*>();
}

auto  HexGrid::RetracePath(HexNode * Start, HexNode * End, unique_ptr<NodeAstarData>& nData) -> vector<HexNode*>
{
	vector<HexNode*> path;
	HexNode* CurrentObserved = End;
	while (CurrentObserved != Start)
	{
		path.push_back(CurrentObserved);
		CurrentObserved = (&*nData)[CurrentObserved->m_GetID()].m_Parent;
	}
	path.push_back(Start);
	std::reverse(path.begin(), path.end());
	return path;
}

/* Broken Heap
auto HexGrid::FindPath(int StartNodeX, int StartNodeY, int EndNodeX, int EndNodeY) -> vector<HexNode*>
{
	HexNode* StartNode = &m_Grid[StartNodeX][StartNodeY];
	HexNode* EndNode = &m_Grid[EndNodeX][EndNodeY];

	int Size = (StartNode)->m_GetHexGrid()->get_Size();

	unique_ptr<NodeAstarData> mappedData(new NodeAstarData[Size * Size]); 	//Ik sla de extra data op in een array. Ik had eerst een map met een key value pair, maar alle hexes naar een array mappen bleek twee maal zo snel. Dit zorgt voor kleine een vertraging, echter hierdoor kan ik wel meerdere paden tegelijk doen. (Multithreaden)
	deque<HexNode*> TSet(Size * Size); //Moet nog een heap worden
	MinHeap OpenSet(TSet,mappedData);
	unordered_set<HexNode*> ClosedSet(Size*Size); //unordered_set blijkt uit tests consistenter lagere timings te geven dan andere STL containers die ik getest heb, oa wanneer de grootte toeneemt
	
	OpenSet.Insert(StartNode);
	NodeAstarData start;
	(&*mappedData)[StartNode->m_GetID()] = start;
	(&*mappedData)[EndNode->m_GetID()] = start;

	while (OpenSet.GetMinFCostNode() != nullptr)
	{
		HexNode* CurrentLocation = OpenSet.GetMinFCostNode();

		OpenSet.DeleteMinFCostNode();
		ClosedSet.insert(CurrentLocation);

		if (CurrentLocation == EndNode)
		{
			//Traceer path
			return RetracePath(StartNode, EndNode, mappedData);
		}
		for (auto it = CurrentLocation->m_Neighbours.begin(); it != CurrentLocation->m_Neighbours.end(); it++)
		{
			HexNode* NghBor = *it;
			if (NghBor->m_GetState() != StartNode->m_GetState() && NghBor->m_GetState() != State::NONE || std::find(ClosedSet.begin(), ClosedSet.end(), NghBor) != ClosedSet.end())
			{
				continue;
			}
			int newMoveCostToNgh = (&*mappedData)[NghBor->m_GetID()].m_gCost + GetDistance(CurrentLocation->m_GetX(), CurrentLocation->m_GetY(), NghBor->m_GetX(), NghBor->m_GetY());
			if (newMoveCostToNgh < (&*mappedData)[NghBor->m_GetID()].m_gCost || std::find(OpenSet.begin(), OpenSet.end(), NghBor) == OpenSet.end())
			{
				NodeAstarData nData;
				nData.m_gCost = newMoveCostToNgh;
				nData.m_hCost = GetDistance(NghBor->m_GetX(), NghBor->m_GetY(), EndNode->m_GetX(), EndNode->m_GetY());
				nData.m_Parent = CurrentLocation;

				if (std::find(OpenSet.begin(), OpenSet.end(), NghBor) == OpenSet.end())
				{
					OpenSet.Insert(NghBor);
					(&*mappedData)[NghBor->m_GetID()] = nData;
				}
				else
				{
					int index = std::find(OpenSet.begin(), OpenSet.end(), NghBor) - OpenSet.begin();
					if (index < OpenSet.size())
					{
						OpenSet.SortNodeUp(index);
					}
				}
			}
		}
	}
	return vector<HexNode*>();
}

auto  HexGrid::RetracePath(HexNode * Start, HexNode * End, unique_ptr<NodeAstarData>& nData) -> vector<HexNode*>
{
	vector<HexNode*> path;
	HexNode* CurrentObserved = End;
	while (CurrentObserved != Start)
	{
		path.push_back(CurrentObserved);
		CurrentObserved = (&*nData)[CurrentObserved->m_GetID()].m_Parent;
	}
	path.push_back(Start);
	std::reverse(path.begin(), path.end());
	return path;
}

/* //Broken Superfast implementation
auto HexGrid::FindPath(int StartNodeX, int StartNodeY, int EndNodeX, int EndNodeY) -> vector<HexNode*>
{
	HexNode* StartNode = &m_Grid[StartNodeX][StartNodeY];
	HexNode* EndNode = &m_Grid[EndNodeX][EndNodeY];

	deque<HexNode*> OpenSet; //Moet nog een heap worden
	unordered_set<HexNode*> ClosedSet; //unordered_set blijkt uit tests consistenter lagere timings te geven dan andere STL containers die ik getest heb, oa wanneer de grootte toeneemt
	unique_ptr<NodeAstarData> mappedData(new NodeAstarData[(StartNode)->m_GetHexGrid()->get_Size() * (StartNode)->m_GetHexGrid()->get_Size()]); 	//Ik sla de extra data op in een array. Ik had eerst een map met een key value pair, maar alle hexes naar een array mappen bleek twee maal zo snel. Dit zorgt voor kleine een vertraging, echter hierdoor kan ik wel meerdere paden tegelijk doen. (Multithreaden)
	
	OpenSet.push_back(StartNode);
	(&*mappedData)[StartNode->m_GetID()].m_isInOpenSet = true;

	NodeAstarData start;
	(&*mappedData)[StartNode->m_GetID()] = start;
	(&*mappedData)[EndNode->m_GetID()] = start;

	while (OpenSet.size() > 0)
	{
		HexNode* CurrentLocation = OpenSet.front();
		int IndexStorer = 0;
		for (int i = 0; i < OpenSet.size(); ++i)
		{
			if ((&*mappedData)[OpenSet[i]->m_GetID()].m_fCost() < (&*mappedData)[CurrentLocation->m_GetID()].m_fCost() || (&*mappedData)[OpenSet[i]->m_GetID()].m_fCost() == (&*mappedData)[CurrentLocation->m_GetID()].m_fCost() && (&*mappedData)[OpenSet[i]->m_GetID()].m_hCost < (&*mappedData)[CurrentLocation->m_GetID()].m_hCost)
			{
				CurrentLocation = OpenSet[i];
				IndexStorer = i;
			}
		}
		OpenSet.erase(OpenSet.begin() + IndexStorer);
		ClosedSet.insert(CurrentLocation);
		(&*mappedData)[CurrentLocation->m_GetID()].m_isClosed = true;

		if (CurrentLocation == EndNode)
		{
			//Traceer path
			return RetracePath(StartNode, EndNode, mappedData);
		}
		for (auto it = CurrentLocation->m_Neighbours.begin(); it != CurrentLocation->m_Neighbours.end(); it++)
		{
			HexNode* NghBor = *it;
			if (NghBor->m_GetState() != StartNode->m_GetState() && NghBor->m_GetState() != State::NONE || (&*mappedData)[NghBor->m_GetID()].m_isClosed)
			{
				continue;
			}
			int newMoveCostToNgh = (&*mappedData)[NghBor->m_GetID()].m_gCost + GetDistance(CurrentLocation->m_GetX(), CurrentLocation->m_GetY(), NghBor->m_GetX(), NghBor->m_GetY());
			bool isInOpenSet = (&*mappedData)[NghBor->m_GetID()].m_isInOpenSet;
			if (newMoveCostToNgh < (&*mappedData)[NghBor->m_GetID()].m_gCost || !isInOpenSet)
			{
				NodeAstarData nData;
				nData.m_gCost = newMoveCostToNgh;
				nData.m_hCost = GetDistance(NghBor->m_GetX(), NghBor->m_GetY(), EndNode->m_GetX(), EndNode->m_GetY());
				nData.m_Parent = CurrentLocation;

				if (!isInOpenSet)
				{
					OpenSet.push_back(NghBor);
					(&*mappedData)[NghBor->m_GetID()] = nData;
					(&*mappedData)[NghBor->m_GetID()].m_isInOpenSet = true;
				}
			}
		}
	}
	return vector<HexNode*>();
}

auto  HexGrid::RetracePath(HexNode * Start, HexNode * End, unique_ptr<NodeAstarData>& nData) -> vector<HexNode*>
{
	vector<HexNode*> path;
	HexNode* CurrentObserved = End;
	while (CurrentObserved != Start)
	{
		path.push_back(CurrentObserved);
		CurrentObserved = (&*nData)[CurrentObserved->m_GetID()].m_Parent;
	}
	path.push_back(Start);
	std::reverse(path.begin(), path.end());
	return path;
}*/

/* Oude map implementatie, ongeveer 2x zo sloom als dan met de array
auto HexGrid::FindPath(int StartNodeX, int StartNodeY, int EndNodeX, int EndNodeY) -> vector<HexNode*>
{
	HexNode* StartNode = &m_Grid[StartNodeX][StartNodeY];
	HexNode* EndNode = &m_Grid[EndNodeX][EndNodeY];

	deque<HexNode*> OpenSet; //Moet nog een heap worden
	unordered_map<HexNode*, NodeAstarData> mappedData; //Ik sla de extra data op in een map. Dit zorgt voor een vertraging, echter hierdoor kan ik wel meerdere paden tegelijk doen. (Multithreaden)
	unordered_set<HexNode*> ClosedSet; //unordered_set blijkt uit tests consistenter lagere timings te geven dan andere STL containers die ik getest heb, oa wanneer de grootte toeneemt
	
	OpenSet.push_back(StartNode);
	NodeAstarData start;
	mappedData[StartNode] = start;
	mappedData[EndNode] = start;

	while (OpenSet.size() > 0)
	{
		HexNode* CurrentLocation = OpenSet.front();
		int IndexStorer = 0;
		for (int i = 0; i < OpenSet.size(); ++i)
		{
			if (mappedData[OpenSet[i]].m_fCost() < mappedData[CurrentLocation].m_fCost() || mappedData[OpenSet[i]].m_fCost() == mappedData[CurrentLocation].m_fCost() && mappedData[OpenSet[i]].m_hCost < mappedData[CurrentLocation].m_hCost)
			{
				CurrentLocation = OpenSet[i];
				IndexStorer = i;
			}
		}
		OpenSet.erase(OpenSet.begin() + IndexStorer);
		ClosedSet.insert(CurrentLocation);

		if (CurrentLocation == EndNode)
		{
			//Retrace path
			return RetracePath(StartNode, EndNode, mappedData);
		}
		for (auto it = CurrentLocation->m_Neighbours.begin(); it != CurrentLocation->m_Neighbours.end(); it++)
		{
			HexNode* NghBor = *it;
			if (NghBor->m_GetState() != StartNode->m_GetState() && NghBor->m_GetState() != State::NONE || std::find(ClosedSet.begin(), ClosedSet.end(), NghBor) != ClosedSet.end())
			{
				continue;
			}
			int newMoveCostToNgh = mappedData[NghBor].m_gCost + GetDistance(CurrentLocation->m_GetX(), CurrentLocation->m_GetY(), NghBor->m_GetX(), NghBor->m_GetY());
			if (newMoveCostToNgh < mappedData[NghBor].m_gCost || std::find(OpenSet.begin(), OpenSet.end(), NghBor) == OpenSet.end())
			{
				NodeAstarData nData;
				nData.m_gCost = newMoveCostToNgh;
				nData.m_hCost = GetDistance(NghBor->m_GetX(), NghBor->m_GetY(), EndNode->m_GetX(), EndNode->m_GetY());
				nData.m_Parent = CurrentLocation;

				if (std::find(OpenSet.begin(), OpenSet.end(), NghBor) == OpenSet.end())
				{
					OpenSet.push_back(NghBor);
					mappedData[NghBor] = nData;
				}
			}
		}
	}
	return vector<HexNode*>();
}

auto  HexGrid::RetracePath(HexNode * Start, HexNode * End, unordered_map<HexNode*,NodeAstarData>& nData) -> vector<HexNode*>
{
	vector<HexNode*> path;
	HexNode* CurrentObserved = End;
	while (CurrentObserved != Start)
	{
		path.push_back(CurrentObserved);
		CurrentObserved = nData[CurrentObserved].m_Parent;
	}
	path.push_back(Start);
	std::reverse(path.begin(), path.end());
	return path;
}*/
/* SingleThreaded variant?
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
*/