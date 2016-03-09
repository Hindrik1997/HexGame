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

	if (StartNode->m_GetState() != State::NONE && EndNode->m_GetState() != State::NONE && StartNode->m_GetState() != EndNode->m_GetState())
	{
		//No path possible
		return vector<HexNode*>();
	}
	int size = (StartNode)->m_GetHexGrid()->get_Size();
	vector<HexNode*> OpenSet;
	vector<HexNode*> ClosedSet; //vector blijkt uit tests consistenter lagere timings te geven dan andere STL containers die ik getest heb, oa wanneer de grootte toeneemt
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
		for (int i = 0; i < OpenSet.size(); ++i)
		{
			if ((&*mappedData)[OpenSet[i]->m_GetID()].m_fCost() < (&*mappedData)[CurrentLocation->m_GetID()].m_fCost())
			{
				CurrentLocation = OpenSet[i];
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

auto HexGrid::ComputeBestMove() -> Move
{

	/*
	int MinDistanceToBorder = 999;
	std::pair<int, int> TPair;

	vector<HexNode*> ObservedSet;
	vector< vector<HexNode*> > Paths;

	for (int x = 0; x < m_Size; ++x)
	{
		for (int y = 0; y < m_Size; ++y)
		{
			if (m_Grid[x][y].m_GetState() == HumanPlayer && std::find(ObservedSet.begin(), ObservedSet.end(), &m_Grid[x][y]) == ObservedSet.end())
			{
				vector<HexNode*> TempSet;
				GetConnectedNodeSet(&m_Grid[x][y], TempSet, ObservedSet);
				for (auto& elem : TempSet)
				{
					ObservedSet.push_back(elem);
				}
				if (TempSet.size() > 1)
					Paths.push_back(TempSet);
			}
		}
	}

	vector< vector<HexNode*> > TSet;

	for (int i = 0; i < Paths.size(); ++i)
	{
		bool IsConnectedToSides = false;
		for (int j = 0; j < Paths[i].size(); ++j)
		{
			if (Paths[i][j]->m_GetX() == 0 || Paths[i][j]->m_GetY() == 0 || Paths[i][j]->m_GetX() == m_Size - 1 || Paths[i][j]->m_GetY() == m_Size - 1)
			{
				IsConnectedToSides = true;
				break;
			}
		}
		if (IsConnectedToSides)
		{
			TSet.push_back(Paths[i]);
		}
	}
	Paths.clear();
	Paths = TSet;

	State PrevState;
	HexNode* FirstNode;
	HexNode* SecondNode;
	if (HumanPlayer == State::RED)
	{
		FirstNode = TopNode;
		SecondNode = BottomNode;
		PrevState = State::RED;
	}
	else
	{
		FirstNode = LeftNode;
		SecondNode = RightNode;
		PrevState = State::BLUE;
	}

	FirstNode->m_SetState(State::NONE);
	SecondNode->m_SetState(State::NONE);

	for (int x = 0; x < m_Size; ++x)
	{
		for (int y = 0; y < m_Size; ++y)
		{
			if (m_Grid[x][y].m_GetState() == HumanPlayer && OccursInSets(&m_Grid[x][y], Paths) == false)
			{
				int FDistance = FindPath(&m_Grid[x][y], FirstNode).size();
				int SDistance = FindPath(&m_Grid[x][y], SecondNode).size();

				if (FDistance < MinDistanceToBorder)
				{
					TPair.first = x;
					TPair.second = y;
					MinDistanceToBorder = FDistance;
				}
				if (SDistance < MinDistanceToBorder)
				{
					TPair.first = x;
					TPair.second = y;
					MinDistanceToBorder = SDistance;
				}
			}
		}
	}

	FirstNode->m_SetState(PrevState);
	SecondNode->m_SetState(PrevState);

	HexNode* FoundPoint = &(*this)(TPair.first, TPair.second);

	FirstNode->m_SetState(State::NONE);
	SecondNode->m_SetState(State::NONE);

	vector<HexNode*> PathA = FindPath(FoundPoint, FirstNode);
	PathA = GetFilteredPath(PathA, FoundPoint, FirstNode);
	vector<HexNode*> PathB = FindPath(FoundPoint, SecondNode);
	PathB = GetFilteredPath(PathB, FoundPoint, SecondNode);

	FirstNode->m_SetState(PrevState);
	SecondNode->m_SetState(PrevState);

	HexNode* TargetNode = PathA.size() > PathB.size() ? SecondNode : FirstNode;
	vector<HexNode*> Path = PathA.size() > PathB.size() ? PathB : PathA;

	State OppositeState = HumanPlayer == State::RED ? State::BLUE : State::RED;
	
	vector<HexNode*> NodeSet;
	for (int i = 0; i < Path.size(); ++i)
	{
		for (int j = 0; j < Path[i]->m_Neighbours.size(); ++j)
		{
			HexNode* NghBor = Path[i]->m_Neighbours[j];

			if (std::find(NodeSet.begin(), NodeSet.end(), NghBor) == NodeSet.end() && NghBor != LeftNode && NghBor != RightNode && NghBor != TopNode && NghBor != BottomNode && NghBor != FoundPoint && NghBor != TargetNode && NghBor->m_GetState() == State::NONE)
			{
				NodeSet.push_back(NghBor);
			}
		}
	}

	int NewLength = 99999;
	int NodeIndex = -1;

	for (int i = 0; i < NodeSet.size(); ++i)
	{
			//Place node, recalculate path length.
			State OriginalState = NodeSet[i]->m_GetState();
			NodeSet[i]->m_SetState(OppositeState);

			vector<HexNode*> NewPath = FindPath(FoundPoint, TargetNode);
			NewPath = GetFilteredPath(NewPath, FoundPoint, TargetNode);

			if (NewPath.size() < NewLength)
			{
				NewLength = NewPath.size();
				NodeIndex = i;
			}
			NodeSet[i]->m_SetState(OriginalState);
	}
	*/
	/*
	vector<HexNode*> BestPotentialPath;
	State PrevState;
	HexNode* FirstNode;
	HexNode* SecondNode;
	if (HumanPlayer == State::RED)
	{
		FirstNode = TopNode;
		SecondNode = BottomNode;
		PrevState = State::RED;
	}
	else
	{
		FirstNode = LeftNode;
		SecondNode = RightNode;
		PrevState = State::BLUE;
	}
	BestPotentialPath = FindBestPotentialPath(FirstNode,SecondNode);
	BestPotentialPath = GetFilteredPath(BestPotentialPath, FirstNode,SecondNode);
	State OppositeState = HumanPlayer == State::RED ? State::BLUE : State::RED;
	vector<HexNode*> NodeSet = BestPotentialPath;
	
	for (int i = 0; i < BestPotentialPath.size(); ++i)
	{
		for (int j = 0; j < BestPotentialPath[i]->m_Neighbours.size(); ++j)
		{
			HexNode* NghBor = BestPotentialPath[i]->m_Neighbours[j];

			if (std::find(NodeSet.begin(), NodeSet.end(), NghBor) == NodeSet.end() && NghBor != LeftNode && NghBor != RightNode && NghBor != TopNode && NghBor != BottomNode  && NghBor->m_GetState() == State::NONE)
			{
				NodeSet.push_back(NghBor);
			}
		}
	}

	int NewLength = 99999;
	int NodeIndex = -1;

	for (int i = 0; i < NodeSet.size(); ++i)
	{
		if (NodeSet[i]->m_GetState() != State::NONE)
			continue;
		//Place node, recalculate path length.
		State OriginalState = NodeSet[i]->m_GetState();
		NodeSet[i]->m_SetState(OppositeState);

		vector<HexNode*> NewPath = FindPath(FirstNode, SecondNode);
		FirstNode->m_SetState(PrevState);
		SecondNode->m_SetState(PrevState);
		NewPath = GetFilteredPath(NewPath, FirstNode, SecondNode);

		if (NewPath.size() < NewLength)
		{
			NewLength = NewPath.size();
			NodeIndex = i;
		}
		NodeSet[i]->m_SetState(OriginalState);
	}

	HexNode* Final = NodeSet[NodeIndex];

	return Move{ Final->m_GetX(), Final->m_GetY(), Final->m_GetState() };
	*/
	
	HexNode* FirstNode;
	HexNode* SecondNode;
	if (HumanPlayer == State::RED)
	{
		FirstNode = TopNode;
		SecondNode = BottomNode;
	}
	else
	{
		FirstNode = LeftNode;
		SecondNode = RightNode;
	}
	State OppositeState = HumanPlayer == State::RED ? State::BLUE : State::RED;

		vector<HexNode*> BestPotentialPath;
		BestPotentialPath = FindBestPotentialPath(FirstNode, SecondNode);
		PotPath = BestPotentialPath;

		int NewLength = -1;
		int NodeIndex = -1;
		
		
		for (int i = 0; i < BestPotentialPath.size(); i++)
		{
			if (BestPotentialPath[i]->m_GetState() != State::NONE)
				continue;
			//Remember state
			State PrevState = BestPotentialPath[i]->m_GetState();
			BestPotentialPath[i]->m_SetState(OppositeState);

			vector<HexNode*> NewPotentialPath = FindBestPotentialPath(FirstNode,SecondNode);
			if ((int)NewPotentialPath.size() == 0)
			{
				BestPotentialPath[i]->m_SetState(PrevState);
				NewLength = NewPotentialPath.size();
				NodeIndex = i;
				break;
			}

			if ((int)NewPotentialPath.size() > NewLength)
			{
				NewLength = NewPotentialPath.size();
				NodeIndex = i;
			}
			BestPotentialPath[i]->m_SetState(PrevState);
		}

		return Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState };
}

auto HexGrid::GetVictorious() -> State
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

auto HexGrid::GetMoves() -> vector<std::pair<int, int>>
{
	vector<std::pair<int, int>> Moves;
	for (int x = 0; x < m_Size; ++x)
	{
		for (int y = 0; y < m_Size; ++y)
		{
			if ((*this)(x, y).m_GetState() == State::NONE)
			{
				Moves.push_back(std::make_pair(x,y));
			}
		}
	}
	return Moves;
}

auto HexGrid::GetConnectedNodeSet(HexNode* StartNode, vector<HexNode*>& CurrentSet,const vector<HexNode*>& TotalSet) -> void
{
	CurrentSet.push_back(StartNode);
	for (int i = 0; i < StartNode->m_Neighbours.size(); ++i)
	{
		//If not in both sets and equals the startnode's state and is not a special node
		if (StartNode->m_Neighbours[i]->m_GetState() == StartNode->m_GetState() && std::find(CurrentSet.begin(), CurrentSet.end(), StartNode->m_Neighbours[i]) == CurrentSet.end() && std::find(TotalSet.begin(), TotalSet.end(), StartNode->m_Neighbours[i]) == TotalSet.end() && StartNode->m_Neighbours[i] != StartNode->m_GetHexGrid()->LeftNode && StartNode->m_Neighbours[i] != StartNode->m_GetHexGrid()->RightNode && StartNode->m_Neighbours[i] != StartNode->m_GetHexGrid()->BottomNode && StartNode->m_Neighbours[i] != StartNode->m_GetHexGrid()->TopNode)
		{
			GetConnectedNodeSet(StartNode->m_Neighbours[i], CurrentSet, TotalSet);
		}
	}	
}

auto HexGrid::OccursInSets(HexNode* Node, vector<vector<HexNode*>>& Set) -> bool
{
	for (int i = 0; i < Set.size(); ++i)
	{
		for (int j = 0; j < Set[i].size(); ++j)
		{
			if (Node == Set[i][j])
				return true;
		}
	}
	return false;
}

vector<HexNode*> HexGrid::GetFilteredPath(vector<HexNode*>& Path, HexNode * StartNode, HexNode * EndNode)
{
	vector<HexNode*> FPath;
	for (int i = 0; i < Path.size(); ++i)
	{
		if (Path[i] != StartNode && Path[i] != EndNode)
		{
			FPath.push_back(Path[i]);
		}
	}
	return FPath;
}

auto HexGrid::FindBestPotentialPath(HexNode* StartNode, HexNode* EndNode) -> vector<HexNode*>
{
	State StartState = StartNode->m_GetState();
	State OppositeState = StartState == State::RED ? State::BLUE : State::RED;

	if (StartNode->m_GetState() != State::NONE && EndNode->m_GetState() != State::NONE && StartNode->m_GetState() != EndNode->m_GetState())
	{
		//No path possible
		return vector<HexNode*>();
	}
	int size = (StartNode)->m_GetHexGrid()->get_Size();
	vector<HexNode*> OpenSet;
	vector<HexNode*> ClosedSet; //vector blijkt uit tests consistenter lagere timings te geven dan andere STL containers die ik getest heb, oa wanneer de grootte toeneemt
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
		for (int i = 0; i < OpenSet.size(); ++i)
		{
			if ((&*mappedData)[OpenSet[i]->m_GetID()].m_fCost() < (&*mappedData)[CurrentLocation->m_GetID()].m_fCost())
			{
				CurrentLocation = OpenSet[i];
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

			if ( /*  If Not traversable */  (NghBor->m_GetState() == OppositeState) /* OR in Closed Set*/ || (&*mappedData)[NghBor->m_GetID()].m_isInClosedSet)
				continue;

			int weight = NghBor->m_GetState() == StartState ? 0 : 3;
			int Distance = GetDistance(CurrentLocation, NghBor);
			if (CurrentLocation == StartNode || NghBor == EndNode)
			{
				Distance = 0;
			}

			int newMovementCostToNeighbour = (&*mappedData)[CurrentLocation->m_GetID()].m_gCost + Distance + weight;
			if (newMovementCostToNeighbour < (&*mappedData)[NghBor->m_GetID()].m_gCost || (&*mappedData)[NghBor->m_GetID()].m_isInOpenSet == false)
			{	

				(&*mappedData)[NghBor->m_GetID()].m_gCost = newMovementCostToNeighbour;			
				HexNode* SpecEndNode;
				if (StartState == State::RED)
				{
					SpecEndNode = &m_Grid[CurrentLocation->m_GetX()][m_Size-1];
				}
				else
				{
					SpecEndNode = &m_Grid[m_Size-1][CurrentLocation->m_GetY()];
				}
				(&*mappedData)[NghBor->m_GetID()].m_hCost = GetDistance(NghBor,SpecEndNode);
				
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