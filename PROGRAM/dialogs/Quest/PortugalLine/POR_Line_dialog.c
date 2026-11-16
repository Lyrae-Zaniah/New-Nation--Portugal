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

			dialog.text = StringFromKey("POR_Line_dialog_1");
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";

			// capitaes na cabine (abordagem)
			if (sId == sCab && sSt == "M1_go")
			{
				dialog.text = StringFromKey("POR_Line_dialog_5");
				link.l1 = StringFromKey("POR_Line_dialog_6");
				link.l1.go = "M1_C2";
			}
			if (sId == sCab && sSt == "M2_aruba")
			{
				dialog.text = StringFromKey("POR_Line_dialog_30");
				link.l1 = StringFromKey("POR_Line_dialog_31");
				link.l1.go = "M2_C2";
			}
			if (sId == sCab && sSt == "M4_go")
			{
				dialog.text = StringFromKey("POR_Line_dialog_42");
				link.l1 = StringFromKey("POR_Line_dialog_43");
				link.l1.go = "M4_C2";
			}
			if (sId == sCab && sSt == "M7_go")
			{
				dialog.text = StringFromKey("POR_Line_dialog_91");
				link.l1 = StringFromKey("POR_Line_dialog_92");
				link.l1.go = "M7_C2";
			}
			// personagens em terra
			if (sId == "POR_Tome")
			{
				dialog.text = StringFromKey("POR_Line_dialog_29");
				link.l1 = StringFromKey("POR_Line_dialog_2");
				link.l1.go = "exit";
				if (sSt == "M2_go")
				{
					if (CheckAttribute(pchar, "questTemp.PorLine.TomeOpen"))
					{
						dialog.text = StringFromKey("POR_Line_dialog_20");
						link.l1 = StringFromKey("POR_Line_dialog_21");
						link.l1.go = "Tome_Pay";
						link.l2 = StringFromKey("POR_Line_dialog_22");
						link.l2.go = "Tome_NoMoney";
					}
					else
					{
						dialog.text = StringFromKey("POR_Line_dialog_14");
						link.l1 = StringFromKey("POR_Line_dialog_15");
						link.l1.go = "Tome_Gov";
						link.l2 = StringFromKey("POR_Line_dialog_17");
						link.l2.go = "Tome_Bento";
						link.l3 = StringFromKey("POR_Line_dialog_2");
						link.l3.go = "exit";
					}
				}
			}
			if (sId == "POR_Amaro")
			{
				dialog.text = StringFromKey("POR_Line_dialog_41");
				link.l1 = StringFromKey("POR_Line_dialog_2");
				link.l1.go = "exit";
				if (sSt == "M3_amaro")
				{
					dialog.text = StringFromKey("POR_Line_dialog_36");
					link.l1 = StringFromKey("POR_Line_dialog_37");
					link.l1.go = "Amaro_2";
				}
			}
			if (sId == "POR_Godinho")
			{
				dialog.text = StringFromKey("POR_Line_dialog_60");
				link.l1 = StringFromKey("POR_Line_dialog_2");
				link.l1.go = "exit";
				if (sSt == "M5_godinho")
				{
					dialog.text = StringFromKey("POR_Line_dialog_52");
					link.l1 = StringFromKey("POR_Line_dialog_53");
					link.l1.go = "God_2";
					link.l2 = StringFromKey("POR_Line_dialog_54");
					link.l2.go = "God_Deny";
				}
			}
			if (sId == "POR_Lourenco")
			{
				dialog.text = StringFromKey("POR_Line_dialog_72");
				link.l1 = StringFromKey("POR_Line_dialog_2");
				link.l1.go = "exit";
				if (sSt == "M5_secretary")
				{
					dialog.text = StringFromKey("POR_Line_dialog_61");
					link.l1 = StringFromKey("POR_Line_dialog_62");
					link.l1.go = "Lou_2";
				}
			}
			if (sId == "POR_Sec")
			{
				iTreasury = 0;
				if (CheckAttribute(pchar, "questTemp.PorGG.Treasury")) iTreasury = sti(pchar.questTemp.PorGG.Treasury);
				if (iTreasury > 0)
				{
					dialog.text = StringFromKey("POR_Line_dialog_98", FindMoneyString(iTreasury));
					link.l1 = StringFromKey("POR_Line_dialog_100");
					link.l1.go = "Sec_Take";
				}
				else
				{
					dialog.text = StringFromKey("POR_Line_dialog_99");
					link.l1 = StringFromKey("POR_Line_dialog_2");
					link.l1.go = "exit";
				}
				link.l2 = StringFromKey("POR_Line_dialog_104");
				link.l2.go = "Sec_Move";
				link.l3 = StringFromKey("POR_Line_dialog_2");
				link.l3.go = "exit";
			}
			if (sId == "POR_Rep")
			{
				dialog.text = StringFromKey("POR_Line_dialog_90");
				link.l1 = StringFromKey("POR_Line_dialog_2");
				link.l1.go = "exit";
				if (sSt == "M6_rep")
				{
					dialog.text = StringFromKey("POR_Line_dialog_73");
					link.l1 = StringFromKey("POR_Line_dialog_74");
					link.l1.go = "Rep_2";
				}
				if (sSt == "M7_go")
				{
					dialog.text = StringFromKey("POR_Line_dialog_82");
					link.l1 = StringFromKey("POR_Line_dialog_81");
					link.l1.go = "exit";
				}
				if (sSt == "M7_captured")
				{
					dialog.text = StringFromKey("POR_Line_dialog_83");
					link.l1 = StringFromKey("POR_Line_dialog_84");
					link.l1.go = "Rep_5";
				}
				if (sSt == "M7_sunk")
				{
					dialog.text = StringFromKey("POR_Line_dialog_83");
					link.l1 = StringFromKey("POR_Line_dialog_85");
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
			dialog.text = StringFromKey("POR_Line_dialog_7");
			link.l1 = StringFromKey("POR_Line_dialog_8");
			link.l1.go = "M1_C3";
		break;

		case "M1_C3":
			dialog.text = StringFromKey("POR_Line_dialog_9");
			link.l1 = StringFromKey("POR_Line_dialog_10");
			link.l1.go = "M1_C4";
		break;

		case "M1_C4":
			dialog.text = StringFromKey("POR_Line_dialog_11");
			link.l1 = StringFromKey("POR_Line_dialog_12");
			link.l1.go = "M1_C5";
		break;

		case "M1_C5":
			dialog.text = StringFromKey("POR_Line_dialog_13");
			link.l1 = StringFromKey("POR_Line_dialog_4");
			link.l1.go = "Cabin_Fight";
		break;

		// ---- missao 2: Tome Ferraz
		case "Tome_Gov":
			dialog.text = StringFromKey("POR_Line_dialog_16");
			link.l1 = StringFromKey("POR_Line_dialog_17");
			link.l1.go = "Tome_Bento";
			link.l2 = StringFromKey("POR_Line_dialog_2");
			link.l2.go = "exit";
		break;

		case "Tome_Bento":
			dialog.text = StringFromKey("POR_Line_dialog_18");
			link.l1 = StringFromKey("POR_Line_dialog_19");
			link.l1.go = "Tome_Price";
			pchar.questTemp.PorLine.TomeOpen = true;
		break;

		case "Tome_Price":
			dialog.text = StringFromKey("POR_Line_dialog_20");
			link.l1 = StringFromKey("POR_Line_dialog_21");
			link.l1.go = "Tome_Pay";
			link.l2 = StringFromKey("POR_Line_dialog_22");
			link.l2.go = "Tome_NoMoney";
		break;

		case "Tome_NoMoney":
			dialog.text = StringFromKey("POR_Line_dialog_23");
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";
		break;

		case "Tome_Pay":
			if (sti(pchar.money) < 3000)
			{
				dialog.text = StringFromKey("POR_Line_dialog_23");
				link.l1 = StringFromKey("POR_Line_dialog_2");
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -3000);
			dialog.text = StringFromKey("POR_Line_dialog_24");
			link.l1 = StringFromKey("POR_Line_dialog_25");
			link.l1.go = "Tome_3";
		break;

		case "Tome_3":
			dialog.text = StringFromKey("POR_Line_dialog_26");
			link.l1 = StringFromKey("POR_Line_dialog_27");
			link.l1.go = "Tome_4";
		break;

		case "Tome_4":
			dialog.text = StringFromKey("POR_Line_dialog_28");
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";
			POR_M2_ToAruba();
		break;

		// ---- missao 2: correio espanhol
		case "M2_C2":
			dialog.text = StringFromKey("POR_Line_dialog_32");
			link.l1 = StringFromKey("POR_Line_dialog_33");
			link.l1.go = "M2_C3";
		break;

		case "M2_C3":
			dialog.text = StringFromKey("POR_Line_dialog_34");
			link.l1 = StringFromKey("POR_Line_dialog_35");
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.Letters = true;
		break;

		// ---- missao 3: Amaro Teles
		case "Amaro_2":
			dialog.text = StringFromKey("POR_Line_dialog_38");
			link.l1 = StringFromKey("POR_Line_dialog_39");
			link.l1.go = "Amaro_3";
		break;

		case "Amaro_3":
			dialog.text = StringFromKey("POR_Line_dialog_40");
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";
			POR_M3_AmaroDone();
		break;

		// ---- missao 4: Gaspar Roiz
		case "M4_C2":
			dialog.text = StringFromKey("POR_Line_dialog_44");
			link.l1 = StringFromKey("POR_Line_dialog_45");
			link.l1.go = "M4_C3";
		break;

		case "M4_C3":
			dialog.text = StringFromKey("POR_Line_dialog_46");
			link.l1 = StringFromKey("POR_Line_dialog_47");
			link.l1.go = "M4_Deal";
			link.l2 = StringFromKey("POR_Line_dialog_48");
			link.l2.go = "M4_NoDeal";
		break;

		case "M4_Deal":
			dialog.text = StringFromKey("POR_Line_dialog_49");
			link.l1 = StringFromKey("POR_Line_dialog_51");
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.RoizAlive = true;
		break;

		case "M4_NoDeal":
			dialog.text = StringFromKey("POR_Line_dialog_50");
			link.l1 = StringFromKey("POR_Line_dialog_4");
			link.l1.go = "Cabin_Fight";
		break;

		// ---- missao 5: Simao Godinho
		case "God_Deny":
			dialog.text = StringFromKey("POR_Line_dialog_55");
			link.l1 = StringFromKey("POR_Line_dialog_56");
			link.l1.go = "God_2";
			link.l2 = StringFromKey("POR_Line_dialog_53");
			link.l2.go = "God_2";
		break;

		case "God_2":
			dialog.text = StringFromKey("POR_Line_dialog_57");
			link.l1 = StringFromKey("POR_Line_dialog_58");
			link.l1.go = "God_3";
		break;

		case "God_3":
			dialog.text = StringFromKey("POR_Line_dialog_59");
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";
			POR_M5_ToSecretary();
		break;

		// ---- missao 5/6: Lourenco Viegas
		case "Lou_2":
			dialog.text = StringFromKey("POR_Line_dialog_63");
			link.l1 = StringFromKey("POR_Line_dialog_64");
			link.l1.go = "Lou_3";
			link.l2 = StringFromKey("POR_Line_dialog_65");
			link.l2.go = "Lou_3";
		break;

		case "Lou_3":
			dialog.text = StringFromKey("POR_Line_dialog_66");
			link.l1 = StringFromKey("POR_Line_dialog_67");
			link.l1.go = "Lou_4";
		break;

		case "Lou_4":
			dialog.text = StringFromKey("POR_Line_dialog_68");
			link.l1 = StringFromKey("POR_Line_dialog_69");
			link.l1.go = "Lou_5";
		break;

		case "Lou_5":
			dialog.text = StringFromKey("POR_Line_dialog_70");
			link.l1 = StringFromKey("POR_Line_dialog_71");
			link.l1.go = "exit";
			POR_M6_Start();
		break;

		// ---- Lourenco, secretario do novo Governador-Geral
		case "Sec_Take":
			iTreasury = sti(pchar.questTemp.PorGG.Treasury);
			AddMoneyToCharacter(pchar, iTreasury);
			pchar.questTemp.PorGG.Treasury = 0;
			dialog.text = StringFromKey("POR_Line_dialog_101");
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";
		break;

		case "Sec_Move":
			dialog.text = StringFromKey("POR_Line_dialog_106");
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
			if (n > 0) dialog.text = StringFromKey("POR_Line_dialog_105");
			link.l9 = StringFromKey("POR_Line_dialog_107");
			link.l9.go = "exit";
		break;

		case "Sec_Move_Do":
			i = sti(NPChar.quest.SeatIdx);
			pchar.questTemp.PorGG.Seat = colonies[i].id;
			NPChar.PorMoveTo = colonies[i].id;
			dialog.text = StringFromKey("POR_Line_dialog_108", GetCityName(colonies[i].id));
			link.l1 = StringFromKey("POR_Line_dialog_2");
			link.l1.go = "exit";
		break;

		// ---- missao 6/7: Representante da Coroa
		case "Rep_2":
			dialog.text = StringFromKey("POR_Line_dialog_75");
			link.l1 = StringFromKey("POR_Line_dialog_76");
			link.l1.go = "Rep_3";
		break;

		case "Rep_3":
			dialog.text = StringFromKey("POR_Line_dialog_77");
			if (CheckAttribute(pchar, "questTemp.PorLine.LettersKept")) dialog.text = StringFromKey("POR_Line_dialog_77") + " " + StringFromKey("POR_Line_dialog_78");
			link.l1 = StringFromKey("POR_Line_dialog_79");
			link.l1.go = "Rep_4";
			POR_SetRank(5);
		break;

		case "Rep_4":
			dialog.text = StringFromKey("POR_Line_dialog_80");
			link.l1 = StringFromKey("POR_Line_dialog_81");
			link.l1.go = "exit";
			POR_M7_Start();
		break;

		case "Rep_5":
			dialog.text = StringFromKey("POR_Line_dialog_86");
			link.l1 = StringFromKey("POR_Line_dialog_87");
			link.l1.go = "Rep_6";
		break;

		case "Rep_6":
			dialog.text = StringFromKey("POR_Line_dialog_88");
			link.l1 = StringFromKey("POR_Line_dialog_89");
			link.l1.go = "exit";
			POR_Final();
		break;

		// ---- missao 7: o governador traidor
		case "M7_C2":
			dialog.text = StringFromKey("POR_Line_dialog_93");
			link.l1 = StringFromKey("POR_Line_dialog_94");
			link.l1.go = "M7_C3";
		break;

		case "M7_C3":
			dialog.text = StringFromKey("POR_Line_dialog_95");
			link.l1 = StringFromKey("POR_Line_dialog_96");
			link.l1.go = "M7_C4";
		break;

		case "M7_C4":
			dialog.text = StringFromKey("POR_Line_dialog_97");
			link.l1 = StringFromKey("POR_Line_dialog_51");
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.GovCaptured = true;
		break;
	}
}
