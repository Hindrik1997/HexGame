#include "HexNode.h"
#include "HexGrid.h"

HexNode::HexNode(int x, int y, const HexGrid& hRef) : m_X(x), m_Y(y), m_ParentGrid(&hRef)
{
}


HexNode::~HexNode()
{

}

auto HexNode::CalculateConnections() -> void
{
	/*  
	Row, Column

	0,0	0,1	0,2	0,3	0,4	0,5	0,6	0,7	0,8	0,9	0,10

		1,0	1,1	1,2	1,3	1,4	1,5	1,6	1,7	1,8	1,9	1,10

			2,0	2,1	2,2 etc...

	Als er 6 neighbours zijn:
		Row-1 
			column
			column + 1
		Row
			column - 1
			column + 1
		Row + 1
			column - 1
			column
	*/

	int row = m_X;
	int column = m_Y;
	bool IsSpecial = false;

	bool SitsLeft = false;
	bool SitsRight = false;
	bool SitsTop = false;
	bool SitsBottom = false;


	if (row == 0)
	{
		//Top van de hexgrid, dus rode zijde, en dus minder neighbours
		IsSpecial = true;
		SitsTop = true;
	}
	if (column == 0)
	{
		//Linkerzijde van de hexgrid, dus blauwe zijde, en dus minder neighbours
		IsSpecial = true;
		SitsLeft = true;
	}
	if (row == m_ParentGrid->get_Size()-1)
	{
		//Onderzijde van de hexgrid, dus rode zijde, en dus minder neighbours
		IsSpecial = true;
		SitsBottom = true;
	}
	if (column == m_ParentGrid->get_Size()-1)
	{
		//Rechterzijde van de hexgrid, dus blauwe zijde, en dus minder neighbours
		IsSpecial = true;
		SitsRight = true;
	}
	if (IsSpecial == false)
	{
		//Gegarandeerd 6 neighbours
		//TopLeft
		m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column)));
		//TopRight
		m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column + 1)));
		//Left
		m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column - 1)));
		//Right
		m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column + 1)));
		//BottomLeft
		m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column - 1)));
		//BottomRight
		m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column)));
		return;
	}
	else
	{
		//Speciale gevallen!
		
		if (SitsTop == true && SitsLeft == false && SitsRight == false)
		{
			//Heeft linker en rechter buur en onderburen
			m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column - 1)));
			m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column + 1)));
			m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column - 1)));
			m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column)));
			return;
		}
		else
		{
			if (SitsTop == true && SitsLeft == true && SitsRight == false)
			{
				//Enkel rechtsonder en rechts
				m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column)));
				m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column + 1)));
			}
			else
			{
				if (SitsTop == true && SitsLeft == false && SitsRight == true)
				{
					//Enkel links onder en rechts onder en links
					m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column - 1)));
					m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column)));
					m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column - 1)));
					return;
				}
				else
				{
					if (SitsRight == true && SitsTop == false && SitsBottom == false)
					{
						//Enkel linksboven, links, linksonder en rechtsonder
						m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column)));
						m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column - 1)));
						m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column - 1)));
						m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column)));
						return;
					}
					else
					{
						if (SitsRight == true && SitsBottom == true && SitsTop == false)
						{
							//Enkel linksboven en links
							m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column)));
							m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column - 1)));
							return;
						}
						else
						{
							if (SitsBottom == true && SitsLeft == false && SitsRight == false)
							{
								//Enkel links, rechts, linksboven en rechtsboven
								m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column)));
								m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column + 1)));
								m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column - 1)));
								m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column + 1)));
								return;
							}
							else
							{
								if (SitsBottom == true && SitsLeft == true && SitsRight == false)
								{
									//Enkel linksboven, rechtsboven en rechts
									m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column)));
									m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column + 1)));
									m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column + 1)));
									return;
								}
								else
								{
									if (SitsTop == false && SitsBottom == false && SitsLeft == true)
									{
										//Enkel linksboven, rechtsboven, rechts en rechtsonder
										m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column)));
										m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row - 1, column + 1)));
										m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row + 1, column)));
										m_Neighbours.push_back(const_cast<HexNode*>(&(*m_ParentGrid)(row, column + 1)));
										return;
									}
									else
									{
										//Weird error occured monsieur!
										MessageBox(NULL,L"Ongeldige HexGrid structuur. De code is wss stuk ofzo.", L"Fout!", MB_OK | MB_ICONEXCLAMATION);
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
