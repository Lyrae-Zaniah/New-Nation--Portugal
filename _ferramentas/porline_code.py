# -*- coding: utf-8 -*-
# Codigo (Storm script) da linha de missoes de Portugal. {G:chave} e {D:chave} viram StringFromKey.
from porline_line import LINE_C

GOV_C = r'''// Nova Nacao: dialogo do Governador-Geral de Portugal e linha de missoes "Pela Gloria da Coroa"
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
	string sSt;
	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = StringFromKey("por_Governor_1");
			link.l99 = StringFromKey("por_Governor_2");
			link.l99.go = "exit";
		break;

		case "work_1":
			if (!CheckAttribute(pchar, "questTemp.PorLine")) pchar.questTemp.PorLine = "";
			NPChar.lastname = "Barreto";
			sSt = pchar.questTemp.PorLine;
			dialog.text = StringFromKey("por_Governor_3");
			link.l1 = StringFromKey("por_Governor_4");
			link.l1.go = "exit";
			if (sSt == "")
			{
				dialog.text = {G:m1_intro};
				link.l1 = {G:p_m1_q};
				link.l1.go = "POR_M1_1";
			}
			if (sSt == "M1_go" || sSt == "M2_go" || sSt == "M2_aruba" || sSt == "M3_go" || sSt == "M3_amaro" || sSt == "M4_go")
			{
				dialog.text = {G:busy};
				link.l1 = {G:busy_p};
				link.l1.go = "exit";
			}
			if (sSt == "M1_done")
			{
				dialog.text = {G:m1_report_q};
				link.l1 = {G:p_m1_report};
				link.l1.go = "POR_M1_R1";
			}
			if (sSt == "M2_done")
			{
				dialog.text = {G:m2_report_q};
				link.l1 = {G:p_m2_report};
				link.l1.go = "POR_M2_R1";
			}
			if (sSt == "M3_done")
			{
				dialog.text = {G:m3_report_q};
				link.l1 = {G:p_m3_good};
				link.l1.go = "POR_M3_R1";
			}
			if (sSt == "M3_lost")
			{
				dialog.text = {G:m3_report_q};
				link.l1 = {G:p_m3_bad};
				link.l1.go = "POR_M3_R1";
			}
			if (sSt == "M4_done")
			{
				dialog.text = {G:m4_report_q};
				link.l1 = {G:p_m4_report};
				link.l1.go = "POR_M4_R1";
			}
			if (sSt == "M5_godinho" || sSt == "M5_secretary" || sSt == "M6_go" || sSt == "M6_rep")
			{
				dialog.text = {G:rest};
				link.l1 = {G:busy_p};
				link.l1.go = "exit";
			}
		break;

		// ---- missao 1
		case "POR_M1_1":
			dialog.text = {G:m1_brief};
			link.l1 = {G:p_m1_who};
			link.l1.go = "POR_M1_2";
		break;

		case "POR_M1_2":
			dialog.text = {G:m1_who};
			link.l1 = {G:p_m1_more};
			link.l1.go = "POR_M1_3";
			link.l2 = {G:p_m1_ok};
			link.l2.go = "exit";
			POR_M1_Start();
		break;

		case "POR_M1_3":
			dialog.text = {G:m1_more};
			link.l1 = {G:p_m1_ok};
			link.l1.go = "exit";
		break;

		case "POR_M1_R1":
			dialog.text = {G:m1_list};
			link.l1 = {G:p_m1_give};
			link.l1.go = "POR_M1_R2";
		break;

		case "POR_M1_R2":
			dialog.text = {G:m1_reward};
			link.l1 = {G:p_next};
			link.l1.go = "POR_M2_1";
			POR_M1_Reward();
		break;

		// ---- missao 2
		case "POR_M2_1":
			dialog.text = {G:m2_order};
			link.l1 = {G:p_m2_go};
			link.l1.go = "exit";
			POR_M2_Start();
		break;

		case "POR_M2_R1":
			dialog.text = {G:m2_friend};
			link.l1 = {G:p_m2_give};
			link.l1.go = "POR_M2_R2";
			if (CheckAttribute(pchar, "questTemp.PorLine.Letters"))
			{
				link.l2 = {G:p_m2_keep};
				link.l2.go = "POR_M2_R3";
			}
		break;

		case "POR_M2_R2":
			dialog.text = {G:m2_reward_give};
			link.l1 = {G:p_as_command};
			link.l1.go = "POR_M3_1";
			DeleteAttribute(pchar, "questTemp.PorLine.Letters");
			POR_M2_Reward();
		break;

		case "POR_M2_R3":
			dialog.text = {G:m2_reward_keep};
			link.l1 = {G:p_as_command};
			link.l1.go = "POR_M3_1";
			pchar.questTemp.PorLine.LettersKept = true;
			POR_M2_Reward();
		break;

		// ---- missao 3
		case "POR_M3_1":
			dialog.text = {G:m3_order};
			link.l1 = {G:p_m3_how};
			link.l1.go = "POR_M3_2";
			link.l2 = {G:p_m3_go};
			link.l2.go = "exit";
			POR_M3_Start();
		break;

		case "POR_M3_2":
			dialog.text = {G:m3_how};
			link.l1 = {G:p_m3_go};
			link.l1.go = "exit";
		break;

		case "POR_M3_R1":
			if (pchar.questTemp.PorLine == "M3_lost")
			{
				dialog.text = {G:m3_bad_r};
			}
			else
			{
				dialog.text = {G:m3_good_r};
			}
			link.l1 = {G:p_next};
			link.l1.go = "POR_M4_1";
			POR_M3_Reward();
		break;

		// ---- missao 4
		case "POR_M4_1":
			dialog.text = {G:m4_order};
			link.l1 = {G:p_m4_greedy};
			link.l1.go = "POR_M4_2";
			link.l2 = {G:p_m4_go};
			link.l2.go = "exit";
			POR_M4_Start();
		break;

		case "POR_M4_2":
			dialog.text = {G:m4_greedy};
			link.l1 = {G:p_m4_go};
			link.l1.go = "exit";
		break;

		case "POR_M4_R1":
			dialog.text = {G:m4_papers};
			link.l1 = {G:p_m4_lie};
			link.l1.go = "POR_M4_R3";
			link.l2 = {G:p_m4_truth};
			link.l2.go = "POR_M4_R2";
		break;

		case "POR_M4_R2":
			dialog.text = {G:m4_truth};
			link.l1 = {G:p_m4_give};
			link.l1.go = "POR_M4_R3";
			pchar.questTemp.PorLine.LedgerGiven = true;
		break;

		case "POR_M4_R3":
			dialog.text = {G:m4_reward};
			link.l1 = {G:p_thanks};
			link.l1.go = "exit";
			POR_M4_Reward();
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // se sair por return dentro do switch, descarregar
}
'''

DLG_C = r'''// Nova Nacao: personagens da linha de missoes de Portugal ("Pela Gloria da Coroa")
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

			dialog.text = {D:nothing};
			link.l1 = {D:ok};
			link.l1.go = "exit";

			// capitaes na cabine (abordagem)
			if (sId == sCab && sSt == "M1_go")
			{
				dialog.text = {D:m1_c1};
				link.l1 = {D:m1_p1};
				link.l1.go = "M1_C2";
			}
			if (sId == sCab && sSt == "M2_aruba")
			{
				dialog.text = {D:m2_c1};
				link.l1 = {D:m2_p1};
				link.l1.go = "M2_C2";
			}
			if (sId == sCab && sSt == "M4_go")
			{
				dialog.text = {D:m4_c1};
				link.l1 = {D:m4_p1};
				link.l1.go = "M4_C2";
			}
			if (sId == sCab && sSt == "M7_go")
			{
				dialog.text = {D:m7_c1};
				link.l1 = {D:m7_p1};
				link.l1.go = "M7_C2";
			}
			// personagens em terra
			if (sId == "POR_Tome")
			{
				dialog.text = {D:tome_after};
				link.l1 = {D:ok};
				link.l1.go = "exit";
				if (sSt == "M2_go")
				{
					if (CheckAttribute(pchar, "questTemp.PorLine.TomeOpen"))
					{
						dialog.text = {D:tome_price};
						link.l1 = {D:tome_p_pay};
						link.l1.go = "Tome_Pay";
						link.l2 = {D:tome_p_nomoney};
						link.l2.go = "Tome_NoMoney";
					}
					else
					{
						dialog.text = {D:tome_1};
						link.l1 = {D:tome_p_gov};
						link.l1.go = "Tome_Gov";
						link.l2 = {D:tome_p_bento};
						link.l2.go = "Tome_Bento";
						link.l3 = {D:ok};
						link.l3.go = "exit";
					}
				}
			}
			if (sId == "POR_Amaro")
			{
				dialog.text = {D:amaro_after};
				link.l1 = {D:ok};
				link.l1.go = "exit";
				if (sSt == "M3_amaro")
				{
					dialog.text = {D:amaro_1};
					link.l1 = {D:amaro_p1};
					link.l1.go = "Amaro_2";
				}
			}
			if (sId == "POR_Godinho")
			{
				dialog.text = {D:god_after};
				link.l1 = {D:ok};
				link.l1.go = "exit";
				if (sSt == "M5_godinho")
				{
					dialog.text = {D:god_1};
					link.l1 = {D:god_p_ledger};
					link.l1.go = "God_2";
					link.l2 = {D:god_p_ask};
					link.l2.go = "God_Deny";
				}
			}
			if (sId == "POR_Lourenco")
			{
				dialog.text = {D:lou_after};
				link.l1 = {D:ok};
				link.l1.go = "exit";
				if (sSt == "M5_secretary")
				{
					dialog.text = {D:lou_1};
					link.l1 = {D:lou_p1};
					link.l1.go = "Lou_2";
				}
			}
			if (sId == "POR_Sec")
			{
				iTreasury = 0;
				if (CheckAttribute(pchar, "questTemp.PorGG.Treasury")) iTreasury = sti(pchar.questTemp.PorGG.Treasury);
				if (iTreasury > 0)
				{
					dialog.text = StringFromKey("POR_Line_dialog_{N:sec_hello}", FindMoneyString(iTreasury));
					link.l1 = {D:sec_p_take};
					link.l1.go = "Sec_Take";
				}
				else
				{
					dialog.text = {D:sec_empty};
					link.l1 = {D:ok};
					link.l1.go = "exit";
				}
				link.l2 = {D:sec_p_move};
				link.l2.go = "Sec_Move";
				link.l3 = {D:ok};
				link.l3.go = "exit";
			}
			if (sId == "POR_Rep")
			{
				dialog.text = {D:rep_done};
				link.l1 = {D:ok};
				link.l1.go = "exit";
				if (sSt == "M6_rep")
				{
					dialog.text = {D:rep_1};
					link.l1 = {D:rep_p1};
					link.l1.go = "Rep_2";
				}
				if (sSt == "M7_go")
				{
					dialog.text = {D:rep_wait};
					link.l1 = {D:rep_p4};
					link.l1.go = "exit";
				}
				if (sSt == "M7_captured")
				{
					dialog.text = {D:rep_back};
					link.l1 = {D:rep_p_captured};
					link.l1.go = "Rep_5";
				}
				if (sSt == "M7_sunk")
				{
					dialog.text = {D:rep_back};
					link.l1 = {D:rep_p_sunk};
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
			dialog.text = {D:m1_c2};
			link.l1 = {D:m1_p2};
			link.l1.go = "M1_C3";
		break;

		case "M1_C3":
			dialog.text = {D:m1_c3};
			link.l1 = {D:m1_p3};
			link.l1.go = "M1_C4";
		break;

		case "M1_C4":
			dialog.text = {D:m1_c4};
			link.l1 = {D:m1_p4};
			link.l1.go = "M1_C5";
		break;

		case "M1_C5":
			dialog.text = {D:m1_c5};
			link.l1 = {D:fight};
			link.l1.go = "Cabin_Fight";
		break;

		// ---- missao 2: Tome Ferraz
		case "Tome_Gov":
			dialog.text = {D:tome_gov};
			link.l1 = {D:tome_p_bento};
			link.l1.go = "Tome_Bento";
			link.l2 = {D:ok};
			link.l2.go = "exit";
		break;

		case "Tome_Bento":
			dialog.text = {D:tome_bento};
			link.l1 = {D:tome_p_ask};
			link.l1.go = "Tome_Price";
			pchar.questTemp.PorLine.TomeOpen = true;
		break;

		case "Tome_Price":
			dialog.text = {D:tome_price};
			link.l1 = {D:tome_p_pay};
			link.l1.go = "Tome_Pay";
			link.l2 = {D:tome_p_nomoney};
			link.l2.go = "Tome_NoMoney";
		break;

		case "Tome_NoMoney":
			dialog.text = {D:tome_nomoney};
			link.l1 = {D:ok};
			link.l1.go = "exit";
		break;

		case "Tome_Pay":
			if (sti(pchar.money) < 3000)
			{
				dialog.text = {D:tome_nomoney};
				link.l1 = {D:ok};
				link.l1.go = "exit";
				break;
			}
			AddMoneyToCharacter(pchar, -3000);
			dialog.text = {D:tome_2};
			link.l1 = {D:tome_p2};
			link.l1.go = "Tome_3";
		break;

		case "Tome_3":
			dialog.text = {D:tome_3};
			link.l1 = {D:tome_p3};
			link.l1.go = "Tome_4";
		break;

		case "Tome_4":
			dialog.text = {D:tome_4};
			link.l1 = {D:ok};
			link.l1.go = "exit";
			POR_M2_ToAruba();
		break;

		// ---- missao 2: correio espanhol
		case "M2_C2":
			dialog.text = {D:m2_c2};
			link.l1 = {D:m2_p2};
			link.l1.go = "M2_C3";
		break;

		case "M2_C3":
			dialog.text = {D:m2_c3};
			link.l1 = {D:m2_p3};
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.Letters = true;
		break;

		// ---- missao 3: Amaro Teles
		case "Amaro_2":
			dialog.text = {D:amaro_2};
			link.l1 = {D:amaro_p2};
			link.l1.go = "Amaro_3";
		break;

		case "Amaro_3":
			dialog.text = {D:amaro_3};
			link.l1 = {D:ok};
			link.l1.go = "exit";
			POR_M3_AmaroDone();
		break;

		// ---- missao 4: Gaspar Roiz
		case "M4_C2":
			dialog.text = {D:m4_c2};
			link.l1 = {D:m4_p2};
			link.l1.go = "M4_C3";
		break;

		case "M4_C3":
			dialog.text = {D:m4_c3};
			link.l1 = {D:m4_p_deal};
			link.l1.go = "M4_Deal";
			link.l2 = {D:m4_p_nodeal};
			link.l2.go = "M4_NoDeal";
		break;

		case "M4_Deal":
			dialog.text = {D:m4_deal};
			link.l1 = {D:take_him};
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.RoizAlive = true;
		break;

		case "M4_NoDeal":
			dialog.text = {D:m4_nodeal};
			link.l1 = {D:fight};
			link.l1.go = "Cabin_Fight";
		break;

		// ---- missao 5: Simao Godinho
		case "God_Deny":
			dialog.text = {D:god_deny};
			link.l1 = {D:god_p_threat};
			link.l1.go = "God_2";
			link.l2 = {D:god_p_ledger};
			link.l2.go = "God_2";
		break;

		case "God_2":
			dialog.text = {D:god_2};
			link.l1 = {D:god_p2};
			link.l1.go = "God_3";
		break;

		case "God_3":
			dialog.text = {D:god_3};
			link.l1 = {D:ok};
			link.l1.go = "exit";
			POR_M5_ToSecretary();
		break;

		// ---- missao 5/6: Lourenco Viegas
		case "Lou_2":
			dialog.text = {D:lou_2};
			link.l1 = {D:lou_p_crown};
			link.l1.go = "Lou_3";
			link.l2 = {D:lou_p_used};
			link.l2.go = "Lou_3";
		break;

		case "Lou_3":
			dialog.text = {D:lou_3};
			link.l1 = {D:lou_p3};
			link.l1.go = "Lou_4";
		break;

		case "Lou_4":
			dialog.text = {D:lou_4};
			link.l1 = {D:lou_p4};
			link.l1.go = "Lou_5";
		break;

		case "Lou_5":
			dialog.text = {D:lou_5};
			link.l1 = {D:lou_p5};
			link.l1.go = "exit";
			POR_M6_Start();
		break;

		// ---- Lourenco, secretario do novo Governador-Geral
		case "Sec_Take":
			iTreasury = sti(pchar.questTemp.PorGG.Treasury);
			AddMoneyToCharacter(pchar, iTreasury);
			pchar.questTemp.PorGG.Treasury = 0;
			dialog.text = {D:sec_take};
			link.l1 = {D:ok};
			link.l1.go = "exit";
		break;

		case "Sec_Move":
			dialog.text = {D:sec_move_none};
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
			if (n > 0) dialog.text = {D:sec_move_q};
			link.l9 = {D:sec_p_cancel};
			link.l9.go = "exit";
		break;

		case "Sec_Move_Do":
			i = sti(NPChar.quest.SeatIdx);
			pchar.questTemp.PorGG.Seat = colonies[i].id;
			NPChar.PorMoveTo = colonies[i].id;
			dialog.text = StringFromKey("POR_Line_dialog_{N:sec_move_ok}", GetCityName(colonies[i].id));
			link.l1 = {D:ok};
			link.l1.go = "exit";
		break;

		// ---- missao 6/7: Representante da Coroa
		case "Rep_2":
			dialog.text = {D:rep_2};
			link.l1 = {D:rep_p2};
			link.l1.go = "Rep_3";
		break;

		case "Rep_3":
			dialog.text = {D:rep_3};
			if (CheckAttribute(pchar, "questTemp.PorLine.LettersKept")) dialog.text = {D:rep_3} + " " + {D:rep_letters};
			link.l1 = {D:rep_p3};
			link.l1.go = "Rep_4";
			POR_SetRank(5);
		break;

		case "Rep_4":
			dialog.text = {D:rep_4};
			link.l1 = {D:rep_p4};
			link.l1.go = "exit";
			POR_M7_Start();
		break;

		case "Rep_5":
			dialog.text = {D:rep_5};
			link.l1 = {D:rep_p5};
			link.l1.go = "Rep_6";
		break;

		case "Rep_6":
			dialog.text = {D:rep_6};
			link.l1 = {D:rep_p6};
			link.l1.go = "exit";
			POR_Final();
		break;

		// ---- missao 7: o governador traidor
		case "M7_C2":
			dialog.text = {D:m7_c2};
			link.l1 = {D:m7_p_used};
			link.l1.go = "M7_C3";
		break;

		case "M7_C3":
			dialog.text = {D:m7_c3};
			link.l1 = {D:m7_p2};
			link.l1.go = "M7_C4";
		break;

		case "M7_C4":
			dialog.text = {D:m7_c4};
			link.l1 = {D:take_him};
			link.l1.go = "Cabin_Surrender";
			pchar.questTemp.PorLine.GovCaptured = true;
		break;
	}
}
'''
