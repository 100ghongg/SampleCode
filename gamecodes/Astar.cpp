#include "stdafx.h"
#include "Astar.h"
#include "BitMapMgr.h"
#include "Player.h"

CAstar* CAstar::m_pInst = nullptr;

bool CAstar::CheckOpen(TILE *& ptile)
{
	for (auto& i : m_OpenMap)
	{
		if (ptile == i.second)
			return true;
	}
	return false;
}


POINT CAstar::PathFind(POINT _m)
{
	TILE*	ptemp = nullptr;
	bool	bCheck = false;
	int		iX = _m.x;
	int		iY = _m.y;
	
	POINT ptDest{ (int(m_pPlayer->GetInfo().fX) % ROOMCX - 144) / 72, (int(m_pPlayer->GetInfo().fY) - 216) / 72 };

	POINT ptSour{ iX, iY };

	// 맵 범위 확인
	if (1 > ptDest.x || ptDest.x >= (TILEX - 5) || 0 > ptDest.y || ptDest.y > (TILEY - 5))
		return ptSour;
	// 도착 확인
	if (ptDest.x == iX && ptDest.y == iY)
		return ptDest;

	m_OpenMap.insert(make_pair(1, (*m_pTileVec)[iY][iX]));
	(*m_pTileVec)[iY][iX]->iMoveCost = 0;
	(*m_pTileVec)[iY][iX]->iTotalCost = 0;
	(*m_pTileVec)[iY][iX]->iManCost = 0;
	(*m_pTileVec)[iY][iX]->pSourTile = nullptr;

	while (true)
	{
		multimap<int, TILE*>::iterator iter = m_OpenMap.begin();
		
		for(int i = 0; i < 3; ++i)
		{
			if (iter == m_OpenMap.end())
				break;
			iter++;
		}

		// 기준 사각형 선택
		iX = ((int((*m_OpenMap.begin()).second->fX) % ROOMCX - 144) / 72);
		iY = (int((*m_OpenMap.begin()).second->fY - 216) / 72);

		m_ClosedVec.push_back((*m_pTileVec)[iY][iX]);
		m_OpenMap.erase(m_OpenMap.begin());

		// 주변사각형 
		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				// 예외처리1 : 자신
				if (x == 0 && y == 0)
					continue;
				// 에외처리2 : 막힘
				else if ((*m_pTileVec)[iY + y][iX + x]->bMovable == false)
				{
					m_ClosedVec.push_back((*m_pTileVec)[iY][iX]);
					continue;
				}
				// 예외처리3 : 재방문
				else if (true == [&]()->bool 
				{
					for (auto& i : m_ClosedVec)
					{
						if (i == (*m_pTileVec)[iY + y][iX + x])
							return true;
					}
					return false;
				}())
					continue;
				// 예외처리4 : 대각
				else if ((*m_pTileVec)[iY + y][iX]->bMovable == false || (*m_pTileVec)[iY][iX + x]->bMovable == false)
					continue;
				// 예외처리5 : 본거?
				if (true == [&]()->bool {
					for (multimap<int, TILE*>::iterator i = m_OpenMap.begin(); i != m_OpenMap.end(); ++i )
					{
						// 본거면?
						if ((*m_pTileVec)[iY + y][iX + x] == i->second)
						{
							// 지금 길이 더 좋으면?
							if (i->second->iMoveCost >
								((0 == x*y) ? ((*m_pTileVec)[iY][iX]->iMoveCost + 10) :
								((*m_pTileVec)[iY][iX]->iMoveCost + 14)))
							{
								i->second->pSourTile = (*m_pTileVec)[iY][iX];
								i->second->iMoveCost = ((0 == x*y) ?
									((*m_pTileVec)[iY][iX]->iMoveCost + 10) :
									((*m_pTileVec)[iY][iX]->iMoveCost + 14));
								i->second->iTotalCost = i->second->iManCost + i->second->iMoveCost;
								ptemp = i->second;
								m_OpenMap.erase(i);
								m_OpenMap.insert(make_pair(ptemp->iTotalCost, ptemp));
							}
							return true;
						}
					}
					return false;
				}())
				{
					continue;
				}

				// 주변타일 -> 소스타일 설정
				(*m_pTileVec)[iY + y][iX + x]->pSourTile = (*m_pTileVec)[iY][iX];
				// 주변타일 -> 코스트 계산
				(*m_pTileVec)[iY + y][iX + x]->iManCost = 10 * (abs(ptDest.x - (iX + x)) + abs(ptDest.y - (iY + y)));
				// MoveCost
				if (x == 0 || y == 0)
					(*m_pTileVec)[iY + y][iX + x]->iMoveCost = (*m_pTileVec)[iY][iX]->iMoveCost + 10;
				else
					(*m_pTileVec)[iY + y][iX + x]->iMoveCost = (*m_pTileVec)[iY][iX]->iMoveCost + 14;

				// 토탈
				(*m_pTileVec)[iY + y][iX + x]->iTotalCost = (*m_pTileVec)[iY + y][iX + x]->iMoveCost + (*m_pTileVec)[iY + y][iX + x]->iManCost;

				// 오픈리스트에 주변타일 추가
				m_OpenMap.insert(make_pair((*m_pTileVec)[iY + y][iX + x]->iTotalCost, (*m_pTileVec)[iY + y][iX + x]));

				// 목적지 확인
				if (iY + y == ptDest.y && iX + x == ptDest.x)
					bCheck = true;
			}
		}

		if (bCheck) break;
	}

	ptemp = (*m_pTileVec)[ptDest.y][ptDest.x];
	while (true)
	{
		if (nullptr == ptemp->pSourTile->pSourTile)
		{
			POINT ptReturn{ (int(ptemp->fX) - (int(ptemp->fX) % 72) + 36), (int(ptemp->fY) - (int(ptemp->fY) % 72) + 36) };
			m_OpenMap.clear();
			m_ClosedVec.clear();
			return ptReturn;
		}

		if (g_Debug)
		{
			Ellipse(_GETMAP["back"]->GetMemDc(),
				int(ptemp->fX) - 12 + g_ScrollX,
				int(ptemp->fY) - 12 + g_ScrollY,
				int(ptemp->fX) + 12 + g_ScrollX,
				int(ptemp->fY) + 12 + g_ScrollY);
		}

		ptemp = ptemp->pSourTile;
	}


	m_OpenMap.clear();
	m_ClosedVec.clear();
}
