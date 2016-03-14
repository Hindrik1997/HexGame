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

	if (MessageBox(NULL, L"Do you want to play with red?", L"Color choice:", MB_YESNO) == IDYES) 
	{
		HumanPlayer = State::RED;
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
	for (int i = 0; i < static_cast<int>(get_Size()); ++i)
	{
		TopNode->AddConnection(&m_Grid[i][0]);
		m_Grid[i][0].AddConnection(TopNode);
	}
	CalculateCoordsSpecialNodes(TopNode,MiddleNode);
	
	BottomNode = new HexNode((get_Size() - 1) / 2, get_Size(), *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < static_cast<int>(get_Size()); ++i)
	{
		BottomNode->AddConnection(&m_Grid[i][get_Size()-1]);
		m_Grid[i][get_Size() - 1].AddConnection(BottomNode);
	}
	CalculateCoordsSpecialNodes(BottomNode, MiddleNode);
	
	LeftNode = new HexNode(-1, (get_Size() / 2), *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < static_cast<int>(get_Size()); ++i)
	{
		LeftNode->AddConnection(&m_Grid[0][i]);
		m_Grid[0][i].AddConnection(LeftNode);
	}
	CalculateCoordsSpecialNodes(LeftNode, MiddleNode);

	RightNode = new HexNode(get_Size(), (get_Size() / 2), *this, IDKeeper);
	IDKeeper++;
	for (int i = 0; i < static_cast<int>(get_Size()); ++i)
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

auto HexGrid::RetracePath(HexNode * Start, HexNode * End, unique_ptr<NodeAstarData>& nData) -> vector<HexNode*>
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

auto HexGrid::ComputeBestMove() -> std::tuple<Move, vector<HexNode*>,bool>
{
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
				NewLength = static_cast<int>(NewPotentialPath.size());
				NodeIndex = i;
				break;
			}

			if ((int)NewPotentialPath.size() > NewLength)
			{				
				NewLength = static_cast<int>(NewPotentialPath.size());
				NodeIndex = i;
			}
			BestPotentialPath[i]->m_SetState(PrevState);
		}

		Move opponLast;
		if (PlayedMoves.size() != 0)
		{
			//Get last played move by human player
			opponLast = PlayedMoves[PlayedMoves.size() - 1];

			if (BestPotentialPath[NodeIndex]->m_GetX() == opponLast.x && BestPotentialPath[NodeIndex]->m_GetY() == opponLast.y)
				return  std::make_tuple(Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState },BestPotentialPath,true);
		}
		else
		{
			return  std::make_tuple(Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState }, BestPotentialPath,true);
		}

		Move CurrentBestMove = Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState };

		//Consider what happens when we play this one, based on the last played node by the human

		bool IsConnectedToFirst = FindPath(&m_Grid[opponLast.x][opponLast.y], FirstNode).size() == 0 ? false : true;
		bool IsConnectedToSecond = FindPath(&m_Grid[opponLast.x][opponLast.y], SecondNode).size() == 0 ? false : true;
		if (IsConnectedToFirst && !IsConnectedToSecond)
		{
			vector<HexNode*> SecondPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], SecondNode);
			//Connected aan first, niet aan second
			if (SecondPath.size() <= NewLength)
			{
				//Play this one instead
				int SecondNewLength = -1;
				int SecondNodeIndex = -1;

				for (int i = 0; i < SecondPath.size(); i++)
				{
					if (SecondPath[i]->m_GetState() != State::NONE)
						continue;
					//Remember state
					State PrevState = SecondPath[i]->m_GetState();
					SecondPath[i]->m_SetState(OppositeState);

					vector<HexNode*> NewPotentialPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], SecondNode);
					if ((int)NewPotentialPath.size() == 0)
					{
						SecondPath[i]->m_SetState(PrevState);
						SecondNewLength = static_cast<int>(NewPotentialPath.size());
						SecondNodeIndex = i;
						break;
					}

					if ((int)NewPotentialPath.size() > SecondNewLength)
					{
						SecondNewLength = static_cast<int>(NewPotentialPath.size());
						SecondNodeIndex = i;
					}
					SecondPath[i]->m_SetState(PrevState);
				}
				return std::make_tuple(Move{ SecondPath[SecondNodeIndex]->m_GetX(),SecondPath[SecondNodeIndex]->m_GetY(), OppositeState }, SecondPath,false);
			}
			else
			{
				return  std::make_tuple(Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState }, BestPotentialPath,true);
			}
		}
		else
		{
			if (!IsConnectedToFirst && IsConnectedToSecond)
			{
				vector<HexNode*> SecondPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], FirstNode);
				//Connected aan first, niet aan second
				if (SecondPath.size() <= NewLength)
				{
					//Play this one instead
					int SecondNewLength = -1;
					int SecondNodeIndex = -1;

					for (int i = 0; i < SecondPath.size(); i++)
					{
						if (SecondPath[i]->m_GetState() != State::NONE)
							continue;
						//Remember state
						State PrevState = SecondPath[i]->m_GetState();
						SecondPath[i]->m_SetState(OppositeState);

						vector<HexNode*> NewPotentialPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], FirstNode);
						if ((int)NewPotentialPath.size() == 0)
						{
							SecondPath[i]->m_SetState(PrevState);
							SecondNewLength = static_cast<int>(NewPotentialPath.size());
							SecondNodeIndex = i;
							break;
						}

						if ((int)NewPotentialPath.size() > SecondNewLength)
						{
							SecondNewLength = static_cast<int>(NewPotentialPath.size());
							SecondNodeIndex = i;
						}
						SecondPath[i]->m_SetState(PrevState);
					}
					return std::make_tuple(Move{ SecondPath[SecondNodeIndex]->m_GetX(),SecondPath[SecondNodeIndex]->m_GetY(), OppositeState },SecondPath,false);
				}
				else
				{
					return std::make_tuple(Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState }, BestPotentialPath,true);
				}
			}
			else
			{

				vector<HexNode*> TFirstPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], FirstNode);
				vector<HexNode*> TSecondPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], SecondNode);

				if (TFirstPath.size() > TSecondPath.size())
				{
					//Search for second node
					if (TSecondPath.size() <= NewLength)
					{
						//Play this one instead
						int SecondNewLength = -1;
						int SecondNodeIndex = -1;

						for (int i = 0; i < TSecondPath.size(); i++)
						{
							if (TSecondPath[i]->m_GetState() != State::NONE)
								continue;
							//Remember state
							State PrevState = TSecondPath[i]->m_GetState();
							TSecondPath[i]->m_SetState(OppositeState);

							vector<HexNode*> NewPotentialPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], SecondNode);
							if ((int)NewPotentialPath.size() == 0)
							{
								TSecondPath[i]->m_SetState(PrevState);
								SecondNewLength = static_cast<int>(NewPotentialPath.size());
								SecondNodeIndex = i;
								break;
							}

							if ((int)NewPotentialPath.size() > SecondNewLength)
							{
								SecondNewLength = static_cast<int>(NewPotentialPath.size());
								SecondNodeIndex = i;
							}
							TSecondPath[i]->m_SetState(PrevState);
						}
						return std::make_tuple(Move{ TSecondPath[SecondNodeIndex]->m_GetX(),TSecondPath[SecondNodeIndex]->m_GetY(), OppositeState }, TSecondPath,false);
					}
					else
					{
						return  std::make_tuple(Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState }, BestPotentialPath,true);
					}
				}
				else
				{
					if (TFirstPath.size() <= NewLength)
					{
						//Play this one instead
						int SecondNewLength = -1;
						int SecondNodeIndex = -1;

						for (int i = 0; i < TFirstPath.size(); i++)
						{
							if (TFirstPath[i]->m_GetState() != State::NONE)
								continue;
							//Remember state
							State PrevState = TFirstPath[i]->m_GetState();
							TFirstPath[i]->m_SetState(OppositeState);

							vector<HexNode*> NewPotentialPath = FindBestPotentialNonWeightedPath(&m_Grid[opponLast.x][opponLast.y], FirstNode);
							if ((int)NewPotentialPath.size() == 0)
							{
								TFirstPath[i]->m_SetState(PrevState);
								SecondNewLength = static_cast<int>(NewPotentialPath.size());
								SecondNodeIndex = i;
								break;
							}

							if ((int)NewPotentialPath.size() > SecondNewLength)
							{
								SecondNewLength = static_cast<int>(NewPotentialPath.size());
								SecondNodeIndex = i;
							}
							TFirstPath[i]->m_SetState(PrevState);
						}
						return std::make_tuple(Move{ TFirstPath[SecondNodeIndex]->m_GetX(),TFirstPath[SecondNodeIndex]->m_GetY(), OppositeState },TFirstPath,false);
					}
					else
					{
						return  std::make_tuple(Move{ BestPotentialPath[NodeIndex]->m_GetX(),BestPotentialPath[NodeIndex]->m_GetY(), OppositeState }, BestPotentialPath,true);
					}
				}
			}
		}
}

auto HexGrid::EvaluateComputedMove(std::tuple<Move, vector<HexNode*>, bool> moveData) -> Move
{
	
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
	
	if (PlayedMoves.size() < 3)
		return std::get<0>(moveData);

	Move PrevMove = PlayedMoves[PlayedMoves.size() - 1];
	Move PrePrevMove = PlayedMoves[PlayedMoves.size() - 3];

	if (m_Grid[PrevMove.x][PrevMove.y].m_GetCubicalZ() == m_Grid[PrePrevMove.x][PrePrevMove.y].m_GetCubicalZ())
		goto EvaluateZAxis;

	//check if connected so a single side
	//if so, simply block the opposing side instead. 
	
	bool IsConnectedToFirstSide = false;
	bool IsConnectedToSecondSide = false;

	for (int x = 0; x < (int)m_Size; ++x)
	{
		for (int y = 0; y < (int)m_Size; ++y)
		{
			if (m_Grid[x][y].m_GetState() == HumanPlayer)
			{
				bool MustBreak = false;
				if (FindPath(&m_Grid[x][y], FirstNode).size() != 0)
				{
					//We found a path, so connected to first
					IsConnectedToFirstSide = true;
				}
				if (FindPath(&m_Grid[x][y], SecondNode).size() != 0)
				{
					//We found a path to the second node too
					IsConnectedToSecondSide = true;
				}
			}
		}
	}
	
	Move M = std::get<0>(moveData);
	HexNode* SuggestedNode = &m_Grid[M.x][M.y];

	if (IsConnectedToFirstSide && !IsConnectedToSecondSide)
	{
		//Connected to first side only
		if (HumanPlayer == State::BLUE)
		{
			float Distance = 999.0f;
			HexNode* ClosestNode = nullptr;
			for (int i = 0; i < (int)m_Size; ++i)
			{
				HexNode* CurrentlyObserving = &m_Grid[(int)m_Size-1][i];

				if (GetRealDistance(CurrentlyObserving, SuggestedNode) < Distance && CurrentlyObserving->m_GetState() == State::NONE)
				{
					ClosestNode = CurrentlyObserving;
					Distance = GetRealDistance(CurrentlyObserving,SuggestedNode);
				}

			}
			return Move{ ClosestNode->m_GetX(), ClosestNode->m_GetY(), OppositeState};
		}
		else
		{
			//For red this is the top
			float Distance = 999.0f;
			HexNode* ClosestNode = nullptr;
			for (int i = 0; i < (int)m_Size; ++i)
			{
				HexNode* CurrentlyObserving = &m_Grid[i][0];

				if (GetRealDistance(CurrentlyObserving, SuggestedNode) < Distance && CurrentlyObserving->m_GetState() == State::NONE)
				{
					ClosestNode = CurrentlyObserving;
					Distance = GetRealDistance(CurrentlyObserving, SuggestedNode);
				}

			}
			return Move{ ClosestNode->m_GetX(), ClosestNode->m_GetY(), OppositeState };
		}
	}
	if (IsConnectedToSecondSide && !IsConnectedToSecondSide)
	{
		//Connected to second side only
		if (HumanPlayer == State::BLUE)
		{
			//This is the right side
			float Distance = 999.0f;
			HexNode* ClosestNode = nullptr;
			for (int i = 0; i < (int)m_Size; ++i)
			{
				HexNode* CurrentlyObserving = &m_Grid[0][i];

				if (GetRealDistance(CurrentlyObserving, SuggestedNode) < Distance && CurrentlyObserving->m_GetState() == State::NONE)
				{
					ClosestNode = CurrentlyObserving;
					Distance = GetRealDistance(CurrentlyObserving, SuggestedNode);
				}

			}
			return Move{ ClosestNode->m_GetX(), ClosestNode->m_GetY(), OppositeState };
		}
		else
		{
			//And this the bottom side
			float Distance = 999.0f;
			HexNode* ClosestNode = nullptr;
			for (int i = 0; i < (int)m_Size; ++i)
			{
				HexNode* CurrentlyObserving = &m_Grid[i][0];

				if (GetRealDistance(CurrentlyObserving, SuggestedNode) < Distance && CurrentlyObserving->m_GetState() == State::NONE)
				{
					ClosestNode = CurrentlyObserving;
					Distance = GetRealDistance(CurrentlyObserving, SuggestedNode);
				}

			}
			return Move{ ClosestNode->m_GetX(), ClosestNode->m_GetY(), OppositeState };
		}
	}

	EvaluateZAxis:


	//Only evaluated two axis, so i still have to do the third one

	bool IsOriginal = std::get<2>(moveData);

	if (m_Grid[PrevMove.x][PrevMove.y].m_GetCubicalZ() == m_Grid[PrePrevMove.x][PrePrevMove.y].m_GetCubicalZ() && abs(PrevMove.x - PrePrevMove.x) == 1 && abs(PrevMove.y - PrePrevMove.y) == 1)
	{
		vector<HexNode*> LeftBottom;
		vector<HexNode*> RightTop;
		HexNode* LowestNode = PrePrevMove.x < PrevMove.x ? &m_Grid[PrePrevMove.x][PrePrevMove.y] : &m_Grid[PrevMove.x][PrevMove.y];
		HexNode* HighestNode = PrePrevMove.x > PrevMove.x ? &m_Grid[PrePrevMove.x][PrePrevMove.y] : &m_Grid[PrevMove.x][PrevMove.y];
		//Are on same axis, so calculate if straight path on z axis reaches edges, if so it is potentially a VERY strong move
		for (int x = 0; x < static_cast<int>(m_Size); ++x)
		{
			for (int y = 0; y < static_cast<int>(m_Size); ++y)
			{
				if (m_Grid[x][y].m_GetCubicalZ() == LowestNode->m_GetCubicalZ())
				{
					if (x == PrevMove.x && y == PrevMove.y || x == PrePrevMove.x && y == PrePrevMove.y)
						continue;
					//actual x is smaller on left down side of z axis.
					if (LowestNode->m_GetX() > m_Grid[x][y].m_GetX())
					{
						LeftBottom.push_back(&m_Grid[x][y]);
					}
					else
					{
						RightTop.push_back(&m_Grid[x][y]);
					}
				}
			}
		}

		bool RightTClear = true;
		bool LeftBClear = true;
		for (auto& node : LeftBottom) 
		{
			if(node->m_GetState() != State::NONE)
			{
				LeftBClear = false;
				break;
			}
		}
		for (auto& node : RightTop)
		{
			if (node->m_GetState() != State::NONE)
			{
				RightTClear = false;
				break;
			}
		}

		if (HumanPlayer == State::BLUE)
		{
			if (LeftBClear)
			{
				if (FindPath(FirstNode, LowestNode).size() != 0)
				{
					//Connected to lower one, so check if winning by finding top, if there is a node which is neighbour of our top.
					for (auto& node : RightTop)
					{
						if (std::find(SecondNode->m_Neighbours.begin(), SecondNode->m_Neighbours.end(), node) != SecondNode->m_Neighbours.end())
						{
							//this move wins, 
							return Move{ node->m_GetX(),node->m_GetY(), OppositeState };
						}
					}
				}
				else
				{
					//Not connected to lower one
					for (auto& node : LeftBottom)
					{
						if (std::find(FirstNode->m_Neighbours.begin(), FirstNode->m_Neighbours.end(), node) != FirstNode->m_Neighbours.end())
						{
							//this move wins, 
							return Move{ node->m_GetX(),node->m_GetY(), OppositeState };
						}
					}
				}
			}
		}
		else
		{
			if (RightTClear)
			{
				if (FindPath(SecondNode, LowestNode).size() != 0)
				{
					//Connected to lower one, so check if winning by finding top, if there is a node which is neighbour of our top.
					for (auto& node : RightTop)
					{
						if (std::find(FirstNode->m_Neighbours.begin(), FirstNode->m_Neighbours.end(), node) != FirstNode->m_Neighbours.end())
						{
							//this move wins,
							return Move{ node->m_GetX(),node->m_GetY(), OppositeState };
						}
					}
				}
				else
				{
					//Not connected to lower one
					//Connected to lower one, so check if winning by finding top, if there is a node which is neighbour of our top.
					for (auto& node : LeftBottom)
					{
						if (std::find(SecondNode->m_Neighbours.begin(), SecondNode->m_Neighbours.end(), node) != SecondNode->m_Neighbours.end())
						{
							//this move wins,
							return Move{ node->m_GetX(),node->m_GetY(), OppositeState };
						}
					}
				}
			}
		}
	}

	return std::get<0>(moveData);
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
	for (int x = 0; x < static_cast<int>(m_Size); ++x)
	{
		for (int y = 0; y < static_cast<int>(m_Size); ++y)
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

auto HexGrid::GetFilteredPath(vector<HexNode*>& Path, HexNode * StartNode, HexNode * EndNode) -> vector<HexNode*>
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

auto HexGrid::PlayMove(Move move, HWND hwnd) -> void
{
	int length = GetWindowTextLength(ViewList) + 1;
	std::unique_ptr<WCHAR> TextBuffer(new WCHAR[length]);
	GetWindowText(ViewList, &(*TextBuffer), length);
	wstring origText = wstring(&*TextBuffer);

	HDC dc = GetDC(hwnd);
	if (m_Grid[move.x][move.y].m_GetState() != State::NONE)
		return;
	if (g_hexGrid->GetVictorious() != State::NONE)
	{
		wstring t;
		if (g_hexGrid->GetVictorious() == State::RED)
			t = L"Red";
		else
			t = L"Blue";

		MessageBox(NULL, t.c_str(), L"And the winner is:", MB_OK);
		ReleaseDC(hwnd, dc);
		return;
	}
	if (HumanPlayer != State::NONE)
	{
		if (HumanPlayer == State::BLUE)
		{
			PlayedMoves.push_back(Move{ move.x,move.y, State::BLUE });
			m_Grid[move.x][move.y].m_SetState(State::BLUE);
			FillHexBlue(dc, *g_hexGrid, move.x, move.y);
			wstring first = L"Blue placed a node on ";
			wstring second = std::to_wstring(move.x) + L" - " + std::to_wstring(move.y);
			wstring third = L"\r\n";
			wstring txt = first + second + third;
			origText += txt.c_str();
			SetWindowText(ViewList, origText.c_str());
		}
		else
		{
			PlayedMoves.push_back(Move{ move.x,move.y, State::RED });
			m_Grid[move.x][move.y].m_SetState(State::RED);
			FillHexRed(dc, *g_hexGrid, move.x, move.y);
			wstring first = L"Red placed a node on ";
			wstring second = std::to_wstring(move.x) + L" - " + std::to_wstring(move.y);
			wstring third = L"\r\n";
			wstring txt = first + second + third;
			origText += txt.c_str();
			SetWindowText(ViewList, origText.c_str());
		}
		if (g_hexGrid->GetVictorious() != State::NONE)
		{
			wstring t;
			if (g_hexGrid->GetVictorious() == State::RED)
				t = L"Red";
			else
				t = L"Blue";

			MessageBox(NULL, t.c_str(), L"And the winner is:", MB_OK);
			ReleaseDC(hwnd,dc);
			return;
		}
		Move m = g_hexGrid->EvaluateComputedMove(g_hexGrid->ComputeBestMove());
		State OppositeState = g_hexGrid->HumanPlayer == State::RED ? State::BLUE : State::RED;
		(*g_hexGrid)(m.x, m.y).m_SetState(OppositeState);
		if (OppositeState == State::RED)
		{
			wstring first = L"Computer placed a red node on ";
			wstring second = std::to_wstring(m.x) + L" - " + std::to_wstring(m.y);
			wstring third = L"\r\n";
			wstring txt = first + second + third;
			origText += txt.c_str();
		}
		else
		{
			wstring first = L"Computer placed a blue node on ";
			wstring second = std::to_wstring(m.x) + L" - " + std::to_wstring(m.y);
			wstring third = L"\r\n";
			wstring txt = first + second + third;
			origText += txt.c_str();
		}
		PlayedMoves.push_back(Move{ m.x,m.y, OppositeState });
		SetWindowText(ViewList, origText.c_str());
	}
	else
	{
		//MANUAL
	}
	UpdateHexes(dc, *g_hexGrid);
	if (g_hexGrid->GetVictorious() != State::NONE)
	{
		wstring t;
		if (g_hexGrid->GetVictorious() == State::RED)
			t = L"Red";
		else
			t = L"Blue";

		MessageBox(NULL, t.c_str(), L"And the winner is:", MB_OK);
	}
	ReleaseDC(hwnd, dc);
}

void HexGrid::UndoMove()
{
	int length = GetWindowTextLength(ViewList) + 1;
	std::unique_ptr<WCHAR> TextBuffer(new WCHAR[length]);
	GetWindowText(ViewList, &(*TextBuffer), length);
	wstring origText = wstring(&*TextBuffer);

	if (HumanPlayer == State::NONE && PlayedMoves.size() != 0)
	{
		Move m = PlayedMoves[PlayedMoves.size() - 1];
		m_Grid[m.x][m.y].m_SetState(State::NONE);
		PlayedMoves.pop_back();
		wstring first = L"Undid move for ";
		wstring second = m.Color == State::RED ? L"red " : L"blue ";
		wstring third = std::to_wstring(m.x) + L" - " + std::to_wstring(m.y) + L"\r\n";
		wstring txt = first + second + third;
		origText += txt.c_str();
	}
	else
	{
		if (PlayedMoves.size() < 2)
			return;
		Move m = PlayedMoves[PlayedMoves.size() - 1];
		m_Grid[m.x][m.y].m_SetState(State::NONE);
		PlayedMoves.pop_back();
		wstring first = L"Undid move for ";
		wstring second = m.Color == State::RED ? L"red " : L"blue ";
		wstring third = std::to_wstring(m.x) + L" - " + std::to_wstring(m.y) + L"\r\n";
		wstring txt = first + second + third;
		origText += txt.c_str();

		Move m2 = PlayedMoves[PlayedMoves.size() - 1];
		m_Grid[m2.x][m2.y].m_SetState(State::NONE);
		PlayedMoves.pop_back();
		wstring first2 = L"Undid move for ";
		wstring second2 = m2.Color == State::RED ? L"red " : L"blue ";
		wstring third2 = std::to_wstring(m2.x) + L" - " + std::to_wstring(m2.y) + L"\r\n";
		wstring txt2 = first2 + second2 + third2;
		origText += txt2.c_str();
	}
	SetWindowText(ViewList, origText.c_str());
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

			int weight = NghBor->m_GetState() == StartState ? 0 : 1;
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

auto HexGrid::FindBestPotentialNonWeightedPath(HexNode* StartNode, HexNode* EndNode) -> vector<HexNode*>
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

			int Distance = GetDistance(CurrentLocation, NghBor);
			if (CurrentLocation == StartNode || NghBor == EndNode)
			{
				Distance = 0;
			}

			int newMovementCostToNeighbour = (&*mappedData)[CurrentLocation->m_GetID()].m_gCost + Distance;
			if (newMovementCostToNeighbour < (&*mappedData)[NghBor->m_GetID()].m_gCost || (&*mappedData)[NghBor->m_GetID()].m_isInOpenSet == false)
			{

				(&*mappedData)[NghBor->m_GetID()].m_gCost = newMovementCostToNeighbour;
				HexNode* SpecEndNode;
				if (StartState == State::RED)
				{
					SpecEndNode = &m_Grid[CurrentLocation->m_GetX()][m_Size - 1];
				}
				else
				{
					SpecEndNode = &m_Grid[m_Size - 1][CurrentLocation->m_GetY()];
				}
				(&*mappedData)[NghBor->m_GetID()].m_hCost = GetDistance(NghBor, SpecEndNode);

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