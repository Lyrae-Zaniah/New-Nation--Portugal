// Nova Nacao: personagens da linha de missoes de Portugal ("Pela Gloria da Coroa")
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;
	string sId, sSt, sCab, sAttr;
	int iTreasury, i, n;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	// escolha da nova sede do governo (SecMove_<indice da colonia>)
	string attrLoc = Dialog.CurrentNode;
	if (HasSubStr(attrLoc, "SecMove_"))
	{
		i = findsubstr(attrLoc, "_", 0);
		NPChar.quest.SeatIdx = strcut(attrLoc, i + 1, strlen(attrLoc) - 1);
		Dialog.CurrentNode = "Sec_Move_Do";
	}

	switch (Dialog.CurrentNode)
	{
		case "First time":
			sId = NPChar.id;
			if (CheckAttribute(NPChar, "CaptanId")) sId = NPChar.CaptanId;
			sSt = "";
			if (CheckAttribute(pchar, "questTemp.PorLine")) sSt = pchar.questTemp.PorLine;
			sCab = "-";
			if (CheckAttribute(pchar, "questTemp.PorLine.Cabin")) sCab = pchar.questTemp.PorLine.Cabin;

			dialog.text = StringFromKey("Diplomat_dialog_2001");
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";

			// capitaes na cabine (abordagem)
			if (sId == sCab && sSt == "M1_go")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2005");
				link.l1 = StringFromKey("Diplomat_dialog_2006");
				pchar.questTemp.PorLine.M1Boarded = true;
				link.l1.go = "M1_C2";
			}
			if (sId == sCab && sSt == "M2_aruba")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2030");
				link.l1 = StringFromKey("Diplomat_dialog_2031");
				pchar.questTemp.PorLine.M2Boarded = true;
				link.l1.go = "M2_C2";
			}
			if (sId == sCab && sSt == "M4_go")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2042");
				link.l1 = StringFromKey("Diplomat_dialog_2043");
				link.l1.go = "M4_C2";
			}
			if (sId == sCab && sSt == "M7_go")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2091");
				link.l1 = StringFromKey("Diplomat_dialog_2092");
				link.l1.go = "M7_C2";
			}
			// personagens em terra
			if (sId == "POR_Tome")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2029");
				link.l1 = StringFromKey("Diplomat_dialog_2002");
				link.l1.go = "exit";
				if (sSt == "M2_go")
				{
					if (CheckAttribute(pchar, "questTemp.PorLine.TomeOpen"))
					{
						dialog.text = StringFromKey("Diplomat_dialog_2020");
						link.l1 = StringFromKey("Diplomat_dialog_2021");
						link.l1.go = "Tome_Pay";
						link.l2 = StringFromKey("Diplomat_dialog_2022");
						link.l2.go = "Tome_NoMoney";
					}
					else
					{
						dialog.text = StringFromKey("Diplomat_dialog_2014");
						link.l1 = StringFromKey("Diplomat_dialog_2015");
						link.l1.go = "Tome_Gov";
						link.l2 = StringFromKey("Diplomat_dialog_2017");
						link.l2.go = "Tome_Bento";
						link.l3 = StringFromKey("Diplomat_dialog_2002");
						link.l3.go = "exit";
					}
				}
			}
			if (sId == "POR_Amaro")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2041");
				link.l1 = StringFromKey("Diplomat_dialog_2002");
				link.l1.go = "exit";
				if (sSt == "M3_amaro")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2036");
					link.l1 = StringFromKey("Diplomat_dialog_2037");
					link.l1.go = "Amaro_2";
				}
			}
			if (sId == "POR_Godinho")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2060");
				link.l1 = StringFromKey("Diplomat_dialog_2002");
				link.l1.go = "exit";
				if (sSt == "M5_godinho")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2052");
					link.l1 = StringFromKey("Diplomat_dialog_2053");
					link.l1.go = "God_2";
					link.l2 = StringFromKey("Diplomat_dialog_2054");
					link.l2.go = "God_Deny";
				}
			}
			if (sId == "POR_Lourenco")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2072");
				link.l1 = StringFromKey("Diplomat_dialog_2002");
				link.l1.go = "exit";
				if (sSt == "M5_secretary")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2061");
					link.l1 = StringFromKey("Diplomat_dialog_2062");
					link.l1.go = "Lou_2";
				}
			}
			if (sId == "POR_Sec")
			{
				iTreasury = 0;
				if (CheckAttribute(pchar, "questTemp.PorGG.Treasury")) iTreasury = sti(pchar.questTemp.PorGG.Treasury);
				dialog.text = StringFromKey("Diplomat_dialog_2099");
				link.l1 = StringFromKey("Diplomat_dialog_2104");
				link.l1.go = "Sec_Move";
				link.l2 = StringFromKey("Diplomat_dialog_2002");
				link.l2.go = "exit";
				if (iTreasury > 0)
				{
					dialog.text = StringFromKey("Diplomat_dialog_2098", FindMoneyString(iTreasury));
					link.l1 = StringFromKey("Diplomat_dialog_2100");
					link.l1.go = "Sec_Take";
					link.l2 = StringFromKey("Diplomat_dialog_2104");
					link.l2.go = "Sec_Move";
					link.l3 = StringFromKey("Diplomat_dialog_2002");
					link.l3.go = "exit";
				}
			}
			if (sId == "POR_Rep")
			{
				dialog.text = StringFromKey("Diplomat_dialog_2090");
				link.l1 = StringFromKey("Diplomat_dialog_2002");
				link.l1.go = "exit";
				if (sSt == "M6_rep")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2073");
					link.l1 = StringFromKey("Diplomat_dialog_2074");
					link.l1.go = "Rep_2";
				}
				if (sSt == "M7_go")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2082");
					link.l1 = StringFromKey("Diplomat_dialog_2081");
					link.l1.go = "exit";
				}
				if (sSt == "M7_captured")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2083");
					link.l1 = StringFromKey("Diplomat_dialog_2084");
					link.l1.go = "Rep_5";
				}
				if (sSt == "M7_sunk")
				{
					dialog.text = StringFromKey("Diplomat_dialog_2083");
					link.l1 = StringFromKey("Diplomat_dialog_2085");
					link.l1.go = "Rep_5";
				}
			}
			NextDiag.TempNode = "First time";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			if (CheckAttribute(NPChar, "PorMoveTo"))
			{
				ChangeCharacterAddressGroup(NPChar, NPChar.PorMoveTo + "_townhall", "goto", "goto1");
				DeleteAttribute(NPChar, "PorMoveTo");
			}
		break;

		case "Cabin_Fight":
			LAi_SetCurHPMax(NPChar);
			QuestAboardCabinDialogExitWithBattle("");
			DialogExit();
		break;

		case "Cabin_Surrender":
			DialogExit();
			QuestAboardCabinDialogSurrender();
		break;

		// ---- missao 1: Bento Malheiro
		case "M1_C2":
			dialog.text = StringFromKey("Diplomat_dialog_2007");
			link.l1 = StringFromKey("Diplomat_dialog_2008");
			link.l1.go = "M1_C3";
		break;

		case "M1_C3":
			dialog.text = StringFromKey("Diplomat_dialog_2009");
			link.l1 = StringFromKey("Diplomat_dialog_2010");
			link.l1.go = "M1_C4";
		break;

		case "M1_C4":
			dialog.text = StringFromKey("Diplomat_dialog_2011");
			link.l1 = StringFromKey("Diplomat_dialog_2012");
			link.l1.go = "M1_C5";
		break;

		case "M1_C5":
			dialog.text = StringFromKey("Diplomat_dialog_2013");
			link.l1 = StringFromKey("Diplomat_dialog_2004");
			link.l1.go = "Cabin_Fight";
		break;

		// ---- missao 2: Tome Ferraz
		case "Tome_Gov":
			dialog.text = StringFromKey("Diplomat_dialog_2016");
			link.l1 = StringFromKey("Diplomat_dialog_2017");
			link.l1.go = "Tome_Bento";
			link.l2 = StringFromKey("Diplomat_dialog_2002");
			link.l2.go = "exit";
		break;

		case "Tome_Bento":
			dialog.text = StringFromKey("Diplomat_dialog_2018");
			link.l1 = StringFromKey("Diplomat_dialog_2019");
			link.l1.go = "Tome_Price";
			pchar.questTemp.PorLine.TomeOpen = true;
		break;

		case "Tome_Price":
			dialog.text = StringFromKey("Diplomat_dialog_2020");
			link.l1 = StringFromKey("Diplomat_dialog_2021");
			link.l1.go = "Tome_Pay";
			link.l2 = StringFromKey("Diplomat_dialog_2022");
			link.l2.go = "Tome_NoMoney";
		break;

		case "Tome_NoMoney":
			dialog.text = StringFromKey("Diplomat_dialog_2023");
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";
		break;

		case "Tome_Pay":
			if (sti(pchar.money) < 3000)
			{
				dialog.text = StringFromKey("Diplomat_dialog_2023");
				link.l1 = StringFromKey("Diplomat_dialog_2002");
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -3000);
			dialog.text = StringFromKey("Diplomat_dialog_2024");
			link.l1 = StringFromKey("Diplomat_dialog_2025");
			link.l1.go = "Tome_3";
		break;

		case "Tome_3":
			dialog.text = StringFromKey("Diplomat_dialog_2026");
			link.l1 = StringFromKey("Diplomat_dialog_2027");
			link.l1.go = "Tome_4";
		break;

		case "Tome_4":
			dialog.text = StringFromKey("Diplomat_dialog_2028");
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";
			POR_M2_ToAruba();
		break;

		// ---- missao 2: correio espanhol
		case "M2_C2":
			dialog.text = StringFromKey("Diplomat_dialog_2032");
			link.l1 = StringFromKey("Diplomat_dialog_2033");
			link.l1.go = "M2_C3";
		break;

		case "M2_C3":
			dialog.text = StringFromKey("Diplomat_dialog_2034");
			link.l1 = StringFromKey("Diplomat_dialog_2035");
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.Letters = true;
		break;

		// ---- missao 3: Amaro Teles
		case "Amaro_2":
			dialog.text = StringFromKey("Diplomat_dialog_2038");
			link.l1 = StringFromKey("Diplomat_dialog_2039");
			link.l1.go = "Amaro_3";
		break;

		case "Amaro_3":
			dialog.text = StringFromKey("Diplomat_dialog_2040");
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";
			POR_M3_AmaroDone();
		break;

		// ---- missao 4: Gaspar Roiz
		case "M4_C2":
			pchar.questTemp.PorLine.RoizBoarded = true;
			dialog.text = StringFromKey("Diplomat_dialog_2044");
			link.l1 = StringFromKey("Diplomat_dialog_2045");
			link.l1.go = "M4_C3";
		break;

		case "M4_C3":
			dialog.text = StringFromKey("Diplomat_dialog_2046");
			link.l1 = StringFromKey("Diplomat_dialog_2047");
			link.l1.go = "M4_Deal";
			link.l2 = StringFromKey("Diplomat_dialog_2048");
			link.l2.go = "M4_NoDeal";
		break;

		case "M4_Deal":
			dialog.text = StringFromKey("Diplomat_dialog_2049");
			link.l1 = StringFromKey("Diplomat_dialog_2051");
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.RoizAlive = true;
		break;

		case "M4_NoDeal":
			dialog.text = StringFromKey("Diplomat_dialog_2050");
			link.l1 = StringFromKey("Diplomat_dialog_2004");
			link.l1.go = "Cabin_Fight";
		break;

		// ---- missao 5: Simao Godinho
		case "God_Deny":
			dialog.text = StringFromKey("Diplomat_dialog_2055");
			link.l1 = StringFromKey("Diplomat_dialog_2056");
			link.l1.go = "God_2";
			link.l2 = StringFromKey("Diplomat_dialog_2053");
			link.l2.go = "God_2";
		break;

		case "God_2":
			dialog.text = StringFromKey("Diplomat_dialog_2057");
			link.l1 = StringFromKey("Diplomat_dialog_2058");
			link.l1.go = "God_3";
		break;

		case "God_3":
			dialog.text = StringFromKey("Diplomat_dialog_2059");
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";
			POR_M5_ToSecretary();
		break;

		// ---- missao 5/6: Lourenco Viegas
		case "Lou_2":
			dialog.text = StringFromKey("Diplomat_dialog_2063");
			link.l1 = StringFromKey("Diplomat_dialog_2064");
			link.l1.go = "Lou_3";
			link.l2 = StringFromKey("Diplomat_dialog_2065");
			link.l2.go = "Lou_3";
		break;

		case "Lou_3":
			dialog.text = StringFromKey("Diplomat_dialog_2066");
			link.l1 = StringFromKey("Diplomat_dialog_2067");
			link.l1.go = "Lou_4";
		break;

		case "Lou_4":
			dialog.text = StringFromKey("Diplomat_dialog_2068");
			link.l1 = StringFromKey("Diplomat_dialog_2069");
			link.l1.go = "Lou_5";
		break;

		case "Lou_5":
			dialog.text = StringFromKey("Diplomat_dialog_2070");
			link.l1 = StringFromKey("Diplomat_dialog_2071");
			link.l1.go = "exit";
			POR_M6_Start();
		break;

		// ---- Lourenco, secretario do novo Governador-Geral
		case "Sec_Take":
			iTreasury = sti(pchar.questTemp.PorGG.Treasury);
			AddMoneyToCharacter(pchar, iTreasury);
			pchar.questTemp.PorGG.Treasury = 0;
			dialog.text = StringFromKey("Diplomat_dialog_2101");
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";
		break;

		case "Sec_Move":
			dialog.text = StringFromKey("Diplomat_dialog_2106");
			n = 0;
			for (i = 0; i < MAX_COLONIES; i++)
			{
				if (n >= 8) continue;
				if (!POR_GG_IsSeatColony(i)) continue;
				if (colonies[i].id == pchar.questTemp.PorGG.Seat) continue;
				n = n + 1;
				sAttr = "l" + n;
				link.(sAttr) = GetCityName(colonies[i].id);
				link.(sAttr).go = "SecMove_" + i;
			}
			if (n > 0) dialog.text = StringFromKey("Diplomat_dialog_2105");
			link.l9 = StringFromKey("Diplomat_dialog_2107");
			link.l9.go = "exit";
		break;

		case "Sec_Move_Do":
			i = sti(NPChar.quest.SeatIdx);
			pchar.questTemp.PorGG.Seat = colonies[i].id;
			NPChar.PorMoveTo = colonies[i].id;
			dialog.text = StringFromKey("Diplomat_dialog_2108", GetCityName(colonies[i].id));
			link.l1 = StringFromKey("Diplomat_dialog_2002");
			link.l1.go = "exit";
		break;

		// ---- missao 6/7: Representante da Coroa
		case "Rep_2":
			dialog.text = StringFromKey("Diplomat_dialog_2075");
			link.l1 = StringFromKey("Diplomat_dialog_2076");
			link.l1.go = "Rep_3";
		break;

		case "Rep_3":
			dialog.text = StringFromKey("Diplomat_dialog_2077");
			if (CheckAttribute(pchar, "questTemp.PorLine.LettersKept")) dialog.text = StringFromKey("Diplomat_dialog_2077") + " " + StringFromKey("Diplomat_dialog_2078");
			link.l1 = StringFromKey("Diplomat_dialog_2079");
			link.l1.go = "Rep_4";
			POR_SetRank(5);
		break;

		case "Rep_4":
			dialog.text = StringFromKey("Diplomat_dialog_2080");
			link.l1 = StringFromKey("Diplomat_dialog_2081");
			link.l1.go = "exit";
			POR_M7_Start();
		break;

		case "Rep_5":
			dialog.text = StringFromKey("Diplomat_dialog_2086");
			link.l1 = StringFromKey("Diplomat_dialog_2087");
			link.l1.go = "Rep_6";
		break;

		case "Rep_6":
			dialog.text = StringFromKey("Diplomat_dialog_2088");
			link.l1 = StringFromKey("Diplomat_dialog_2089");
			link.l1.go = "exit";
			POR_Final();
		break;

		// ---- missao 7: o governador traidor
		case "M7_C2":
			dialog.text = StringFromKey("Diplomat_dialog_2093");
			link.l1 = StringFromKey("Diplomat_dialog_2094");
			link.l1.go = "M7_C3";
		break;

		case "M7_C3":
			dialog.text = StringFromKey("Diplomat_dialog_2095");
			link.l1 = StringFromKey("Diplomat_dialog_2096");
			link.l1.go = "M7_C4";
		break;

		case "M7_C4":
			dialog.text = StringFromKey("Diplomat_dialog_2097");
			link.l1 = StringFromKey("Diplomat_dialog_2051");
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.GovCaptured = true;
		break;
	}
}
