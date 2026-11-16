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
	Log_Info(StringFromKey("POR_Line_dialog_3", GetAddress_FormTitle(PORTUGAL, iRank)));
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
	ChangeCharacterAddressGroup(sld, sLoc, sGroup, sLocator);
	LAi_SetStayTypeNoGroup(sld);
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
	SetQuestHeader("POR_Line");
	AddQuestRecord("POR_Line", "1");
	POR_GovMark(false);
	POR_SetLocCond("POR_M1_Battle", "Trinidad");
}

void POR_M1_Battle(string qName)
{
	ref sld;
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
	AddQuestRecord("POR_Line", "2");
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
	POR_MakeLandNpc("POR_Tome", "citiz_" + (rand(9) + 11), "Tomé", "Ferraz", "Villemstad_tavern", "goto", "goto1");
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
	AddQuestRecord("POR_Line", "5");
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
	sld = POR_AddShip("POR_M3_Ours", 1, "citiz_" + (rand(9) + 11), PORTUGAL, SHIP_FLEUT, CANNON_TYPE_CANNON_LBS12, 0, "blade24");
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
		POR_MakeLandNpc("POR_Amaro", "citiz_" + (rand(9) + 11), "Amaro", "Teles", "Cumana_tavern", "goto", "goto1");
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
	AddQuestRecord("POR_Line", "9");
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
	POR_MakeLandNpc("POR_Godinho", "citiz_" + (rand(9) + 11), "Simão", "Godinho", "Caracas_tavern", "goto", "goto1");
}

// ---------------------------------------------------------------- missao 5: o livro de contas
void POR_M5_ToSecretary()
{
	pchar.questTemp.PorLine = "M5_secretary";
	AddQuestRecord("POR_Line", "11");
	RemoveLandQuestMark_Main(CharacterFromID("POR_Godinho"), "POR_Line");
	POR_MakeLandNpc("POR_Lourenco", "citiz_" + (rand(9) + 11), "Lourenço", "Viegas", "Cumana_tavern", "goto", "goto1");
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
	POR_MakeLandNpc("POR_Rep", "huber_0", "Álvaro", "Coutinho", "Cumana_townhall", "goto", "goto1");
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
	Log_Info(StringFromKey("POR_Line_dialog_3", GetAddress_FormTitle(PORTUGAL, 6)));
	AddMoneyToCharacter(pchar, 100000);
	ChangeCharacterNationReputation(pchar, PORTUGAL, 20);
	AddCharacterExpToSkill(pchar, "Leadership", 300);
	pchar.questTemp.PorLine = "done";
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
		sld = POR_MakeLandNpc("POR_Sec", "citiz_" + (rand(9) + 11), "Lourenço", "Viegas", "Caracas_townhall", "goto", "goto1");
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
		if (GetCharacterIndex("POR_Sec") >= 0) ChangeCharacterAddressGroup(CharacterFromID("POR_Sec"), colonies[i].id + "_townhall", "goto", "goto1");
		Log_Info(StringFromKey("POR_Line_dialog_109", GetCityName(colonies[i].id)));
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
		Log_Info(StringFromKey("POR_Line_dialog_102", GetCityName(pchar.questTemp.PorGG.Seat), FindMoneyString(iSum)));
	}
	DoQuestFunctionDelay("POR_GG_Rearm", 1.0);
}
