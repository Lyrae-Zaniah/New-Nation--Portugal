extern void InitNations();

#event_handler(GET_RELATION_EVENT,"GetRelationEvent");

void Nation_InitAfterLoading()
{
	int i,j;
	// check version
	int version_number = 099;
	if( CheckAttribute(pchar,"version_number") ) {
		version_number = pchar.version_number;
	}

	if( version_number < 104 )
	// делаем ресайз наций для добавления новой нации
	{
		// меняем таблицу наций
		SetArraySize(&Nations,MAX_NATIONS);
		if(LoadSegment("nations\nations_init.c"))
		{
			InitNations();
			UnloadSegment("nations\nations_init.c");
		}

		// меняем таблицу отношений между нациями
		SetArraySize(&NationsRelations,MAX_NATIONS_SQR);
		for(i=MAX_NATIONS-2;i>=0;i--) {
			for(j=MAX_NATIONS-2;j>=0;j--) {
				NationsRelations[i * MAX_NATIONS + j] = NationsRelations[i * (MAX_NATIONS-1) + j];
			}
			NationsRelations[i * MAX_NATIONS + MAX_NATIONS-1] = RELATION_FRIEND;
		}
		//for(i=0; i<MAX_NATIONS; i++) {
		//	NationsRelations[SMUGGLER * MAX_NATIONS + i] = RELATION_FRIEND;
		//}

		// меняем таблицу отношений наций к главному персонажу
		SetArraySize(&NationsRelations2MainCharacter,MAX_NATIONS);
	}
}

void Nations_SetAllFriends()
{
	int i;
	
	for(i=0;i<MAX_NATIONS_SQR;i++)
	{
		NationsRelations[i] = RELATION_FRIEND;
	}

	for(i=0;i<MAX_NATIONS;i++)
	{
		NationsRelations2MainCharacter[i] = RELATION_FRIEND;
	}
}

void NationsInit()
{
	if(LoadSegment("nations\nations_init.c"))
	{
		InitNations();
		UnloadSegment("nations\nations_init.c");
	}
    
	Nations_SetAllFriends();
}

void SetNationRelation2MainCharacter(int iNation, int iRelationType)
{
	NationsRelations2MainCharacter[iNation] = iRelationType;
}

void SetNationRelationBoth(int iNation1, int iNation2, int iRelationType)
{
    if (iNation1 != iNation2)//fix
    {
        NationsRelations[iNation1 * MAX_NATIONS + iNation2] = iRelationType;
    	NationsRelations[iNation2 * MAX_NATIONS + iNation1] = iRelationType;
	}
}

void SetNationRelation(int iNation1, int iNation2, int iRelationType)
{
	NationsRelations[iNation1 * MAX_NATIONS + iNation2] = iRelationType;
}

// =================================================================================================
// Character Relation
// =================================================================================================
void SetCharacterRelationAsOtherCharacter(int iCharacterIndex1, int iCharacterIndex2)
{
	Characters[iCharacterIndex1].relation.UseOtherCharacter = iCharacterIndex2;
}

void SetCharacterRelationBoth(int iCharacterIndex1, int iCharacterIndex2, int iRelationType)
{
	SetCharacterRelation(iCharacterIndex1, iCharacterIndex2, iRelationType);
	SetCharacterRelation(iCharacterIndex2, iCharacterIndex1, iRelationType);
}

void SetCharacterRelation(int iCharacterIndex1, int iCharacterIndex2, int iRelationType)
{
	string sTemp1 = "" + iCharacterIndex2;
	Characters[iCharacterIndex1].relation.(sTemp1) = iRelationType;
}

// =================================================================================================
// Get Relation Methods
// =================================================================================================
int GetNationRelation(int iNation1, int iNation2)
{
    // boal fix 171004 на всяк случай
    if (iNation1 == iNation2) return RELATION_FRIEND;
    
    return NationsRelations[iNation1 * MAX_NATIONS + iNation2];
}

int GetNationRelation2MainCharacter(int iNation)
{
	return NationsRelations2MainCharacter[iNation];
}
// отношение к базе boal 05/07/06
int GetRelation2BaseNation(int iNation)
{
	if (iNation == GetBaseHeroNation())
	{
	    if (GetCharacterReputation_WithNation(pchar, iNation) <= -10)
	    {
	        return RELATION_ENEMY;
	    }
	}
	return GetNationRelation(iNation, GetBaseHeroNation());
}

int GetNationRelation2Character(int iNation, int iCharacterIndex)
{
	ref rCharacter = GetCharacter(iCharacterIndex);

	if (CheckAttribute(&rCharacter, "relation.UseOtherCharacter"))
	{
		if (sti(rCharacter.relation.UseOtherCharacter) >= 0)
		{
			rCharacter = GetCharacter(sti(rCharacter.relation.UseOtherCharacter));
		}
	}

	int iMCI = GetMainCharacterIndex();
	int iNation2 = sti(rCharacter.nation);
	if (iMCI == sti(rCharacter.index))
	{
		return NationsRelations2MainCharacter[iNation];
	}
	return NationsRelations[iNation * MAX_NATIONS + iNation2];
}

int GetRelation(int iCharacterIndex1, int iCharacterIndex2)
{
	int iMCI = GetMainCharacterIndex();

	ref rCharacter1 = GetCharacter(iCharacterIndex1);
	ref rCharacter2 = GetCharacter(iCharacterIndex2);

	// if sea active, check groups first, if both have same group - they are friends
	if (bSeaActive)
	{
		if (CheckAttribute(rCharacter1, "SeaAI.Group.Name") && CheckAttribute(rCharacter2, "SeaAI.Group.Name"))
		{
			if (rCharacter1.SeaAI.Group.Name == rCharacter2.SeaAI.Group.Name) { return RELATION_FRIEND; }
		}
	}

	if (CheckAttribute(&rCharacter1, "relation.UseOtherCharacter"))
	{
		if (sti(rCharacter1.relation.UseOtherCharacter) >= 0)
		{
			rCharacter1 = GetCharacter(sti(rCharacter1.relation.UseOtherCharacter));
		}
	}
	if (CheckAttribute(&rCharacter2, "relation.UseOtherCharacter"))
	{
		if (sti(rCharacter2.relation.UseOtherCharacter) >= 0)
		{
			rCharacter2 = GetCharacter(sti(rCharacter2.relation.UseOtherCharacter));
		}
	}

	if (rCharacter1.index == rCharacter2.index) { return RELATION_FRIEND; }

	// character 2 character relation
	string sTemp = "relation." + rCharacter2.index;
	if (CheckAttribute(&rCharacter1,sTemp))
	{
		return sti(rCharacter1.(sTemp));
	}
	//Trace("iNation1 = " + iNation1 + ", iNation2 = " + iNation2);

	int iNation1 = sti(rCharacter1.nation);
	int iNation2 = sti(rCharacter2.nation);

	// nation 2 character relation
	if (iMCI == sti(rCharacter1.index))
	{
		//Trace("iMainCharacterRelation = " + NationsRelations2MainCharacter[iNation2]);
		return NationsRelations2MainCharacter[iNation2];
	}
	if (iMCI == sti(rCharacter2.index))
	{
		//Trace("iMainCharacterRelation = " + NationsRelations2MainCharacter[iNation1]);
		return NationsRelations2MainCharacter[iNation1];
	}

	// nation 2 nation relation
	return NationsRelations[iNation1 * MAX_NATIONS + iNation2];
}

int GetRelationEvent()
{
	int iCharacterIndex1 = GetEventData();
	int iCharacterIndex2 = GetEventData();
	return GetRelation(iCharacterIndex1, iCharacterIndex2);
}

int FindEnemyNation2Nation(int iNation)
{
	int iNations[MAX_NATIONS];
	int iNumNations = 0;

	for (int i=0;i<MAX_NATIONS;i++)
	{
		if (GetNationRelation(i,iNation) == RELATION_ENEMY)
		{
			iNations[iNumNations] = i;
			iNumNations++;
		}
	}
	if (iNumNations == 0) return -1;
	return iNations[rand(iNumNations - 1)];
}

int FindNonEnemyNation2Nation(int iNation)
{
	int iNations[MAX_NATIONS];
	int iNumNations = 0;

	for (int i=0;i<MAX_NATIONS;i++)
	{
		if (GetNationRelation(i,iNation) != RELATION_ENEMY && i != iNation)
		{
			iNations[iNumNations] = i;
			iNumNations++;
		}
	}
	if (iNumNations == 0) return -1;
	return iNations[rand(iNumNations - 1)];
}


int FindNonEnemyNation2NationWithoutPirates(int iNation)
{
	int iNations[MAX_NATIONS];
	int iNumNations = 0;

	for (int i=0;i<MAX_NATIONS;i++)
	{
		if (i == PIRATE) continue; // Nova Nacao: inclui Portugal
		if (GetNationRelation(i,iNation) != RELATION_ENEMY && i != iNation)
		{
			iNations[iNumNations] = i;
			iNumNations++;
		}
	}
	if (iNumNations == 0) return -1;
	return iNations[rand(iNumNations - 1)];
}

int FindEnemyNation2NationWithoutPirates(int iNation)
{
	int iNations[MAX_NATIONS];
	int iNumNations = 0;

	for (int i=0;i<MAX_NATIONS;i++)
	{
		if (i == PIRATE) continue; // Nova Nacao: inclui Portugal
		if (GetNationRelation(i,iNation) == RELATION_ENEMY)
		{
			iNations[iNumNations] = i;
			iNumNations++;
		}
	}
	if (iNumNations == 0) return -1;
	return iNations[rand(iNumNations - 1)];
}

int FindQuestNationWithoutPirates(int iNation)
{
	int iNations[MAX_NATIONS];
	int iNumNations = 0;

	for (int i=0;i<MAX_NATIONS;i++)
	{
		if (i == PIRATE) continue; // Nova Nacao: inclui Portugal
		if (i != iNation)
		{
			iNations[iNumNations] = i;
			iNumNations++;
		}
	}
	if (iNumNations == 0) return -1;
	return iNations[rand(iNumNations - 1)];
}

int FindEnemyNation2Character(int iCharacterIndex)
{
	int iNations[MAX_NATIONS];
	int iNumNations = 0;

	for (int i=0; i<MAX_NATIONS; i++)
	{
		if (GetNationRelation2Character(i, iCharacterIndex) == RELATION_ENEMY)
		{
			iNations[iNumNations] = i;
			iNumNations++;
		}
	}
	if (iNumNations == 0) return -1;
	return iNations[rand(iNumNations - 1)];
}

int GetNationTypeByName(string nationName)
{
	for(int i=0; i<MAX_NATIONS; i++)
	{
		if(Nations[i].Name==nationName) return i;
	}
	return -1;
}

string GetNationNameByType(int iNationType)
{
	if (iNationType < MAX_NATIONS)
	{
		return Nations[iNationType].Name;
	}
	return "-1";
}

string GetShortNationNameByType(int iNationType)
{
	if (iNationType < MAX_NATIONS)
	{
		return Nations[iNationType].ShortName;
	}
	return "-1";
}

ref GetNationByType(int iNationType)
{
	return &Nations[iNationType];
}

string GetRelationName(int relationCode)
{
	switch(relationCode)
	{
	case RELATION_FRIEND:	return "Friend"; break;
	case RELATION_ENEMY:	return "Enemy"; break;
	case RELATION_NEUTRAL:	return "Neutral"; break;
	}
	return "Unknown";
}

int FindNationExceptNationWithoutPirates(int iNation)
{
	object RandNations[MAX_NATIONS];
	int m = 0;
	for(int i = 0; i < MAX_NATIONS - 2; i++)
	{
		if(i != iNation)
		{
			RandNations[m].nation = i;
			m++;
		}
	}
	m = rand(m - 1);
	int iResultNation = sti(RandNations[m].nation);
	return iResultNation;
}

bool IsStopMapFollowEncounters()
{
	int iRelation;
	for(int i = 0; i < MAX_NATIONS; i++)
	{
		iRelation = GetNationRelation2MainCharacter(i);
		if(iRelation != RELATION_FRIEND)
		{
			return false;
		}
	}
	return true;
}

int GetEnemyNationToMainCharacter()
{
	object Relations[MAX_NATIONS];
	int iStep = -1;
	for(int i = 0; i < MAX_NATIONS; i++)
	{
		if(GetNationRelation2MainCharacter(i) == RELATION_ENEMY)
		{
			iStep++;
			Relations[iStep].nation = i;
		}
	}
	if(iStep == -1)
	{
		trace("WARNING! NO ENEMY NATION!");
		return -1;
	}
	iStep = rand(iStep);
	int iResult = sti(Relations[iStep].nation);

	return iResult;
}
// boal
void DelCharacterRelation(int iCharacterIndex1)
{
	DeleteAttribute(&Characters[iCharacterIndex1], "relation");
}
// =================================================================================================
// Nova Nacao: Portugal
// Portugal tem identidade propria ("por") para reputacao, cacadores, IDs e estatisticas,
// mas ainda nao tem modelos/musicas/falas proprios: para esses assets usa os espanhois.
// =================================================================================================
string NationAssetShortName(int iNation)
{
	if (iNation == PORTUGAL) return "spa";
	return NationShortName(iNation);
}

// Modelos portugueses (uniformes ingleses recoloridos: vermelho -> branco)
#define PORTUGAL_MODELS_1 ",elite_por_1,elite_por_2,elite_por_3,elite_por_4,elite_por_5,guard_por_1,guard_por_2,guard_por_3,guard_por_4,milit_mush_por_1,milit_mush_por_2,milit_mush_por_3,milit_mush_por_4,milit_mush_por_5,milit_mush_por_6,milit_mush_por_7,milit_mush_por_8,milit_por_1,milit_por_2,milit_por_3,milit_por_4,milit_por_5,milit_por_6,milit_por_7,milit_por_8,navy_mush_por_1,navy_mush_por_2,navy_mush_por_3,navy_mush_por_4,navy_mush_por_5,navy_mush_por_6,navy_off_por_1,navy_off_por_2,navy_off_por_3,navy_off_por_4,navy_off_por_5,navy_off_por_6,navy_por_1,navy_por_10,navy_por_11,navy_por_12,navy_por_13,navy_por_14,navy_por_15,navy_por_16,navy_por_2,navy_por_3,navy_por_4,navy_por_5,navy_por_6,navy_por_7,navy_por_8,"
#define PORTUGAL_MODELS_2 ",navy_por_9,off_por_1,off_por_10,off_por_11,off_por_2,off_por_3,off_por_4,off_por_5,off_por_6,off_por_7,off_por_8,off_por_9,por_mush_1,por_mush_2,por_mush_3,por_mush_4,por_mush_5,por_mush_6,por_mush_7,por_mush_8,sold_por_1,sold_por_10,sold_por_11,sold_por_12,sold_por_13,sold_por_14,sold_por_15,sold_por_16,sold_por_2,sold_por_3,sold_por_4,sold_por_5,sold_por_6,sold_por_7,sold_por_8,sold_por_9,urban_mush_por_1,urban_mush_por_2,urban_mush_por_3,urban_mush_por_4,urban_mush_por_5,urban_mush_por_6,urban_mush_por_7,urban_mush_por_8,urban_por_1,urban_por_2,urban_por_3,urban_por_4,urban_por_5,urban_por_6,urban_por_7,urban_por_8,"

// nome curto para MODELOS 3D: Portugal tem modelos proprios ("por")
// icone da colonia no mapa-mundi (icons.tga): Portugal usa a linha 12
int NationWorldMapIcon(int iNation)
{
	if (iNation == PORTUGAL) return 12;
	return iNation;
}

string NationModelShortName(int iNation)
{
	if (iNation == PORTUGAL) return "por";
	return NationShortName(iNation);
}

bool PortugalModelExists(string sModel)
{
	string sKey = "," + sModel + ",";
	if (findSubStr(PORTUGAL_MODELS_1, sKey, 0) >= 0) return true;
	return findSubStr(PORTUGAL_MODELS_2, sKey, 0) >= 0;
}

// rede de seguranca: modelo portugues que nao existe -> equivalente espanhol
string PortugalFixModel(string sModel)
{
	bool bPor = findSubStr(sModel, "_por_", 0) >= 0;
	if (!bPor && strlen(sModel) > 4)
	{
		if (strcut(sModel, 0, 3) == "por_") bPor = true;
	}
	if (!bPor) return sModel;
	if (PortugalModelExists(sModel)) return sModel;
	if (findSubStr(sModel, "_por_", 0) >= 0) return StrReplaceAll(sModel, "_por_", "_spa_");
	return "spa_" + strcut(sModel, 4, strlen(sModel) - 1);
}

// retrato: modelo portugues usa o retrato do equivalente ingles
string PortugalPortraitModel(string sModel)
{
	if (!PortugalModelExists(sModel)) return sModel;
	if (findSubStr(sModel, "_por_", 0) >= 0) return StrReplaceAll(sModel, "_por_", "_eng_");
	return "eng_" + strcut(sModel, 4, strlen(sModel) - 1);
}

// navios: Portugal usa a lista de navios disponiveis da Espanha
int NationForShipList(int iNation)
{
	if (iNation == PORTUGAL) return SPAIN;
	return iNation;
}

// Nova Nacao: da ao jogador o posto de Governador-Geral (6o posto portugues).
// Para ser chamada pela linha de missoes de Portugal. Exige a carta de corso portuguesa equipada.
bool Portugal_SetGovernorGeneral()
{
	if (!isMainCharacterPatented()) return false;
	ref itm;
	makeref(itm, Items[sti(pchar.EquipedPatentId)]);
	if (sti(itm.Nation) != PORTUGAL) return false;
	itm.TitulCur = 6;
	itm.TitulCurNext = 0;
	return true;
}
