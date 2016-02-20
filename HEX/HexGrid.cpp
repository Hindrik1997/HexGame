#include "HexGrid.h"
#include "WindowFunctions.h"
#include "StateAndAstar.h"

using std::unique_ptr;

auto CalculateCoordsAndNeighbours(HexNode* ThisNode, HexNode* StartNode, vector<HexNode*>& EvaluatedSet) -> void
{
	EvaluatedSet.push_back(ThisNode);
	//x + y + z = 0
	ThisNode->m_SetCubicalX(ThisNode->m_GetX() - StartNode->m_GetX());
	ThisNode->m_SetCubicalY(ThisNode->m_GetY() - StartNode->m_GetY());
	ThisNode->m_SetCubicalZ((-1*ThisNode->m_GetCubicalX()) - (ThisNode->m_GetCubicalY()));

	for (int i = 0; i < ThisNode->m_Neighbours.size(); ++i)
	{
		if (ThisNode->m_Neighbours[i] != ThisNode->m_GetHexGrid()->TopNode)
		{
			if (std::find(EvaluatedSet.begin(), EvaluatedSet.end(), ThisNode->m_Neighbours[i]) == EvaluatedSet.end())
				CalculateCoordsAndNeighbours(ThisNode->m_Neighbours[i], StartNode, EvaluatedSet);
		}
	}
}

auto CalculateCoordsSpecialNodes(HexNode* ThisNode, HexNode* StartNode) -> void
{
	//x + y + z = 0
	ThisNode->m_SetCubicalX(ThisNode->m_GetX() - StartNode->m_GetX());
	ThisNode->m_SetCubicalY(ThisNode->m_GetY() - StartNode->m_GetY());
	ThisNode->m_SetCubicalZ((-1 * ThisNode->m_GetCubicalX()) - (ThisNode->m_GetCubicalY()));
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
	if (BottomNode)
		delete BottomNode;
	if (TopNode)
		delete TopNode;
	if (LeftNode)
		delete LeftNode;
	if (RightNode)
		delete RightNode;
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

	int MiddleX = m_Size / 2;
	int MiddleY = m_Size / 2;

	HexNode* MiddleNode = &(*this)(MiddleX, MiddleY);


	TopNode = new HexNode((get_Size()-1)/2, -1, *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < get_Size(); ++i)
	{
		TopNode->AddConnection(&m_Grid[i][0]);
		m_Grid[i][0].AddConnection(TopNode);
	}
	CalculateCoordsSpecialNodes(TopNode,MiddleNode);
	
	BottomNode = new HexNode((get_Size() - 1) / 2, get_Size(), *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < get_Size(); ++i)
	{
		BottomNode->AddConnection(&m_Grid[i][get_Size()-1]);
		m_Grid[i][get_Size() - 1].AddConnection(BottomNode);
	}
	CalculateCoordsSpecialNodes(BottomNode, MiddleNode);
	
	LeftNode = new HexNode(-1, (get_Size() / 2), *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < get_Size(); ++i)
	{
		LeftNode->AddConnection(&m_Grid[0][i]);
		m_Grid[0][i].AddConnection(LeftNode);
	}
	CalculateCoordsSpecialNodes(LeftNode, MiddleNode);

	RightNode = new HexNode(get_Size(), (get_Size() / 2), *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < get_Size(); ++i)
	{
		RightNode->AddConnection(&m_Grid[get_Size() - 1][i]);
		m_Grid[get_Size() - 1][i].AddConnection(RightNode);
	}
	CalculateCoordsSpecialNodes(RightNode, MiddleNode);
	LeftNode->m_SetState(State::BLUE);
	RightNode->m_SetState(State::BLUE);
	TopNode->m_SetState(State::RED);
	BottomNode->m_SetState(State::RED);
}

auto HexGrid::CalculateCubicalCoordinates() -> void
{
	vector<HexNode*> EvaluatedSet;
	int MiddleX = m_Size / 2;
	int MiddleY = m_Size / 2;

	HexNode* MiddleNode = &(*this)(MiddleX, MiddleY);
	MiddleNode->m_SetCubicalCoords(0, 0, 0);
	CalculateCoordsAndNeighbours(MiddleNode, MiddleNode, EvaluatedSet);
}


auto HexGrid::FindPath(HexNode* StartNode, HexNode* EndNode) -> vector<HexNode*>
{
	//HexNode* StartNode = &m_Grid[StartNodeX][StartNodeY];
	//HexNode* EndNode = &m_Grid[EndNodeX][EndNodeY];

	if (StartNode->m_GetState() != State::NONE && EndNode->m_GetState() != State::NONE && StartNode->m_GetState() != EndNode->m_GetState())
	{
		//No path possible
		return vector<HexNode*>();
	}
	vector<HexNode*> OpenSet;
	vector<HexNode*> ClosedSet;
	int size = (StartNode)->m_GetHexGrid()->get_Size();
	unique_ptr<NodeAstarData> mappedData(new NodeAstarData[size * size + 4]); 	//Ik sla de extra data op in een array. Ik had eerst een map met een key value pair, maar alle hexes naar een array mappen bleek twee maal zo snel. Dit zorgt voor kleine een vertraging, echter hierdoor kan ik wel meerdere paden tegelijk doen. (Multithreaden)
	
	for (int i = 0; i < 4; ++i)
	{
		(&*mappedData)[size * size + i].isSpecial = true;
	} 

	OpenSet.push_back(StartNode);
	(&*mappedData)[StartNode->m_GetID()].m_isInOpenSet = true;
	NodeAstarData start;
	NodeAstarData end;
	(&*mappedData)[StartNode->m_GetID()] = start;
	(&*mappedData)[EndNode->m_GetID()] = end;
	while (OpenSet.size() > 0)
	{
		HexNode* CurrentLocation = OpenSet.front();
		//Gezien de beperkte grootte van de OpenSet, is het voor mij sneller om de hele set te doorzoeken dan om een binary heap te implementen en steeds te sorteren. Daar waar hij met heap ~44 microseconden neemt worst case, en ~25 microseconden zonder op mijn eigen pc.
		for (auto it = OpenSet.begin(); it != OpenSet.end(); it++)
		{
			if ((&*mappedData)[(*it)->m_GetID()].m_fCost() < (&*mappedData)[CurrentLocation->m_GetID() + 4].m_fCost())
			{
				CurrentLocation = (*it);
			}
		}
		OpenSet.erase(std::remove(OpenSet.begin(), OpenSet.end(), CurrentLocation));
		(&*mappedData)[CurrentLocation->m_GetID()].m_isInOpenSet = false;
		ClosedSet.push_back(CurrentLocation);
		(&*mappedData)[CurrentLocation->m_GetID()].m_isInClosedSet = true;

		if (CurrentLocation == EndNode)
		{
			return RetracePath(StartNode, EndNode, mappedData);
		}

		for (auto it = CurrentLocation->m_Neighbours.begin(); it != CurrentLocation->m_Neighbours.end(); it++)
		{
			HexNode* NghBor = *it;

			if ( /*  If Not traversable */  (NghBor->m_GetState() != State::NONE && NghBor->m_GetState() != CurrentLocation->m_GetState()) /* OR in Closed Set*/ || (&*mappedData)[NghBor->m_GetID()].m_isInClosedSet)
				continue;

			int newMovementCostToNeighbour = (&*mappedData)[CurrentLocation->m_GetID()].m_gCost + GetDistance(CurrentLocation, NghBor);
			if (newMovementCostToNeighbour < (&*mappedData)[NghBor->m_GetID()].m_gCost || (&*mappedData)[NghBor->m_GetID()].m_isInOpenSet == false)
			{
				(&*mappedData)[NghBor->m_GetID()].m_gCost = newMovementCostToNeighbour;
				(&*mappedData)[NghBor->m_GetID()].m_hCost = GetDistance(NghBor, EndNode);
				(&*mappedData)[NghBor->m_GetID()].m_Parent = CurrentLocation;
				if ((&*mappedData)[NghBor->m_GetID()].m_isInOpenSet == false)
				{
					OpenSet.push_back(NghBor);
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
}

auto HexGrid::ComputeBestMove(Move m) -> Move
{
	State IsVictorious = GetVictorious();

	MessageBox(NULL, std::to_wstring((char)IsVictorious).c_str(), L"", MB_OK);





	return Move{ 0,0,0 };
}

State HexGrid::GetVictorious()
{
	vector<HexNode*> BlueSet = FindPath(LeftNode,RightNode);
	vector<HexNode*> RedSet = FindPath(TopNode,BottomNode);
	if (BlueSet.size() == 0 && RedSet.size() == 0)
	{
		return State::NONE;
	}
	else
		if (BlueSet.size() == 0)
			return State::RED;
		else
			return State::BLUE;
}

