void CollisionCheck()
{
	/*
		skip...
	*/

	// IBB 충돌처리
	if (!(rDir.x == 0.f && rDir.y == 0.f))
	{
		RECT rc;
		////   가로형 트랙 ccd
		for (auto& tile : m_Objlist[OBJ_TRACK])
		{
			INFO&	tileInfo = tile->GetInfo();
			if (rDir.y != 0.f)// && tileInfo.vSize.x >= tileInfo.vSize.y)						// 타일이 가로형인애들만 ccd
			{
				if (IntersectRect(&rc, &ccd, &(tile->GetRect())))
				{
					if (rc.right - rc.left <= 2) continue;								// 이미지사이즈 찡김방지
					float fDistance = rIbb.vPos.y - tileInfo.vPos.y;
					float fColTime = 0.f;
					if (fDistance < 0.f)												// 트랙이 밑에있음
					{
						fDistance = ((int)tileInfo.vPos.y - (int)tileInfo.vSize.y / 2.f) - ((int)rIbb.vPos.y + (int)rIbb.vSize.y / 2.f);
						if (fDistance <= 0.f && rDir.y > 0.f)
						{
							static_cast<CIbb*>(m_pIbb)->SetYPos(fDistance);
							static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
							continue;
						}
						fColTime = fDistance / (rDir.y * 0.05f);
						if (0.17f > fColTime)
						{
							D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y - tileInfo.vSize.y / 2.f - rIbb.vSize.y / 2.f), 0.f };
							static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
						}
					}
					else													// 트랙이 위에있음
					{
						fDistance = ((int)rIbb.vPos.y - (int)rIbb.vSize.y / 2.f) - ((int)tileInfo.vPos.y + (int)tileInfo.vSize.y / 2.f);
						if (fDistance <= 0.f && rDir.y < 0.f)
						{
							static_cast<CIbb*>(m_pIbb)->SetYPos(-fDistance);
							static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
							continue;
						}
						fColTime = fDistance / (rDir.y * -0.05f);
						if (0.17f > fColTime)
						{
							D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y + tileInfo.vSize.y / 2.f + rIbb.vSize.y / 2.f), 0.f };
							static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
						}
					}
				}
			}
		}
		////   세로형 트랙
		for (auto& tile : m_Objlist[OBJ_TRACK])							// 세로형 타일 일반 밀어내기 충돌처리
		{
			INFO&	tileInfo = tile->GetInfo();
			if (tileInfo.vSize.x < tileInfo.vSize.y)
			{
				if (IntersectRect(&rc, &ccdx, &(tile->GetRect())))
				{
					float fDistance = rIbb.vPos.x - tileInfo.vPos.x;
					float fColTime = 0.f;
					if (fDistance < 0.f)											// 트랙이 오른쪽에있음
					{
						fDistance = ((int)tileInfo.vPos.x - (int)tileInfo.vSize.x / 2.f) - ((int)rIbb.vPos.x + (int)rIbb.vSize.x / 2.f);
						if (fDistance <= 0.f && rDir.x < 0.f)
						{
							continue;
						}
						fColTime = fDistance / (rDir.x * 0.05f);
						if (0.17f > fColTime)
						{
							static_cast<CIbb*>(m_pIbb)->SetXdir(0);
						}
					}
					else													// 트랙이 왼쪽에있음
					{
						fDistance = ((int)rIbb.vPos.x - (int)rIbb.vSize.x / 2.f) - ((int)tileInfo.vPos.x + (int)tileInfo.vSize.x / 2.f);
						if (fDistance <= 0.f && rDir.x > 0.f)
						{
							continue;
						}
						fColTime = fDistance / (rDir.x * -0.05f);
						if (0.17f > fColTime)
						{
							static_cast<CIbb*>(m_pIbb)->SetXdir(0);
						}
					}
				}
			}
		}
		////   구멍
		bool bCheck = false;
		for (auto& hole : m_Objlist[OBJ_HOLE])
		{
			INFO&	tileInfo = hole->GetInfo();
			if (rDir.y != 0.f && tileInfo.vSize.x >= tileInfo.vSize.y)						// 타일이 가로형인애들만 ccd
			{
				if (IntersectRect(&rc, &ccd, &(hole->GetRect())))
				{
					float fDistance = rIbb.vPos.y - tileInfo.vPos.y;
					float fColTime = fDistance / (rDir.y * 0.05f);
					if (fDistance < 0.f)												// 트랙이 밑에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(1);
						bCheck = true;
					}
					else													// 트랙이 위에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(2);
						bCheck = true;
					}
				}
			}
			else
			{
				if (IntersectRect(&rc, &ccdx, &(hole->GetRect())))
				{
					float fDistance = rIbb.vPos.x - tileInfo.vPos.x;
					float fColTime = 0.f;
					if (fDistance < 0.f)												// 트랙이 오른쪽에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(3);
						bCheck = true;
					}
					else																// 트랙이 왼쪽에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(4);
						bCheck = true;
					}
				}
			}
		}
		////  IBB 구멍
		for (auto& hole : m_Objlist[OBJ_IHOLE])
		{
			INFO&	tileInfo = hole->GetInfo();
			if (rDir.y != 0.f && tileInfo.vSize.x >= tileInfo.vSize.y)						// 타일이 가로형인애들만 ccd
			{
				if (IntersectRect(&rc, &ccd, &(hole->GetRect())))
				{
					float fDistance = rIbb.vPos.y - tileInfo.vPos.y;
					float fColTime = fDistance / (rDir.y * 0.05f);
					if (fDistance < 0.f)												// 트랙이 밑에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(1);
						bCheck = true;
					}
					else													// 트랙이 위에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(2);
						bCheck = true;
					}
				}
			}
			else
			{
				if (IntersectRect(&rc, &ccdx, &(hole->GetRect())))
				{
					float fDistance = rIbb.vPos.x - tileInfo.vPos.x;
					float fColTime = 0.f;
					if (fDistance < 0.f)												// 트랙이 오른쪽에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(3);
						bCheck = true;
					}
					else																// 트랙이 왼쪽에있음
					{
						static_cast<CIbb*>(m_pIbb)->SetHoleState(4);
						bCheck = true;
					}
				}
			}
		}
		////  OBB 구멍
		for (auto& hole : m_Objlist[OBJ_OHOLE])
		{
			INFO&	tileInfo = hole->GetInfo();
			if (tileInfo.vSize.x < tileInfo.vSize.y)
			{
				if (IntersectRect(&rc, &ccdx, &(hole->GetRect())))
				{
					float fDistance = rIbb.vPos.x - tileInfo.vPos.x;
					float fColTime = 0.f;
					if (fDistance < 0.f)											// 트랙이 오른쪽에있음
					{
						fDistance = ((int)tileInfo.vPos.x - (int)tileInfo.vSize.x / 2.f) - ((int)rIbb.vPos.x + (int)rIbb.vSize.x / 2.f);
						if (fDistance <= 0.f && rDir.x < 0.f)
						{
							continue;
						}
						fColTime = fDistance / (rDir.x * 0.05f);
						if (0.17f > fColTime)
						{
							static_cast<CIbb*>(m_pIbb)->SetXdir(0);
						}
					}
					else													// 트랙이 왼쪽에있음
					{
						fDistance = ((int)rIbb.vPos.x - (int)rIbb.vSize.x / 2.f) - ((int)tileInfo.vPos.x + (int)tileInfo.vSize.x / 2.f);
						if (fDistance <= 0.f && rDir.x > 0.f)
						{
							continue;
						}
						fColTime = fDistance / (rDir.x * -0.05f);
						if (0.17f > fColTime)
						{
							static_cast<CIbb*>(m_pIbb)->SetXdir(0);
						}
					}
				}
			}
			else
			{
				if (rDir.y != 0.f)// && tileInfo.vSize.x >= tileInfo.vSize.y)						// 타일이 가로형인애들만 ccd
				{
					if (IntersectRect(&rc, &ccd, &(hole->GetRect())))
					{
						if (rc.right - rc.left <= 2) continue;								// 이미지사이즈 찡김방지
						float fDistance = rIbb.vPos.y - tileInfo.vPos.y;
						float fColTime = 0.f;
						if (fDistance < 0.f)												// 트랙이 밑에있음
						{
							fDistance = ((int)tileInfo.vPos.y - (int)tileInfo.vSize.y / 2.f) - ((int)rIbb.vPos.y + (int)rIbb.vSize.y / 2.f);
							if (fDistance <= 0.f && rDir.y > 0.f)
							{
								static_cast<CIbb*>(m_pIbb)->SetYPos(fDistance);
								static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
								continue;
							}
							fColTime = fDistance / (rDir.y * 0.05f);
							if (0.17f > fColTime)
							{
								D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y - tileInfo.vSize.y / 2.f - rIbb.vSize.y / 2.f), 0.f };
								static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
							}
						}
						else													// 트랙이 위에있음
						{
							fDistance = ((int)rIbb.vPos.y - (int)rIbb.vSize.y / 2.f) - ((int)tileInfo.vPos.y + (int)tileInfo.vSize.y / 2.f);
							if (fDistance <= 0.f && rDir.y < 0.f)
							{
								static_cast<CIbb*>(m_pIbb)->SetYPos(-fDistance);
								static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
								continue;
							}
							fColTime = fDistance / (rDir.y * -0.05f);
							if (0.17f > fColTime)
							{
								D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y + tileInfo.vSize.y / 2.f + rIbb.vSize.y / 2.f), 0.f };
								static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
							}
						}
					}
				}
			}
		}
		if (!bCheck) static_cast<CIbb*>(m_pIbb)->SetHoleState(0);
		////   점프
		static_cast<CIbb*>(m_pIbb)->SetOnJump(0);	 // 초기화
		for (auto& jump : m_Objlist[OBJ_JUMP])
		{
			INFO&	tileInfo = jump->GetInfo();
			if (rDir.y != 0.f)// && tileInfo.vSize.x >= tileInfo.vSize.y)						// 타일이 가로형인애들만 ccd
			{
				if (IntersectRect(&rc, &ccd, &(jump->GetRect())))
				{
					if (rc.right - rc.left <= 2) continue;								// 이미지사이즈 찡김방지
					float fDistance = rIbb.vPos.y - tileInfo.vPos.y;
					float fColTime = 0.f;
					if (fDistance < 0.f)												// 트랙이 밑에있음
					{
						fDistance = ((int)tileInfo.vPos.y - (int)tileInfo.vSize.y / 2.f) - ((int)rIbb.vPos.y + (int)rIbb.vSize.y / 2.f);
						if (fDistance <= 0.f && rDir.y > 0.f)
						{
							static_cast<CIbb*>(m_pIbb)->SetYPos(fDistance);
							static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
							static_cast<CIbb*>(m_pIbb)->SetOnJump(1);
							continue;
						}
						fColTime = fDistance / (rDir.y * 0.05f);
						if (0.17f > fColTime)
						{
							if (static_cast<CObb*>(m_pObb)->GetOnJump()) static_cast<CObb*>(m_pObb)->SetYdir(rDir.y);
							D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y - tileInfo.vSize.y / 2.f - rIbb.vSize.y / 2.f), 0.f };
							static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
							static_cast<CIbb*>(m_pIbb)->SetOnJump(1);
						}
						else if (0.5f > fColTime)
						{
							if (static_cast<CObb*>(m_pObb)->GetOnJump()) static_cast<CObb*>(m_pObb)->SetYdir(rDir.y);
						}
					}
					else													// 트랙이 위에있음
					{
						fDistance = ((int)rIbb.vPos.y - (int)rIbb.vSize.y / 2.f) - ((int)tileInfo.vPos.y + (int)tileInfo.vSize.y / 2.f);
						if (fDistance <= 0.f && rDir.y < 0.f)
						{
							static_cast<CIbb*>(m_pIbb)->SetYPos(-fDistance);
							static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
							static_cast<CIbb*>(m_pIbb)->SetOnJump(1);
							continue;
						}
						fColTime = fDistance / (rDir.y * -0.05f);
						if (0.17f > fColTime)
						{
							if (static_cast<CObb*>(m_pObb)->GetOnJump()) static_cast<CObb*>(m_pObb)->SetYdir(rDir.y);
							D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y + tileInfo.vSize.y / 2.f + rIbb.vSize.y / 2.f), 0.f };
							static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
							static_cast<CIbb*>(m_pIbb)->SetOnJump(1);
						}
						else if( 0.5f > fColTime)
						{
							if (static_cast<CObb*>(m_pObb)->GetOnJump()) static_cast<CObb*>(m_pObb)->SetYdir(rDir.y);
						}
					}
				}
			}
		}
		//     발판
		for (auto& sup : m_Objlist[OBJ_SUPPORT])
		{
			INFO&	tileInfo = sup->GetInfo();
			if (static_cast<CIbb*>(m_pIbb)->GetGravity() > 0)				// 중력이 아래방향일때
			{
				if (IntersectRect(&rc, &ccd, &(sup->GetRect())))
				{
					if (rc.right - rc.left <= 3)	continue;								// 이미지사이즈 찡김방지
					if (rDir.y < 0.f)				continue;								// 플레이어가 위쪽으로 이동할때
					float fDistance = ((int)tileInfo.vPos.y - (int)tileInfo.vSize.y / 2.f) - ((int)rIbb.vPos.y + (int)rIbb.vSize.y / 2.f);	// 발판머리와 플레이어 발 사이 거리
					float fColTime = fDistance / (rDir.y * 0.05f);
					if (fDistance > 0.f)			// 거리가 벌어져있으면
					{
						if (0.4f > fColTime)		// 박을 예정이면
						{
							D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y - tileInfo.vSize.y / 2.f - rIbb.vSize.y / 2.f), 0.f };
							static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
						}
					}
					else if (fDistance == 0.f)		// 닿아있으면
					{
						static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
					}
					else							// 겹쳐있으면
					{
						if (-fDistance < rDir.y * GET_TIME)
						{
							static_cast<CIbb*>(m_pIbb)->SetYPos(fDistance);
							static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
							continue;
						}
					}
				}
			}
			else																// 중력이 위쪽일때
			{
				if (IntersectRect(&rc, &ccd, &(sup->GetRect())))
				{
					if (rc.right - rc.left <= 3)	continue;								// 이미지사이즈 찡김방지
					if (rDir.y > 0.f)				continue;								// 플레이어가 아래쪽으로 이동할때
					float fDistance = ((int)rIbb.vPos.y - (int)rIbb.vSize.y / 2.f) -  ((int)tileInfo.vPos.y + (int)tileInfo.vSize.y / 2.f);	// 발판머리와 플레이어 발 사이 거리
					float fColTime = fDistance / (rDir.y * 0.05f);
					if (fDistance > 0.f)			// 거리가 벌어져있으면
					{
						if (0.4f > fColTime)		// 박을 예정이면
						{
							D3DXVECTOR3 vColPos = { (rIbb.vPos.x), (tileInfo.vPos.y + tileInfo.vSize.y / 2.f + rIbb.vSize.y / 2.f), 0.f };
							static_cast<CIbb*>(m_pIbb)->SetCollison(fColTime, vColPos, 0);
						}
					}
					else if (fDistance == 0.f)		// 닿아있으면
					{
						static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
					}
					else							// 겹쳐있으면
					{
						if (-fDistance < rDir.y * GET_TIME)
						{
							static_cast<CIbb*>(m_pIbb)->SetYPos(fDistance);
							static_cast<CIbb*>(m_pIbb)->SetYdir(0.f);
							continue;
						}
					}
				}
			}
		}
	}
	/*
		skip...
	*/
}

void Render(void)
{
	const TEXINFO*	pTexture = CTextureMgr::GetInstance()->GetTexture(m_tInfo.ObjKey, m_tInfo.StateKey, m_tFrame.fFrame);

	if (nullptr == pTexture)
		return;

	D3DXMATRIX	matZoom;
	D3DXMatrixScaling(&matZoom, g_vZoom.x, g_vZoom.y, 0.f);
	m_tInfo.matWorld *= matZoom;

	if (m_tInfo.vPos.y < 0.f || m_tInfo.vPos.y > MAPY)
	{
		pTexture = CTextureMgr::GetInstance()->GetTexture(m_tInfo.ObjKey, L"I_jump", 0);
		D3DXMATRIX matRender, matSclae;
		if (m_tInfo.vPos.y < 0.f)
		{
			D3DXMatrixScaling(&matSclae, 1.f, g_vZoom.y, 0.f);
			D3DXMatrixTranslation(&matRender, m_tInfo.vPos.x + g_vScroll.x, 16.f, 0.f);
		}
		else
		{
			D3DXMatrixScaling(&matSclae, 1.f, -g_vZoom.y, 0.f);
			D3DXMatrixTranslation(&matRender, m_tInfo.vPos.x + g_vScroll.x, WINCY - 20.f, 0.f);
		}
		D3DXMatrixScaling(&matZoom, g_vZoom.x, 1.f, 0.f);

		float fX = pTexture->tImgInfo.Width / 2.f;
		float fY = pTexture->tImgInfo.Height / 2.f;
		matRender = matSclae * matRender * matZoom;
		CDevice::GetInstance()->GetSprite()->SetTransform(&matRender);
		CDevice::GetInstance()->GetSprite()->Draw(pTexture->pTexture, nullptr, &D3DXVECTOR3(fX, fY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		float fX = pTexture->tImgInfo.Width / 2.f;
		float fY = pTexture->tImgInfo.Height / 2.f;
		CDevice::GetInstance()->GetSprite()->SetTransform(&m_tInfo.matWorld);
		CDevice::GetInstance()->GetSprite()->Draw(pTexture->pTexture, nullptr, &D3DXVECTOR3(fX, fY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}
