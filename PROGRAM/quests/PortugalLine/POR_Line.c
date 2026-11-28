/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Nova Nacao: linha de missoes de Portugal - "Pela Gloria da Coroa"
//	Estado em pchar.questTemp.PorLine:
//	""  -> M1_go -> M1_done -> M2_go -> M2_aruba -> M2_done -> M3_go -> M3_done/M3_lost -> M4_go -> M4_done
//	-> M5_godinho -> M5_secretary -> M6_go -> M6_rep -> M7_go -> M7_captured/M7_sunk -> done
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define POR_DLG "Quest\PortugalLine\POR_Line_dialog.c"

// ---------------------------------------------------------------- utilitarios
void POR_SetRank(int iRank)
{
	if (!isMainCharacterPatented()) return;
	ref itm;
	makeref(itm, Items[sti(pchar.EquipedPatentId)]);
	if (sti(itm.Nation) != PORTUGAL) return;
	if (sti(itm.TitulCur) >= iRank) return;
	itm.TitulCur = iRank;
	itm.TitulCurNext = 0;
	Log_Info(StringFromKey("Diplomat_dialog_2003", GetAddress_FormTitle(PORTUGAL, iRank)));
}

void POR_SetLocCond(string sQuest, string sLoc)
{
	DeleteAttribute(pchar, "quest." + sQuest);
	pchar.quest.(sQuest).win_condition.l1 = "location";
	pchar.quest.(sQuest).win_condition.l1.location = sLoc;
	pchar.quest.(sQuest).function = sQuest;
}

void POR_SetGroupDeathCond(string sQuest, string sGroup)
{
	DeleteAttribute(pchar, "quest." + sQuest);
	pchar.quest.(sQuest).win_condition.l1 = "Group_Death";
	pchar.quest.(sQuest).win_condition.l1.group = sGroup;
	pchar.quest.(sQuest).function = sQuest;
}

void POR_SetMapEnterCond(string sQuest)
{
	DeleteAttribute(pchar, "quest." + sQuest);
	pchar.quest.(sQuest).win_condition.l1 = "MapEnter";
	pchar.quest.(sQuest).function = sQuest;
}

void POR_GovMark(bool bOn)
{
	int iGov = GetCharacterIndex("por_guber");
	if (iGov < 0) return;
	if (bOn) AddLandQuestMark_Main(&Characters[iGov], "POR_Line");
	else RemoveLandQuestMark_Main(&Characters[iGov], "POR_Line");
}

// cria um navio inimigo da missao e o coloca no grupo
ref POR_AddShip(string sGroup, int idx, string sModel, int iNation, int iShip, int iCannon, int iRankAdd, string sBlade)
{
	int iTry = 0;
	if (CheckAttribute(pchar, "questTemp.PorLine.Try")) iTry = sti(pchar.questTemp.PorLine.Try);
	int iRank = sti(pchar.rank) + iRankAdd;
	ref sld = GetCharacter(NPC_GenerateCharacter(sGroup + "_" + iTry + "_" + idx, sModel, "man", "man", iRank, iNation, 0, true));
	FantomMakeCoolSailor(sld, iShip, "", iCannon, 60, 55, 55);
	FantomMakeCoolFighter(sld, iRank, 60, 50, sBlade, "pistol3", 50);
	Group_AddCharacter(sGroup, sld.id);
	return sld;
}

// prepara a esquadra para atacar o jogador perto da ilha
void POR_LaunchGroup(string sGroup, string sCommander, string sIsland)
{
	Group_SetGroupCommander(sGroup, sCommander);
	Group_SetTaskAttack(sGroup, PLAYER_GROUP);
	Group_SetPursuitGroup(sGroup, PLAYER_GROUP);
	Group_SetAddress(sGroup, sIsland, "", "");
	Group_LockTask(sGroup);
	Island_SetReloadEnableGlobal(sIsland, false);
}

void POR_NewGroup(string sGroup, string sType)
{
	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, sType);
	if (!CheckAttribute(pchar, "questTemp.PorLine.Try")) pchar.questTemp.PorLine.Try = 0;
	pchar.questTemp.PorLine.Try = sti(pchar.questTemp.PorLine.Try) + 1;
}

// personagem de terra parado num local
ref POR_MakeLandNpc(string sId, string sModel, string sName, string sLastName, string sLoc, string sGroup, string sLocator)
{
	ref sld = GetCharacter(NPC_GenerateCharacter(sId, sModel, "man", "man", 10, PORTUGAL, -1, false));
	sld.name = sName;
	sld.lastname = sLastName;
	sld.Dialog.Filename = POR_DLG;
	sld.Dialog.CurrentNode = "First time";
	LAi_SetImmortal(sld, true);
	LAi_LoginInCaptureTown(sld, true);
	if (sGroup == "sit")
	{
		// sentado a uma mesa da taverna, como os fregueses
		FreeSitLocator(sLoc, sLocator);
		ChangeCharacterAddressGroup(sld, sLoc, sGroup, sLocator);
		LAi_SetSitType(sld);
	}
	else
	{
		ChangeCharacterAddressGroup(sld, sLoc, sGroup, sLocator);
		LAi_SetStayTypeNoGroup(sld);
	}
	AddLandQuestMark_Main(sld, "POR_Line");
	return sld;
}

void POR_RemoveNpc(string sId)
{
	int i = GetCharacterIndex(sId);
	if (i < 0) return;
	RemoveLandQuestMark_Main(&Characters[i], "POR_Line");
	ChangeCharacterAddressGroup(&Characters[i], "none", "", "");
	Characters[i].lifeDay = 0;
}

void POR_Reward(int iMoney, int iRep)
{
	AddMoneyToCharacter(pchar, iMoney);
	ChangeCharacterNationReputation(pchar, PORTUGAL, iRep);
	AddCharacterExpToSkill(pchar, "Leadership", 100);
	AddCharacterExpToSkill(pchar, "Sailing", 100);
	AddCharacterExpToSkill(pchar, "Accuracy", 80);
}

// ---------------------------------------------------------------- missao 1: piratas de Trinidad
void POR_M1_Start()
{
	pchar.questTemp.PorLine = "M1_go";
	POR_Suit_Give(); // Nova Nacao 1.1.0: Lisboa manda o traje de fidalgo
	SetQuestHeader("POR_Line");
	AddQuestRecord("POR_Line", "1");
	POR_GovMark(false);
	POR_SetLocCond("POR_M1_Battle", "Trinidad");
}

void POR_M1_Battle(string qName)
{
	ref sld;
	DeleteAttribute(pchar, "questTemp.PorLine.M1Boarded");
	DeleteAttribute(pchar, "questTemp.PorLine.M1Sunk");
	POR_NewGroup("POR_M1", "pirate");
	sld = POR_AddShip("POR_M1", 1, "pirate_" + (rand(15) + 1), PIRATE, SHIP_BRIGANTINE, CANNON_TYPE_CANNON_LBS16, 2, "blade24");
	sld.name = "Bento";
	sld.lastname = "Malheiro";
	sld.Ship.Name = "Corvo Negro";
	sld.Dialog.Filename = POR_DLG;
	sld.Dialog.CurrentNode = "First time";
	pchar.questTemp.PorLine.Cabin = sld.id;
	pchar.questTemp.PorLine.Cmd = sld.id;
	sld = POR_AddShip("POR_M1", 2, "pirate_" + (rand(15) + 1), PIRATE, SHIP_SCHOONER_W, CANNON_TYPE_CANNON_LBS12, 0, "blade24");
	POR_LaunchGroup("POR_M1", pchar.questTemp.PorLine.Cmd, "Trinidad");
	POR_SetGroupDeathCond("POR_M1_Win", "POR_M1");
	POR_SetMapEnterCond("POR_M1_Flee");
}

void POR_M1_Win(string qName)
{
	DeleteQuestCondition("POR_M1_Flee");
	Island_SetReloadEnableGlobal("Trinidad", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	pchar.questTemp.PorLine = "M1_done";
	if (CheckAttribute(pchar, "questTemp.PorLine.M1Boarded"))
	{
		AddQuestRecord("POR_Line", "2");
	}
	else
	{
		pchar.questTemp.PorLine.M1Sunk = true;
		AddQuestRecord("POR_Line", "2b");
	}
	POR_GovMark(true);
}

void POR_M1_Flee(string qName)
{
	DeleteQuestCondition("POR_M1_Win");
	Group_DeleteGroup("POR_M1");
	Island_SetReloadEnableGlobal("Trinidad", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	AddQuestRecord("POR_Line", "f");
	POR_SetLocCond("POR_M1_Battle", "Trinidad");
}

void POR_M1_Reward()
{
	POR_Reward(15000, 5);
	POR_SetRank(2);
	POR_GovMark(false);
}

// ---------------------------------------------------------------- missao 2: o ouro de Castela
void POR_M2_Start()
{
	pchar.questTemp.PorLine = "M2_go";
	AddQuestRecord("POR_Line", "3");
	POR_MakeLandNpc("POR_Tome", "citiz_" + (rand(9) + 11), "Tomé", "Ferraz", "Villemstad_tavern", "sit", "sit_base1");
}

void POR_M2_ToAruba()
{
	pchar.questTemp.PorLine = "M2_aruba";
	AddQuestRecord("POR_Line", "4");
	RemoveLandQuestMark_Main(CharacterFromID("POR_Tome"), "POR_Line");
	POR_SetLocCond("POR_M2_Battle", "Aruba");
}

void POR_M2_Battle(string qName)
{
	ref sld;
	DeleteAttribute(pchar, "questTemp.PorLine.M2Boarded");
	DeleteAttribute(pchar, "questTemp.PorLine.M2Sunk");
	POR_NewGroup("POR_M2", "war");
	sld = POR_AddShip("POR_M2", 1, "navy_off_spa_3", SPAIN, SHIP_BRIG, CANNON_TYPE_CANNON_LBS16, 3, "blade26");
	sld.name = "Íñigo";
	sld.lastname = "Salcedo";
	sld.Ship.Name = "Santa Clara";
	sld.Dialog.Filename = POR_DLG;
	sld.Dialog.CurrentNode = "First time";
	pchar.questTemp.PorLine.Cabin = sld.id;
	pchar.questTemp.PorLine.Cmd = sld.id;
	sld = POR_AddShip("POR_M2", 2, "navy_off_spa_3", SPAIN, SHIP_BRIGANTINE, CANNON_TYPE_CANNON_LBS16, 2, "blade26");
	POR_LaunchGroup("POR_M2", pchar.questTemp.PorLine.Cmd, "Aruba");
	POR_SetGroupDeathCond("POR_M2_Win", "POR_M2");
	POR_SetMapEnterCond("POR_M2_Flee");
}

void POR_M2_Win(string qName)
{
	DeleteQuestCondition("POR_M2_Flee");
	Island_SetReloadEnableGlobal("Aruba", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	pchar.questTemp.PorLine = "M2_done";
	if (CheckAttribute(pchar, "questTemp.PorLine.M2Boarded"))
	{
		AddQuestRecord("POR_Line", "5");
	}
	else
	{
		pchar.questTemp.PorLine.M2Sunk = true;
		pchar.questTemp.PorLine.Letters = true;
		AddQuestRecord("POR_Line", "5b");
	}
	POR_GovMark(true);
}

void POR_M2_Flee(string qName)
{
	DeleteQuestCondition("POR_M2_Win");
	Group_DeleteGroup("POR_M2");
	Island_SetReloadEnableGlobal("Aruba", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	AddQuestRecord("POR_Line", "f");
	POR_SetLocCond("POR_M2_Battle", "Aruba");
}

void POR_M2_Reward()
{
	POR_Reward(20000, 5);
	POR_GovMark(false);
	POR_RemoveNpc("POR_Tome");
}

// ---------------------------------------------------------------- missao 3: as velas de Pernambuco
void POR_M3_Start()
{
	pchar.questTemp.PorLine = "M3_go";
	AddQuestRecord("POR_Line", "6");
	POR_SetLocCond("POR_M3_Battle", "Cumana");
}

void POR_M3_Battle(string qName)
{
	ref sld;
	DeleteAttribute(pchar, "questTemp.PorLine.MerchLost");
	// a flauta de Pernambuco (aliada)
	POR_NewGroup("POR_M3_Ours", "trade");
	pchar.questTemp.PorLine.AmaroModel = "merch_" + (rand(14) + 1);
	string sAmaro = pchar.questTemp.PorLine.AmaroModel;
	sld = POR_AddShip("POR_M3_Ours", 1, sAmaro, PORTUGAL, SHIP_FLEUT, CANNON_TYPE_CANNON_LBS12, 0, "blade24");
	sld.name = "Amaro";
	sld.lastname = "Teles";
	sld.Ship.Name = "Desterro";
	sld.ShipEnemyDisable = true;
	sld.Abordage.Enable = false;
	pchar.questTemp.PorLine.Merch = sld.id;
	Group_SetGroupCommander("POR_M3_Ours", sld.id);
	Group_SetAddress("POR_M3_Ours", "Cumana", "", "");
	// os piratas
	POR_NewGroup("POR_M3", "pirate");
	sld = POR_AddShip("POR_M3", 1, "pirate_" + (rand(15) + 1), PIRATE, SHIP_CORVETTE_L, CANNON_TYPE_CANNON_LBS16, 3, "blade33");
	pchar.questTemp.PorLine.Cmd = sld.id;
	sld = POR_AddShip("POR_M3", 2, "pirate_" + (rand(15) + 1), PIRATE, SHIP_BRIG, CANNON_TYPE_CANNON_LBS16, 2, "blade24");
	sld = POR_AddShip("POR_M3", 3, "pirate_" + (rand(15) + 1), PIRATE, SHIP_SCHOONER_W, CANNON_TYPE_CANNON_LBS12, 1, "blade24");
	POR_LaunchGroup("POR_M3", pchar.questTemp.PorLine.Cmd, "Cumana");
	Group_UnlockTask("POR_M3"); // os piratas vao atras da flauta
	Group_SetTaskAttack("POR_M3", "POR_M3_Ours");
	Group_SetTaskAttack("POR_M3_Ours", "POR_M3");
	Group_LockTask("POR_M3_Ours");
	POR_SetGroupDeathCond("POR_M3_Win", "POR_M3");
	POR_SetMapEnterCond("POR_M3_Flee");
	DeleteAttribute(pchar, "quest.POR_M3_MerchDead");
	pchar.quest.POR_M3_MerchDead.win_condition.l1 = "NPC_Death";
	pchar.quest.POR_M3_MerchDead.win_condition.l1.character = pchar.questTemp.PorLine.Merch;
	pchar.quest.POR_M3_MerchDead.function = "POR_M3_MerchDead";
}

void POR_M3_MerchDead(string qName)
{
	pchar.questTemp.PorLine.MerchLost = true;
}

void POR_M3_Win(string qName)
{
	DeleteQuestCondition("POR_M3_Flee");
	DeleteQuestCondition("POR_M3_MerchDead");
	Island_SetReloadEnableGlobal("Cumana", true);
	Group_DeleteAtEnd("POR_M3_Ours");
	if (CheckAttribute(pchar, "questTemp.PorLine.MerchLost"))
	{
		pchar.questTemp.PorLine = "M3_lost";
		AddQuestRecord("POR_Line", "7b");
	}
	else
	{
		// o capitao da flauta quer conversar na taverna
		pchar.questTemp.PorLine = "M3_amaro";
		AddQuestRecord("POR_Line", "7a");
		string sAmaroM = "merch_" + (rand(14) + 1);
		if (CheckAttribute(pchar, "questTemp.PorLine.AmaroModel")) sAmaroM = pchar.questTemp.PorLine.AmaroModel;
		POR_MakeLandNpc("POR_Amaro", sAmaroM, "Amaro", "Teles", "Cumana_tavern", "sit", "sit_base3");
		return;
	}
	POR_GovMark(true);
}

void POR_M3_AmaroDone()
{
	pchar.questTemp.PorLine = "M3_done";
	AddQuestRecord("POR_Line", "7");
	RemoveLandQuestMark_Main(CharacterFromID("POR_Amaro"), "POR_Line");
	POR_GovMark(true);
}

void POR_M3_Flee(string qName)
{
	DeleteQuestCondition("POR_M3_Win");
	DeleteQuestCondition("POR_M3_MerchDead");
	Group_DeleteGroup("POR_M3");
	Group_DeleteGroup("POR_M3_Ours");
	Island_SetReloadEnableGlobal("Cumana", true);
	AddQuestRecord("POR_Line", "f");
	POR_SetLocCond("POR_M3_Battle", "Cumana");
}

void POR_M3_Reward()
{
	if (pchar.questTemp.PorLine == "M3_lost") POR_Reward(10000, 3);
	else POR_Reward(25000, 8);
	POR_SetRank(3);
	POR_GovMark(false);
	POR_RemoveNpc("POR_Amaro");
}

// ---------------------------------------------------------------- missao 4: o ninho do Mao de Ferro
void POR_M4_Start()
{
	pchar.questTemp.PorLine = "M4_go";
	AddQuestRecord("POR_Line", "8");
	POR_SetLocCond("POR_M4_Battle", "Caiman");
}

void POR_M4_Battle(string qName)
{
	ref sld;
	POR_NewGroup("POR_M4", "pirate");
	sld = POR_AddShip("POR_M4", 1, "pirate_" + (rand(15) + 1), PIRATE, SHIP_FRIGATE, CANNON_TYPE_CANNON_LBS24, 5, "blade33");
	sld.name = "Gaspar";
	sld.lastname = "Roiz";
	sld.Ship.Name = "Leviatã";
	sld.Dialog.Filename = POR_DLG;
	sld.Dialog.CurrentNode = "First time";
	pchar.questTemp.PorLine.Cabin = sld.id;
	pchar.questTemp.PorLine.Cmd = sld.id;
	sld = POR_AddShip("POR_M4", 2, "pirate_" + (rand(15) + 1), PIRATE, SHIP_CORVETTE, CANNON_TYPE_CANNON_LBS20, 3, "blade33");
	sld = POR_AddShip("POR_M4", 3, "pirate_" + (rand(15) + 1), PIRATE, SHIP_BRIG, CANNON_TYPE_CANNON_LBS16, 2, "blade24");
	POR_LaunchGroup("POR_M4", pchar.questTemp.PorLine.Cmd, "Caiman");
	POR_SetGroupDeathCond("POR_M4_Win", "POR_M4");
	POR_SetMapEnterCond("POR_M4_Flee");
}

void POR_M4_Win(string qName)
{
	DeleteQuestCondition("POR_M4_Flee");
	Island_SetReloadEnableGlobal("Caiman", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	pchar.questTemp.PorLine = "M4_done";
	// abordou e falou com o Roiz, ou so afundou a esquadra
	if (CheckAttribute(pchar, "questTemp.PorLine.RoizBoarded")) AddQuestRecord("POR_Line", "9");
	else AddQuestRecord("POR_Line", "9b");
	POR_GovMark(true);
}

void POR_M4_Flee(string qName)
{
	DeleteQuestCondition("POR_M4_Win");
	Group_DeleteGroup("POR_M4");
	Island_SetReloadEnableGlobal("Caiman", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	AddQuestRecord("POR_Line", "f");
	POR_SetLocCond("POR_M4_Battle", "Caiman");
}

void POR_M4_Reward()
{
	POR_Reward(40000, 10);
	POR_SetRank(4);
	POR_GovMark(false);
	// a investigacao continua sem o governador
	pchar.questTemp.PorLine = "M5_godinho";
	AddQuestRecord("POR_Line", "10");
	POR_MakeLandNpc("POR_Godinho", "citiz_" + (rand(9) + 11), "Simão", "Godinho", "Caracas_tavern", "sit", "sit_base2");
}

// ---------------------------------------------------------------- missao 5: o livro de contas
void POR_M5_ToSecretary()
{
	pchar.questTemp.PorLine = "M5_secretary";
	AddQuestRecord("POR_Line", "11");
	RemoveLandQuestMark_Main(CharacterFromID("POR_Godinho"), "POR_Line");
	POR_MakeLandNpc("POR_Lourenco", "citiz_" + (rand(9) + 11), "Lourenço", "Viegas", "Cumana_tavern", "sit", "sit_base4");
}

// ---------------------------------------------------------------- missao 6: a ira do governador
void POR_M6_Start()
{
	pchar.questTemp.PorLine = "M6_go";
	AddQuestRecord("POR_Line", "12");
	RemoveLandQuestMark_Main(CharacterFromID("POR_Lourenco"), "POR_Line");
	POR_RemoveNpc("POR_Godinho");
	POR_SetLocCond("POR_M6_Battle", "Cumana");
}

void POR_M6_Battle(string qName)
{
	ref sld;
	POR_NewGroup("POR_M6", "war");
	sld = POR_AddShip("POR_M6", 1, "navy_off_spa_4", SPAIN, SHIP_CORVETTE, CANNON_TYPE_CANNON_LBS20, 4, "blade33");
	sld.name = "Mateo";
	sld.lastname = "Arrieta";
	sld.Ship.Name = "Diablo Rojo";
	pchar.questTemp.PorLine.Cmd = sld.id;
	sld = POR_AddShip("POR_M6", 2, "navy_off_spa_3", SPAIN, SHIP_BRIGANTINE, CANNON_TYPE_CANNON_LBS16, 3, "blade26");
	POR_LaunchGroup("POR_M6", pchar.questTemp.PorLine.Cmd, "Cumana");
	POR_SetGroupDeathCond("POR_M6_Win", "POR_M6");
	POR_SetMapEnterCond("POR_M6_Flee");
}

// vencendo ou escapando, o desembargador chega a Cumana
void POR_M6_RepArrives()
{
	Island_SetReloadEnableGlobal("Cumana", true);
	pchar.questTemp.PorLine = "M6_rep";
	AddQuestRecord("POR_Line", "13");
	POR_RemoveNpc("POR_Lourenco");
	POR_MakeLandNpc("POR_Rep", "huber_por0", "Álvaro", "Coutinho", "Cumana_townhall", "goto", "goto1");
}

void POR_M6_Win(string qName)
{
	DeleteQuestCondition("POR_M6_Flee");
	POR_M6_RepArrives();
}

void POR_M6_Flee(string qName)
{
	DeleteQuestCondition("POR_M6_Win");
	Group_DeleteGroup("POR_M6");
	POR_M6_RepArrives();
}

// ---------------------------------------------------------------- missao 7: justica d'El-Rei
void POR_M7_Start()
{
	ref sld;
	pchar.questTemp.PorLine = "M7_go";
	AddQuestRecord("POR_Line", "14");
	RemoveLandQuestMark_Main(CharacterFromID("POR_Rep"), "POR_Line");
	// o governador sai da residencia de Caracas e nao volta mais
	sld = CharacterFromID("por_guber");
	sld.notMoveAble = true;
	POR_GovMark(false);
	ChangeCharacterAddressGroup(sld, "none", "", "");
	sld.location = "none";
	if (CheckAttribute(sld, "CityToStay"))
	{
		int iMayor = GetCharacterIndex(sld.CityToStay + "_Mayor");
		if (iMayor >= 0)
		{
			Characters[iMayor].location.group = "sit";
			Characters[iMayor].location.locator = "sit1";
			LAi_SetHuberTypeNoGroup(&Characters[iMayor]);
		}
		DeleteAttribute(sld, "CityToStay");
	}
	POR_SetLocCond("POR_M7_Battle", "Caracas");
}

void POR_M7_Battle(string qName)
{
	ref sld;
	DeleteAttribute(pchar, "questTemp.PorLine.GovCaptured");
	POR_NewGroup("POR_M7", "war");
	sld = POR_AddShip("POR_M7", 1, "huber_spa", SPAIN, SHIP_GALEON, CANNON_TYPE_CANNON_LBS24, 5, "blade26");
	sld.name = "Francisco";
	sld.lastname = "Barreto";
	sld.Ship.Name = "São Miguel";
	sld.Dialog.Filename = POR_DLG;
	sld.Dialog.CurrentNode = "First time";
	pchar.questTemp.PorLine.Cabin = sld.id;
	pchar.questTemp.PorLine.Cmd = sld.id;
	sld = POR_AddShip("POR_M7", 2, "navy_off_spa_4", SPAIN, SHIP_CORVETTE, CANNON_TYPE_CANNON_LBS20, 4, "blade33");
	sld = POR_AddShip("POR_M7", 3, "navy_off_spa_3", SPAIN, SHIP_BRIGANTINE, CANNON_TYPE_CANNON_LBS16, 3, "blade26");
	// o traidor foge sob bandeira espanhola
	POR_LaunchGroup("POR_M7", pchar.questTemp.PorLine.Cmd, "Caracas");
	POR_SetGroupDeathCond("POR_M7_Win", "POR_M7");
	POR_SetMapEnterCond("POR_M7_Flee");
}

void POR_M7_Win(string qName)
{
	DeleteQuestCondition("POR_M7_Flee");
	Island_SetReloadEnableGlobal("Caracas", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	if (CheckAttribute(pchar, "questTemp.PorLine.GovCaptured"))
	{
		pchar.questTemp.PorLine = "M7_captured";
		AddQuestRecord("POR_Line", "15");
	}
	else
	{
		pchar.questTemp.PorLine = "M7_sunk";
		AddQuestRecord("POR_Line", "15b");
	}
	AddLandQuestMark_Main(CharacterFromID("POR_Rep"), "POR_Line");
}

void POR_M7_Flee(string qName)
{
	DeleteQuestCondition("POR_M7_Win");
	Group_DeleteGroup("POR_M7");
	Island_SetReloadEnableGlobal("Caracas", true);
	DeleteAttribute(pchar, "questTemp.PorLine.Cabin");
	AddQuestRecord("POR_Line", "f");
	POR_SetLocCond("POR_M7_Battle", "Caracas");
}

// ---------------------------------------------------------------- final
void POR_Final()
{
	RemoveLandQuestMark_Main(CharacterFromID("POR_Rep"), "POR_Line");
	POR_SetRank(5);
	Portugal_SetGovernorGeneral();
	Log_Info(StringFromKey("Diplomat_dialog_2003", GetAddress_FormTitle(PORTUGAL, 6)));
	AddMoneyToCharacter(pchar, 100000);
	ChangeCharacterNationReputation(pchar, PORTUGAL, 20);
	AddCharacterExpToSkill(pchar, "Leadership", 300);
	pchar.questTemp.PorLine = "done";
	Portugal_CheckLineEnd();
	AddQuestRecord("POR_Line", "16");
	CloseQuestHeader("POR_Line");
	POR_GG_Init();
}

// ---------------------------------------------------------------- o novo Governador-Geral
// Lourenco Viegas vira secretario em Caracas e guarda a decima mensal das colonias portuguesas
void POR_GG_Init()
{
	ref sld;
	pchar.questTemp.PorGG = true;
	pchar.questTemp.PorGG.Treasury = 0;
	pchar.questTemp.PorGG.Seat = "Caracas";
	if (GetCharacterIndex("POR_Sec") < 0)
	{
		sld = POR_MakeLandNpc("POR_Sec", "citiz_" + (rand(9) + 11), "Lourenço", "Viegas", "Caracas_townhall", "goto", POR_SecLocator("Caracas"));
		RemoveLandQuestMark_Main(sld, "POR_Line");
	}
	POR_GG_Rearm("");
}

void POR_GG_Rearm(string qName)
{
	DeleteAttribute(pchar, "quest.POR_GG_Income");
	SetFunctionTimerCondition("POR_GG_Income", 0, 1, 0, false);
}

// colonia portuguesa com residencia, que pode ser sede do governo
bool POR_GG_IsSeatColony(int iCol)
{
	if (colonies[iCol].nation == "none") return false;
	if (sti(colonies[iCol].nation) != PORTUGAL) return false;
	if (FindLocation(colonies[iCol].id + "_townhall") < 0) return false;
	return true;
}

// se a sede caiu nas maos de outra nacao, o secretario foge com o tesouro
void POR_GG_CheckSeat()
{
	int i;
	int iSeat = FindColony(pchar.questTemp.PorGG.Seat);
	if (iSeat >= 0 && POR_GG_IsSeatColony(iSeat)) return;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (!POR_GG_IsSeatColony(i)) continue;
		pchar.questTemp.PorGG.Seat = colonies[i].id;
		if (GetCharacterIndex("POR_Sec") >= 0) ChangeCharacterAddressGroup(CharacterFromID("POR_Sec"), colonies[i].id + "_townhall", "goto", POR_SecLocator(colonies[i].id));
		Log_Info(StringFromKey("Diplomat_dialog_2109", GetCityName(colonies[i].id)));
		return;
	}
}

// lucro estimado de uma colonia por mes, pelo porte do mercado
int POR_ColonyProfit(int iCol)
{
	int iProfit = 60000;
	string sSize;
	if (CheckAttribute(&colonies[iCol], "StoreNum"))
	{
		sSize = Stores[sti(colonies[iCol].StoreNum)].StoreSize;
		if (sSize == "large") iProfit = 150000;
		if (sSize == "medium") iProfit = 100000;
	}
	return iProfit;
}

void POR_GG_Income(string qName)
{
	if (!CheckAttribute(pchar, "questTemp.PorGG")) return;
	if (!CheckAttribute(pchar, "questTemp.PorGG.Seat")) pchar.questTemp.PorGG.Seat = "Caracas";
	POR_GG_CheckSeat();
	int i, iPart;
	int iSum = 0;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		if (colonies[i].nation == "none") continue;
		if (sti(colonies[i].nation) != PORTUGAL) continue;
		if (CheckAttribute(&colonies[i], "HeroOwn") && sti(colonies[i].HeroOwn) == true) continue;
		// a decima parte do lucro, com variacao de 80% a 120%
		iPart = POR_ColonyProfit(i) / 10;
		iPart = iPart * (80 + rand(40)) / 100;
		iSum = iSum + iPart;
	}
	if (iSum > 0)
	{
		// o tesouro acumula ate o Governador-Geral retirar
		pchar.questTemp.PorGG.Treasury = sti(pchar.questTemp.PorGG.Treasury) + iSum;
		Log_Info(StringFromKey("Diplomat_dialog_2102", GetCityName(pchar.questTemp.PorGG.Seat), FindMoneyString(iSum)));
	}
	DoQuestFunctionDelay("POR_GG_Rearm", 1.0);
}


// ================================================================ Frota do Brasil
// Comboio portugues (acucar e tabaco) que sai do Atlantico a cada ~90 dias rumo a Caracas
// (ou Cumana, se Caracas nao for portuguesa). Estado em pchar.questTemp.PorBF: wait | sail | lost
#define POR_BF_WAIT 90

bool POR_BF_ColonyOk(string sCol)
{
	int i = FindColony(sCol);
	if (i < 0) return false;
	if (colonies[i].nation == "none") return false;
	if (sti(colonies[i].nation) != PORTUGAL) return false;
	if (CheckAttribute(&colonies[i], "Siege")) return false;
	return true;
}

string POR_BF_Dest()
{
	if (POR_BF_ColonyOk("Caracas")) return "Caracas";
	if (POR_BF_ColonyOk("Cumana")) return "Cumana";
	return "";
}

// chamado todo dia (Portugal_FixMayorModels)
void POR_BF_Tick()
{
	POR_Suit_Tick(); // Nova Nacao 1.1.0: traje portugues em saves antigos
	if (!CheckAttribute(pchar, "questTemp.PorBF"))
	{
		pchar.questTemp.PorBF = "wait";
		pchar.questTemp.PorBF.Wait = 30;
		SaveCurrentQuestDateParam("questTemp.PorBFDate");
		return;
	}
	string sSt = pchar.questTemp.PorBF;
	if (sSt == "lost") return; // a limpeza vem ao sair para o mapa
	if (sSt == "sail")
	{
		// rede de seguranca: o comboio sumiu do mapa sem chegar nem afundar
		if (GetQuestPastDayParam("questTemp.PorBFDate") > 45) POR_BF_End("");
		return;
	}
	if (GetQuestPastDayParam("questTemp.PorBFDate") < sti(pchar.questTemp.PorBF.Wait)) return;
	string sDest = POR_BF_Dest();
	if (sDest == "")
	{
		pchar.questTemp.PorBF.Wait = 30; // tenta de novo daqui a um mes
		SaveCurrentQuestDateParam("questTemp.PorBFDate");
		return;
	}
	POR_BF_Start(sDest);
}

void POR_BF_Start(string sDest)
{
	ref chref, sld;
	int i, iChar, iSpace, iType, iNum;
	iNum = 1;
	if (CheckAttribute(pchar, "questTemp.PorBF.Num")) iNum = sti(pchar.questTemp.PorBF.Num) + 1;
	pchar.questTemp.PorBF.Num = iNum;
	string sHead = "POR_BF_Head_" + iNum;
	string sGroup = "Sea_" + sHead;
	int iRank = sti(pchar.rank) + 3;
	if (iRank < 10) iRank = 10;

	// capitania: galeao pesado
	iChar = NPC_GenerateCharacter(sHead, "off_spa_6", "man", "man", iRank, PORTUGAL, 40, true);
	makeref(chref, Characters[iChar]);
	chref.ship.type = GenerateShipExt(SHIP_GALEON_H, 1, chref);
	SetBaseShipData(chref);
	SetCrewQuantityFull(chref);
	Fantom_SetCannons(chref, "war");
	Fantom_SetBalls(chref, "pirate");
	Fantom_SetGoods(chref, "war");
	SetRandomNameToShip(chref);
	SetFantomParamHunter(chref);
	SetCaptanModelByEncType(chref, "war");
	SetRandGeraldSail(chref, PORTUGAL);
	chref.DontRansackCaptain = true;

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
	Group_SetType(sGroup, "trade");
	Group_AddCharacter(sGroup, chref.id);
	Group_SetGroupCommander(sGroup, chref.id);
	Group_LockTask(sGroup);

	// 2 escoltas e 3 mercantes com acucar e tabaco
	for (i = 1; i <= 5; i++)
	{
		iChar = NPC_GenerateCharacter(sHead + "_" + i, "off_spa_2", "man", "man", iRank - 2, PORTUGAL, 40, true);
		makeref(sld, Characters[iChar]);
		if (i <= 2)
		{
			sld.Ship.Mode = "war";
			iType = SHIP_FRIGATE;
			if (i == 2) iType = SHIP_CORVETTE;
			sld.ship.type = GenerateShipExt(iType, 1, sld);
			SetBaseShipData(sld);
			SetCrewQuantityFull(sld);
			Fantom_SetCannons(sld, "war");
			Fantom_SetBalls(sld, "pirate");
			Fantom_SetGoods(sld, "war");
			SetCaptanModelByEncType(sld, "war");
		}
		else
		{
			sld.Ship.Mode = "trade";
			iType = SHIP_GALEON;
			if (i == 4) iType = SHIP_FLEUT;
			if (i == 5) iType = SHIP_PINNACE;
			sld.ship.type = GenerateShipExt(iType, 1, sld);
			SetBaseShipData(sld);
			SetCrewQuantityFull(sld);
			Fantom_SetCannons(sld, "trade");
			Fantom_SetBalls(sld, "trade");
			iSpace = GetCharacterFreeSpace(sld, GOOD_SUGAR) / 2;
			Fantom_SetCharacterGoods(sld, GOOD_SUGAR, iSpace, 1);
			iSpace = GetCharacterFreeSpace(sld, GOOD_TOBACCO) * 2 / 3;
			Fantom_SetCharacterGoods(sld, GOOD_TOBACCO, iSpace, 1);
			SetCaptanModelByEncType(sld, "trade");
		}
		SetRandomNameToShip(sld);
		SetRandGeraldSail(sld, PORTUGAL);
		sld.DontRansackCaptain = true;
		Group_AddCharacter(sGroup, sld.id);
	}

	// no mapa: do Atlantico (a leste de Barbados e Trinidad) ate o porto
	chref.mapEnc.type = "trade";
	chref.mapEnc.worldMapShip = "portuguese_galleon"; // modelo do mod (RESOURCE/Models/WorldMap)
	chref.mapEnc.Name = StringFromKey("Diplomat_dialog_2132");
	POR_BF_Launch(sHead, sDest, 20);

	DeleteAttribute(pchar, "quest.POR_BF_Lost");
	pchar.quest.POR_BF_Lost.win_condition.l1 = "NPC_Death";
	pchar.quest.POR_BF_Lost.win_condition.l1.character = sHead;
	pchar.quest.POR_BF_Lost.function = "POR_BF_Lost";

	pchar.questTemp.PorBF = "sail";
	pchar.questTemp.PorBF.Dest = sDest;
	pchar.questTemp.PorBF.Head = sHead;
	DeleteAttribute(pchar, "questTemp.PorBF.Defended");
	DeleteAttribute(pchar, "questTemp.PorBF.Retry");
	SaveCurrentQuestDateParam("questTemp.PorBFDate");

	AddSimpleRumour(StringFromKey("Diplomat_dialog_2133", GetCityName(sDest)), PORTUGAL, 20, 3);
	AddSimpleRumour(StringFromKey("Diplomat_dialog_2133", GetCityName(sDest)), PORTUGAL + 10, 20, 2);
	if (CheckAttribute(pchar, "questTemp.PorGG")) Log_Info(StringFromKey("Diplomat_dialog_2136", GetCityName(sDest)));
}

// poe o comboio no mapa-mundi: do Atlantico (a leste de Barbados e Trinidad) ate o porto
void POR_BF_Launch(string sHead, string sDest, int iDays)
{
	string sTown = sDest + "_town";
	float fx = stf(worldMap.islands.(sDest).(sTown).position.x);
	float fz = stf(worldMap.islands.(sDest).(sTown).position.z);
	Map_CreateTraderXZ(965.0, -640.0, fx, fz, sHead, iDays);
}

// o comboio chegou ao porto (Map_TraderSucces_quest, worldmap_encgen.c)
// O mapa manda o mesmo evento quando NAO consegue criar o encontro (modelo ou ponto de partida invalido).
void POR_BF_Arrived(string sChar)
{
	if (!CheckAttribute(pchar, "questTemp.PorBF.Head")) return;
	if (pchar.questTemp.PorBF.Head != sChar) return;
	string sSt = pchar.questTemp.PorBF;
	if (sSt != "sail") return;
	string sDest = pchar.questTemp.PorBF.Dest;
	if (GetQuestPastDayParam("questTemp.PorBFDate") < 3)
	{
		// nao houve viagem: tenta de novo uma vez com o modelo espanhol; se falhar de novo, cancela sem premio
		trace("Nova Nacao: Frota do Brasil nao foi criada no mapa (" + sChar + ")");
		if (!CheckAttribute(pchar, "questTemp.PorBF.Retry"))
		{
			pchar.questTemp.PorBF.Retry = true;
			Characters[GetCharacterIndex(sChar)].mapEnc.worldMapShip = "spanish_galleon";
			POR_BF_Launch(sChar, sDest, 20);
			return;
		}
		POR_Ord_FleetGone();
		POR_BF_End("");
		return;
	}
	int iBonus;
	AddSimpleRumour(StringFromKey("Diplomat_dialog_2134", GetCityName(sDest)), PORTUGAL, 20, 3);
	if (CheckAttribute(pchar, "questTemp.PorGG"))
	{
		iBonus = 15000 + rand(15000);
		if (CheckAttribute(pchar, "questTemp.PorBF.Defended")) iBonus = iBonus + 15000;
		pchar.questTemp.PorGG.Treasury = sti(pchar.questTemp.PorGG.Treasury) + iBonus;
		Log_Info(StringFromKey("Diplomat_dialog_2137", GetCityName(sDest), FindMoneyString(iBonus)));
	}
	POR_Ord_FleetGone();
	POR_BF_End("");
}

// a capitania foi afundada ou tomada
void POR_BF_Lost(string qName)
{
	string sSt = pchar.questTemp.PorBF;
	if (sSt != "sail") return;
	pchar.questTemp.PorBF = "lost";
	Map_ReleaseQuestEncounter(pchar.questTemp.PorBF.Head);
	AddSimpleRumour(StringFromKey("Diplomat_dialog_2135"), PORTUGAL, 30, 3);
	AddSimpleRumour(StringFromKey("Diplomat_dialog_2135"), PORTUGAL + 10, 30, 3);
	POR_Ord_FleetGone();
	// o grupo so e apagado fora do combate
	POR_SetMapEnterCond("POR_BF_End");
}

void POR_BF_End(string qName)
{
	string sHead = "";
	if (CheckAttribute(pchar, "questTemp.PorBF.Head")) sHead = pchar.questTemp.PorBF.Head;
	DeleteQuestCondition("POR_BF_Lost");
	if (sHead != "")
	{
		Map_ReleaseQuestEncounter(sHead);
		Group_DeleteGroup("Sea_" + sHead);
	}
	DeleteAttribute(pchar, "questTemp.PorBF.Head");
	pchar.questTemp.PorBF = "wait";
	pchar.questTemp.PorBF.Wait = POR_BF_WAIT;
	SaveCurrentQuestDateParam("questTemp.PorBFDate");
}

// ================================================================ Traje de Nobre Portugues (suit_por)
// E o modelo 9 da lista de modelos do heroi (pchar.HeroModel): <Heroi>_P, copia do Noble Attire (modelo 8)
// em branco com dourado. So para os herois que ja tem o modelo pronto no mod.
string POR_Suit_Model()
{
	if (!CheckAttribute(pchar, "HeroModel")) return "";
	string s0 = GetSubStringByNum(pchar.HeroModel, 0);
	string s8 = GetSubStringByNum(pchar.HeroModel, 8);
	if (s8 == s0) return ""; // lista sem o modelo 8
	if (s8 == "Dan_Howk_8") return "Dan_Howk_P";
	if (s8 == "Blad_8") return "Blad_P";
	if (s8 == "Emmanuel_8") return "Emmanuel_P";
	if (s8 == "Esp_8") return "Esp_P";
	if (s8 == "Gram_8") return "Gram_P";
	if (s8 == "Pat_8") return "Pat_P";
	if (s8 == "Port_8") return "Port_P";
	return "";
}

// garante o modelo 9 na lista (a lista e gravada no save)
// chamado ao vestir o traje (characterUtilite.c): garante o modelo mesmo se o item veio de outro jeito
void POR_Suit_Equip(ref chref)
{
	if (!IsMainCharacter(chref)) return;
	string sM = POR_Suit_Model();
	if (sM == "") return;
	POR_Suit_AddModel();
	chref.model = sM;
}

void POR_Suit_AddModel()
{
	string sM = POR_Suit_Model();
	if (sM == "") return;
	if (GetSubStringByNum(pchar.HeroModel, 9) == sM) return;
	pchar.HeroModel = pchar.HeroModel + "," + sM;
}

// entregue ao aceitar a linha de missoes de Portugal (e, em saves antigos, na virada do dia)
void POR_Suit_Give()
{
	if (CheckAttribute(pchar, "questTemp.PorSuit")) return;
	if (POR_Suit_Model() == "") return;
	POR_Suit_AddModel();
	pchar.questTemp.PorSuit = true;
	AddItems(pchar, "suit_por", 1);
	Log_Info(StringFromKey("Diplomat_dialog_2165"));
}

void POR_Suit_Tick()
{
	if (CheckAttribute(pchar, "questTemp.PorSuit"))
	{
		POR_Suit_AddModel();
		return;
	}
	if (!CheckAttribute(pchar, "questTemp.PorLine")) return;
	string sSt = pchar.questTemp.PorLine;
	if (sSt == "") return;
	POR_Suit_Give();
}

// ================================================================ lugar do secretario
// Em Caracas ele fica de pe ao lado da cadeira do governador (goto6); nas outras residencias, no goto1
string POR_SecLocator(string sCol)
{
	if (sCol == "Caracas") return "goto6";
	return "goto1";
}

// saves antigos: o secretario estava no meio da sala de Caracas
void POR_GG_FixSecPos()
{
	int i = GetCharacterIndex("POR_Sec");
	if (i < 0) return;
	if (!CheckAttribute(&Characters[i], "location.locator")) return;
	if (Characters[i].location != "Caracas_townhall") return;
	if (Characters[i].location.locator != "goto1") return;
	ChangeCharacterAddressGroup(&Characters[i], "Caracas_townhall", "goto", "goto6");
}

// ================================================================ despachos do Governador-Geral
// Opcionais: o secretario oferece quando o jogador pergunta; sem prazo, sem punicao, pode desistir.
// A recompensa vai direto para o tesouro (nao e preciso voltar para receber).
// Duas trilhas independentes:
//   questTemp.PorOrd  - despachos comuns (hunt | smug), com intervalo de POR_ORD_COOLDOWN dias
//   questTemp.PorOrdF - defesa da Frota do Brasil, oferecida sempre que ha frota no mar (sem intervalo)
// Campos: .Island .Reward .Cmd .Fight .Lost
#define POR_ORD_COOLDOWN 20

// nome-base das condicoes de quest e dos grupos de cada trilha
string POR_Ord_Q(string sK)
{
	if (sK == "PorOrdF") return "POR_OrdF";
	return "POR_Ord";
}

string POR_Ord_State()
{
	if (!CheckAttribute(pchar, "questTemp.PorOrd")) return "";
	return pchar.questTemp.PorOrd;
}

bool POR_OrdF_Active()
{
	return CheckAttribute(pchar, "questTemp.PorOrdF");
}

bool POR_Ord_Ready()
{
	if (!CheckAttribute(pchar, "questTemp.PorOrdDate.control_year")) return true;
	if (GetQuestPastDayParam("questTemp.PorOrdDate") >= POR_ORD_COOLDOWN) return true;
	return false;
}

string POR_Ord_PlaceName(string sIsl)
{
	if (FindColony(sIsl) >= 0) return GetCityName(sIsl); // costa de uma colonia (Cumana, Caracas, sede do governo)
	return XI_ConvertString(sIsl);
}

string POR_Ord_RandIsland()
{
	int r = rand(4);
	if (r == 0) return "Trinidad";
	if (r == 1) return "Aruba";
	if (r == 2) return "Curacao";
	if (r == 3) return "Cumana";
	return "Caracas";
}

int POR_Ord_Tier()
{
	int r = sti(pchar.rank);
	if (r < 12) return 0;
	if (r < 22) return 1;
	return 2;
}

// ha frota no mar, ainda nao defendida
bool POR_Ord_FleetOpen()
{
	if (!CheckAttribute(pchar, "questTemp.PorBF")) return false;
	string sSt = pchar.questTemp.PorBF;
	if (sSt != "sail") return false;
	if (CheckAttribute(pchar, "questTemp.PorBF.Defended")) return false;
	return true;
}

// o despacho da frota pode ser oferecido agora (independe do intervalo e do outro despacho)
bool POR_Ord_FleetOffer()
{
	if (!POR_Ord_FleetOpen()) return false;
	if (POR_OrdF_Active()) return false;
	return true;
}

// prepara uma oferta (ainda nao aceita) e devolve o tipo
// comuns: hunt (pirata) | smug (contrabando) | priv (corsario espanhol com nome) | escort (navio do desembargador) | supply (carga para a guarnicao)
string POR_Ord_Offer(bool bFleet)
{
	int r = rand(4);
	string sType = "hunt";
	if (r == 1) sType = "smug";
	if (r == 2) sType = "priv";
	if (r == 3) sType = "escort";
	if (r == 4) sType = "supply";
	if (bFleet) sType = "fleet";
	DeleteAttribute(pchar, "questTemp.PorOrdOffer");
	pchar.questTemp.PorOrdOffer = sType;
	pchar.questTemp.PorOrdOffer.Island = POR_Ord_RandIsland();
	if (sType == "fleet") pchar.questTemp.PorOrdOffer.Island = "Trinidad";
	int iRew = 12000 + sti(pchar.rank) * 800;
	if (sType == "smug") iRew = 10000 + sti(pchar.rank) * 600;
	if (sType == "priv") iRew = 18000 + sti(pchar.rank) * 1000;
	if (sType == "escort") iRew = 14000 + sti(pchar.rank) * 800;
	if (sType == "fleet") iRew = 20000 + sti(pchar.rank) * 1000;
	if (sType == "priv") POR_Ord_PrivName();
	if (sType == "supply") POR_Ord_SupplyPick();
	pchar.questTemp.PorOrdOffer.Reward = iRew;
	if (sType == "supply") pchar.questTemp.PorOrdOffer.Reward = POR_Ord_SupplyReward();
	return sType;
}

// corsario espanhol: nome e navio sorteados
void POR_Ord_PrivName()
{
	int r = rand(4);
	string sN = "Diego"; string sL = "de Ulloa"; string sS = "San Cristóbal";
	if (r == 1) { sN = "Gaspar"; sL = "de Lerma"; sS = "Santa Margarita"; }
	if (r == 2) { sN = "Juan"; sL = "de Arteaga"; sS = "Nuestra Señora del Pilar"; }
	if (r == 3) { sN = "Rodrigo"; sL = "Pimentel"; sS = "San Telmo"; }
	if (r == 4) { sN = "Martín"; sL = "de Zubiaur"; sS = "El Halcón"; }
	pchar.questTemp.PorOrdOffer.PName = sN;
	pchar.questTemp.PorOrdOffer.PLast = sL;
	pchar.questTemp.PorOrdOffer.PShip = sS;
}

// suprimento: o que, quanto e para qual guarnicao (a sede do governo)
void POR_Ord_SupplyPick()
{
	int r = rand(2);
	int iGood = GOOD_POWDER;
	int iQty = 200 + rand(4) * 50;
	if (r == 1) { iGood = GOOD_FOOD; iQty = 300 + rand(4) * 50; }
	if (r == 2) { iGood = GOOD_MEDICAMENT; iQty = 80 + rand(4) * 20; }
	pchar.questTemp.PorOrdOffer.Good = iGood;
	pchar.questTemp.PorOrdOffer.Qty = iQty;
	pchar.questTemp.PorOrdOffer.Island = pchar.questTemp.PorGG.Seat;
}

// paga o dobro do preco base da carga, mais um tanto pelo servico
int POR_Ord_SupplyReward()
{
	int iGood = sti(pchar.questTemp.PorOrdOffer.Good);
	int iQty = sti(pchar.questTemp.PorOrdOffer.Qty);
	int iUnits = 1;
	if (CheckAttribute(&Goods[iGood], "Units")) iUnits = sti(Goods[iGood].Units);
	if (iUnits < 1) iUnits = 1;
	return 5000 + sti(pchar.rank) * 300 + sti(Goods[iGood].Cost) * iQty * 2 / iUnits;
}

string POR_Ord_GoodName(string sK)
{
	return XI_ConvertString(Goods[sti(pchar.questTemp.(sK).Good)].Name);
}

string POR_Ord_PrivFull(string sK)
{
	return pchar.questTemp.(sK).PName + " " + pchar.questTemp.(sK).PLast;
}

void POR_Ord_Accept()
{
	string sType = pchar.questTemp.PorOrdOffer;
	string sIsl = pchar.questTemp.PorOrdOffer.Island;
	string sK = "PorOrd";
	if (sType == "fleet") sK = "PorOrdF";
	aref aTo, aFrom;
	DeleteAttribute(pchar, "questTemp." + sK);
	pchar.questTemp.(sK) = sType;
	makearef(aTo, pchar.questTemp.(sK));
	makearef(aFrom, pchar.questTemp.PorOrdOffer);
	CopyAttributes(aTo, aFrom);
	pchar.questTemp.(sK) = sType;
	DeleteAttribute(pchar, "questTemp.PorOrdOffer");
	SetQuestHeader("POR_Orders");
	ReOpenQuestHeader("POR_Orders");
	AddQuestRecord("POR_Orders", sType);
	AddQuestUserData("POR_Orders", "sPlace", POR_Ord_PlaceName(sIsl));
	AddQuestUserData("POR_Orders", "sMoney", FindMoneyString(sti(pchar.questTemp.(sK).Reward)));
	if (sType == "priv") AddQuestUserData("POR_Orders", "sName", POR_Ord_PrivFull(sK));
	if (sType == "supply")
	{
		AddQuestUserData("POR_Orders", "sQty", pchar.questTemp.(sK).Qty);
		AddQuestUserData("POR_Orders", "sGood", POR_Ord_GoodName(sK));
		return; // entrega no secretario, sem combate
	}
	POR_SetLocCond(POR_Ord_Q(sK) + "_Battle", sIsl);
}

// encerra uma trilha; so a trilha comum conta o intervalo
void POR_Ord_Clear(string sK)
{
	DeleteAttribute(pchar, "questTemp." + sK);
	if (sK == "PorOrd") SaveCurrentQuestDateParam("questTemp.PorOrdDate");
	if (POR_Ord_State() != "") return;
	if (POR_OrdF_Active()) return;
	CloseQuestHeader("POR_Orders");
}

void POR_Ord_Cancel(string sK)
{
	DeleteQuestCondition(POR_Ord_Q(sK) + "_Battle");
	AddQuestRecord("POR_Orders", "cancel");
	POR_Ord_Clear(sK);
}

// a viagem da frota terminou (chegou ou afundou) sem que o jogador lutasse pelo despacho dela
void POR_Ord_FleetGone()
{
	if (!POR_OrdF_Active()) return;
	if (CheckAttribute(pchar, "questTemp.PorOrdF.Fight")) return; // combate em andamento: deixa terminar
	DeleteQuestCondition("POR_OrdF_Battle");
	AddQuestRecord("POR_Orders", "fleet_gone");
	POR_Ord_Clear("PorOrdF");
}

ref POR_Ord_Ship(string sGroup, int idx, int iNation, int iShip)
{
	string sModel = "pirate_" + (rand(15) + 1);
	if (iNation == SPAIN) sModel = "off_spa_2";
	if (iNation == PORTUGAL) sModel = "merch_" + (rand(14) + 1);
	ref sld = POR_AddShip(sGroup, idx, sModel, iNation, iShip, CANNON_TYPE_LBS_BY_SHIP, 1, "blade24");
	if (iNation == SPAIN) SetCaptanModelByEncType(sld, "war");
	SetRandomNameToShip(sld);
	return sld;
}

// condicoes de quest de cada trilha (a quest so passa o proprio nome)
void POR_Ord_Battle(string qName) { POR_Ord_DoBattle("PorOrd"); }
void POR_OrdF_Battle(string qName) { POR_Ord_DoBattle("PorOrdF"); }
void POR_Ord_Win(string qName) { POR_Ord_DoWin("PorOrd"); }
void POR_OrdF_Win(string qName) { POR_Ord_DoWin("PorOrdF"); }
void POR_Ord_Flee(string qName) { POR_Ord_DoFlee("PorOrd"); }
void POR_OrdF_Flee(string qName) { POR_Ord_DoFlee("PorOrdF"); }

void POR_Ord_DoBattle(string sK)
{
	ref sld;
	int iSpace;
	string sG = POR_Ord_Q(sK);
	string sOurs = sG + "_Ours";
	string sType = pchar.questTemp.(sK);
	string sIsl = pchar.questTemp.(sK).Island;
	int iT = POR_Ord_Tier();
	pchar.questTemp.(sK).Fight = true;
	DeleteAttribute(pchar, "questTemp." + sK + ".Lost");
	POR_NewGroup(sG, "pirate");

	if (sType == "hunt")
	{
		if (iT == 0) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_BRIGANTINE);
		if (iT == 1) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_CORVETTE_L);
		if (iT == 2) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_FRIGATE);
		pchar.questTemp.(sK).Cmd = sld.id;
		if (iT == 0) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_SCHOONER_W);
		if (iT == 1) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_BRIG);
		if (iT == 2) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_CORVETTE);
	}
	if (sType == "smug")
	{
		if (iT == 0) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_SCHOONER_W);
		if (iT == 1) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_BRIGANTINE);
		if (iT == 2) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_CORVETTE_L);
		pchar.questTemp.(sK).Cmd = sld.id;
		if (iT == 0) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_LUGGER);
		if (iT == 1) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_BARKENTINE);
		if (iT == 2) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_BRIGANTINE);
		// a carga do contrabando
		iSpace = GetCharacterFreeSpace(sld, GOOD_TOBACCO) / 2;
		Fantom_SetCharacterGoods(sld, GOOD_TOBACCO, iSpace, 1);
		iSpace = GetCharacterFreeSpace(sld, GOOD_SUGAR) / 2;
		Fantom_SetCharacterGoods(sld, GOOD_SUGAR, iSpace, 1);
	}
	if (sType == "priv")
	{
		// o corsario espanhol com nome e o seu consorte
		if (iT == 0) sld = POR_Ord_Ship(sG, 1, SPAIN, SHIP_CORVETTE);
		if (iT == 1) sld = POR_Ord_Ship(sG, 1, SPAIN, SHIP_FRIGATE);
		if (iT == 2) sld = POR_Ord_Ship(sG, 1, SPAIN, SHIP_FRIGATE_H);
		sld.name = pchar.questTemp.(sK).PName;
		sld.lastname = pchar.questTemp.(sK).PLast;
		sld.Ship.Name = pchar.questTemp.(sK).PShip;
		sld.AlwaysEnemy = true;
		pchar.questTemp.(sK).Cmd = sld.id;
		if (iT == 0) sld = POR_Ord_Ship(sG, 2, SPAIN, SHIP_BRIG);
		if (iT == 1) sld = POR_Ord_Ship(sG, 2, SPAIN, SHIP_CORVETTE_L);
		if (iT == 2) sld = POR_Ord_Ship(sG, 2, SPAIN, SHIP_CORVETTE);
		sld.AlwaysEnemy = true;
	}
	if (sType == "escort")
	{
		// o navio do desembargador (aliado)
		POR_NewGroup(sOurs, "trade");
		sld = POR_Ord_Ship(sOurs, 1, PORTUGAL, SHIP_PINNACE);
		sld.ShipEnemyDisable = true;
		sld.Abordage.Enable = false;
		Group_SetGroupCommander(sOurs, sld.id);
		pchar.quest.POR_Ord_MerchDead1.win_condition.l1 = "NPC_Death";
		pchar.quest.POR_Ord_MerchDead1.win_condition.l1.character = sld.id;
		pchar.quest.POR_Ord_MerchDead1.function = "POR_Ord_MerchDeadN";
		Group_SetAddress(sOurs, sIsl, "", "");
		// os piratas
		if (iT == 0) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_BRIGANTINE);
		if (iT == 1) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_CORVETTE_L);
		if (iT == 2) sld = POR_Ord_Ship(sG, 1, PIRATE, SHIP_FRIGATE);
		pchar.questTemp.(sK).Cmd = sld.id;
		if (iT == 0) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_SCHOONER_W);
		if (iT == 1) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_BRIG);
		if (iT == 2) sld = POR_Ord_Ship(sG, 2, PIRATE, SHIP_CORVETTE);
	}
	if (sType == "fleet")
	{
		// parte da frota (aliada)
		POR_NewGroup(sOurs, "trade");
		sld = POR_Ord_Ship(sOurs, 1, PORTUGAL, SHIP_GALEON);
		sld.ShipEnemyDisable = true;
		sld.Abordage.Enable = false;
		iSpace = GetCharacterFreeSpace(sld, GOOD_SUGAR) / 2;
		Fantom_SetCharacterGoods(sld, GOOD_SUGAR, iSpace, 1);
		Group_SetGroupCommander(sOurs, sld.id);
		pchar.quest.POR_OrdF_MerchDead1.win_condition.l1 = "NPC_Death";
		pchar.quest.POR_OrdF_MerchDead1.win_condition.l1.character = sld.id;
		pchar.quest.POR_OrdF_MerchDead1.function = "POR_Ord_MerchDead";
		sld = POR_Ord_Ship(sOurs, 2, PORTUGAL, SHIP_FLEUT);
		sld.ShipEnemyDisable = true;
		sld.Abordage.Enable = false;
		iSpace = GetCharacterFreeSpace(sld, GOOD_TOBACCO) / 2;
		Fantom_SetCharacterGoods(sld, GOOD_TOBACCO, iSpace, 1);
		pchar.quest.POR_OrdF_MerchDead2.win_condition.l1 = "NPC_Death";
		pchar.quest.POR_OrdF_MerchDead2.win_condition.l1.character = sld.id;
		pchar.quest.POR_OrdF_MerchDead2.function = "POR_Ord_MerchDead";
		Group_SetAddress(sOurs, sIsl, "", "");
		// os corsarios espanhois
		if (iT == 0) sld = POR_Ord_Ship(sG, 1, SPAIN, SHIP_CORVETTE_L);
		if (iT == 1) sld = POR_Ord_Ship(sG, 1, SPAIN, SHIP_CORVETTE);
		if (iT == 2) sld = POR_Ord_Ship(sG, 1, SPAIN, SHIP_FRIGATE);
		sld.AlwaysEnemy = true;
		pchar.questTemp.(sK).Cmd = sld.id;
		sld = POR_Ord_Ship(sG, 2, SPAIN, SHIP_BRIG);
		sld.AlwaysEnemy = true;
		if (iT == 2)
		{
			sld = POR_Ord_Ship(sG, 3, SPAIN, SHIP_CORVETTE);
			sld.AlwaysEnemy = true;
		}
	}

	POR_LaunchGroup(sG, pchar.questTemp.(sK).Cmd, sIsl);
	if (POR_Ord_HasOurs(sType))
	{
		Group_UnlockTask(sG); // os corsarios vao atras dos mercantes
		Group_SetTaskAttack(sG, sOurs);
		Group_SetTaskAttack(sOurs, sG);
		Group_LockTask(sOurs);
	}
	POR_SetGroupDeathCond(sG + "_Win", sG);
	POR_SetMapEnterCond(sG + "_Flee");
}

// despachos com navio aliado para proteger
bool POR_Ord_HasOurs(string sType)
{
	if (sType == "fleet") return true;
	if (sType == "escort") return true;
	return false;
}

void POR_Ord_MerchDead(string qName)
{
	pchar.questTemp.PorOrdF.Lost = true;
}

void POR_Ord_MerchDeadN(string qName)
{
	pchar.questTemp.PorOrd.Lost = true;
}

void POR_Ord_DoWin(string sK)
{
	string sG = POR_Ord_Q(sK);
	string sType = pchar.questTemp.(sK);
	string sIsl = pchar.questTemp.(sK).Island;
	int iRew = sti(pchar.questTemp.(sK).Reward);
	int iRep = 3;
	string sRec = "done";
	DeleteQuestCondition(sG + "_Flee");
	DeleteQuestCondition(sG + "_MerchDead1");
	DeleteQuestCondition(sG + "_MerchDead2");
	if (sType == "escort")
	{
		Group_DeleteAtEnd(sG + "_Ours");
		if (CheckAttribute(pchar, "questTemp.PorOrd.Lost"))
		{
			iRew = iRew / 2;
			iRep = 1;
			sRec = "done_lost_escort";
		}
	}
	if (sType == "fleet")
	{
		Group_DeleteAtEnd(sG + "_Ours");
		iRep = 5;
		if (POR_Ord_FleetOpen()) pchar.questTemp.PorBF.Defended = true;
		if (CheckAttribute(pchar, "questTemp.PorOrdF.Lost"))
		{
			iRew = iRew / 2;
			iRep = 2;
			sRec = "done_loss";
		}
	}
	Island_SetReloadEnableGlobal(sIsl, true);
	if (CheckAttribute(pchar, "questTemp.PorGG.Treasury")) pchar.questTemp.PorGG.Treasury = sti(pchar.questTemp.PorGG.Treasury) + iRew;
	else AddMoneyToCharacter(pchar, iRew);
	ChangeCharacterNationReputation(pchar, PORTUGAL, iRep);
	AddCharacterExpToSkill(pchar, "Leadership", 80);
	AddCharacterExpToSkill(pchar, "Accuracy", 80);
	AddCharacterExpToSkill(pchar, "Sailing", 80);
	AddQuestRecord("POR_Orders", sRec);
	AddQuestUserData("POR_Orders", "sMoney", FindMoneyString(iRew));
	Log_Info(StringFromKey("Diplomat_dialog_2151", FindMoneyString(iRew)));
	// presentes da primeira vez
	if (sType == "fleet")
	{
		if (!CheckAttribute(pchar, "questTemp.PorGG.GotAstrolabe"))
		{
			pchar.questTemp.PorGG.GotAstrolabe = true;
			AddItems(pchar, "por_astrolabe", 1);
			Log_Info(StringFromKey("Diplomat_dialog_2163"));
		}
	}
	if (sType == "priv")
	{
		if (!CheckAttribute(pchar, "questTemp.PorGG.GotSphere"))
		{
			pchar.questTemp.PorGG.GotSphere = true;
			AddItems(pchar, "por_sphere", 1);
			Log_Info(StringFromKey("Diplomat_dialog_2164"));
		}
	}
	POR_Ord_Count();
	POR_Ord_Clear(sK);
}

// ---------------------------------------------------------------- marcos (habito e Padre Eterno)
void POR_Ord_Count()
{
	int n = 0;
	if (CheckAttribute(pchar, "questTemp.PorGG.OrdDone")) n = sti(pchar.questTemp.PorGG.OrdDone);
	pchar.questTemp.PorGG.OrdDone = n + 1;
}

// presente esperando no secretario: "habit" | "ship" | ""
string POR_Ord_Gift()
{
	if (!CheckAttribute(pchar, "questTemp.PorGG.OrdDone")) return "";
	int n = sti(pchar.questTemp.PorGG.OrdDone);
	if (n >= 3)
	{
		if (!CheckAttribute(pchar, "questTemp.PorGG.GotHabit")) return "habit";
	}
	if (n >= 6)
	{
		if (!CheckAttribute(pchar, "questTemp.PorGG.GotShip")) return "ship";
	}
	return "";
}

void POR_GG_GiveHabit()
{
	pchar.questTemp.PorGG.GotHabit = true;
	AddItems(pchar, "por_habit", 1);
	ChangeCharacterNationReputation(pchar, PORTUGAL, 5);
}

// o galeao Padre Eterno vai para as docas da sede do governo (guarda gratuita); false se as docas estao cheias
bool POR_GG_StoreShip()
{
	int iPM = GetCharacterIndex(pchar.questTemp.PorGG.Seat + "_PortMan");
	if (iPM < 0) return false;
	ref pm = &Characters[iPM];
	if (!CheckAttribute(pm, "Portman")) pm.Portman = 0;
	int nMax = 3;
	if (CheckAttribute(pm, "storeShipsNum")) nMax = sti(pm.storeShipsNum);
	if (sti(pm.Portman) >= nMax) return false;

	ref sld = GetCharacter(NPC_GenerateCharacter("POR_PadreEterno_Tmp", "off_spa_2", "man", "man", 10, PORTUGAL, 1, false));
	sld.ship.type = GenerateShipExt(SHIP_GALEON_H, 1, sld);
	UpgradeShipParameter(sld, "HP");
	UpgradeShipParameter(sld, "Capacity");
	UpgradeShipParameter(sld, "MaxCrew");
	UpgradeShipParameter(sld, "Cannons");
	UpgradeShipParameter(sld, "TurnRate");
	SetBaseShipData(sld);
	sld.Ship.Name = "Padre Eterno";
	SetCrewQuantity(sld, 0);
	SetGoodsInitNull(sld);

	string sKey = pm.id + "0_" + sld.ship.type;
	RealShips[sti(sld.ship.type)].StoreShip = true;
	aref aTo, aFrom;
	makearef(aTo, pm.ShipInStockMan.(sKey).ship);
	makearef(aFrom, sld.ship);
	CopyAttributes(aTo, aFrom);
	pm.ShipInStockMan.(sKey).MoneyForShip = 0;
	pm.ShipInStockMan.(sKey).AltDate = GetQuestBookDataDigit();
	SaveCurrentNpcQuestDateParam(pm, "ShipInStockMan." + sKey + ".Date");
	pm.Portman = sti(pm.Portman) + 1;
	if (!CheckAttribute(pchar, "ShipInStock")) pchar.ShipInStock = 0;
	pchar.ShipInStock = sti(pchar.ShipInStock) + 1;

	sld.ship.type = SHIP_NOTUSED;
	sld.lifeDay = 0;
	pchar.questTemp.PorGG.GotShip = true;
	return true;
}

// ---------------------------------------------------------------- suprimentos (entregues no secretario)
bool POR_Ord_SupplyReady()
{
	if (POR_Ord_State() != "supply") return false;
	if (GetSquadronGoods(pchar, sti(pchar.questTemp.PorOrd.Good)) < sti(pchar.questTemp.PorOrd.Qty)) return false;
	return true;
}

void POR_Ord_SupplyDeliver()
{
	int iRew = sti(pchar.questTemp.PorOrd.Reward);
	RemoveCharacterGoods(pchar, sti(pchar.questTemp.PorOrd.Good), sti(pchar.questTemp.PorOrd.Qty));
	if (CheckAttribute(pchar, "questTemp.PorGG.Treasury")) pchar.questTemp.PorGG.Treasury = sti(pchar.questTemp.PorGG.Treasury) + iRew;
	else AddMoneyToCharacter(pchar, iRew);
	ChangeCharacterNationReputation(pchar, PORTUGAL, 3);
	AddCharacterExpToSkill(pchar, "Commerce", 120);
	AddQuestRecord("POR_Orders", "supplied");
	AddQuestUserData("POR_Orders", "sMoney", FindMoneyString(iRew));
	POR_Ord_Count();
	POR_Ord_Clear("PorOrd");
}

void POR_Ord_DoFlee(string sK)
{
	string sG = POR_Ord_Q(sK);
	string sIsl = pchar.questTemp.(sK).Island;
	DeleteQuestCondition(sG + "_Win");
	Group_DeleteGroup(sG);
	DeleteQuestCondition(sG + "_MerchDead1");
	DeleteQuestCondition(sG + "_MerchDead2");
	Group_DeleteGroup(sG + "_Ours");
	Island_SetReloadEnableGlobal(sIsl, true);
	DeleteAttribute(pchar, "questTemp." + sK + ".Fight");
	AddQuestRecord("POR_Orders", "flee");
	// o despacho da frota acaba se ela ja terminou a viagem; os outros esperam o jogador voltar
	if (sK == "PorOrdF")
	{
		if (!POR_Ord_FleetOpen())
		{
			POR_Ord_FleetGone();
			return;
		}
	}
	POR_SetLocCond(sG + "_Battle", sIsl);
}

// saves que aceitaram a defesa da frota na versao de teste anterior (trilha unica)
void POR_Ord_Migrate()
{
	if (POR_Ord_State() != "fleet") return;
	if (CheckAttribute(pchar, "questTemp.PorOrd.Fight")) return;
	DeleteQuestCondition("POR_Ord_Battle");
	pchar.questTemp.PorOrdF = "fleet";
	pchar.questTemp.PorOrdF.Island = pchar.questTemp.PorOrd.Island;
	pchar.questTemp.PorOrdF.Reward = pchar.questTemp.PorOrd.Reward;
	DeleteAttribute(pchar, "questTemp.PorOrd");
	POR_SetLocCond("POR_OrdF_Battle", pchar.questTemp.PorOrdF.Island);
}
