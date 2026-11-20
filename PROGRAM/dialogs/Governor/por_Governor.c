// Nova Nacao: dialogo do Governador-Geral de Portugal e linha de missoes "Pela Gloria da Coroa"
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
	string sSt;
	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = StringFromKey("Diplomat_dialog_1001");
			link.l99 = StringFromKey("Diplomat_dialog_1002");
			link.l99.go = "exit";
		break;

		case "work_1":
			if (!CheckAttribute(pchar, "questTemp.PorLine")) pchar.questTemp.PorLine = "";
			NPChar.lastname = "Barreto";
			sSt = pchar.questTemp.PorLine;
			dialog.text = StringFromKey("Diplomat_dialog_1003");
			link.l1 = StringFromKey("Diplomat_dialog_1004");
			link.l1.go = "exit";
			if (sSt == "")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1008");
				link.l1 = StringFromKey("Diplomat_dialog_1009");
				link.l1.go = "POR_M1_1";
			}
			if (sSt == "M1_go" || sSt == "M2_go" || sSt == "M2_aruba" || sSt == "M3_go" || sSt == "M3_amaro" || sSt == "M4_go")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1005");
				link.l1 = StringFromKey("Diplomat_dialog_1006");
				link.l1.go = "exit";
			}
			if (sSt == "M1_done")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1016");
				link.l1 = StringFromKey("Diplomat_dialog_1017");
				if (CheckAttribute(pchar, "questTemp.PorLine.M1Sunk")) link.l1 = StringFromKey("Diplomat_dialog_1055");
				link.l1.go = "POR_M1_R1";
			}
			if (sSt == "M2_done")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1024");
				link.l1 = StringFromKey("Diplomat_dialog_1025");
				if (CheckAttribute(pchar, "questTemp.PorLine.M2Sunk")) link.l1 = StringFromKey("Diplomat_dialog_1056");
				link.l1.go = "POR_M2_R1";
			}
			if (sSt == "M3_done")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1036");
				link.l1 = StringFromKey("Diplomat_dialog_1037");
				link.l1.go = "POR_M3_R1";
			}
			if (sSt == "M3_lost")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1036");
				link.l1 = StringFromKey("Diplomat_dialog_1039");
				link.l1.go = "POR_M3_R1";
			}
			if (sSt == "M4_done")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1045");
				link.l1 = StringFromKey("Diplomat_dialog_1046");
				link.l1.go = "POR_M4_R1";
			}
			if (sSt == "M5_godinho" || sSt == "M5_secretary" || sSt == "M6_go" || sSt == "M6_rep")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1007");
				link.l1 = StringFromKey("Diplomat_dialog_1006");
				link.l1.go = "exit";
			}
		break;

		// ---- missao 1
		case "POR_M1_1":
			dialog.text = StringFromKey("Diplomat_dialog_1010");
			link.l1 = StringFromKey("Diplomat_dialog_1011");
			link.l1.go = "POR_M1_2";
		break;

		case "POR_M1_2":
			dialog.text = StringFromKey("Diplomat_dialog_1012");
			link.l1 = StringFromKey("Diplomat_dialog_1013");
			link.l1.go = "POR_M1_3";
			link.l2 = StringFromKey("Diplomat_dialog_1015");
			link.l2.go = "exit";
			POR_M1_Start();
		break;

		case "POR_M1_3":
			dialog.text = StringFromKey("Diplomat_dialog_1014");
			link.l1 = StringFromKey("Diplomat_dialog_1015");
			link.l1.go = "exit";
		break;

		case "POR_M1_R1":
			dialog.text = StringFromKey("Diplomat_dialog_1018");
			link.l1 = StringFromKey("Diplomat_dialog_1019");
			link.l1.go = "POR_M1_R2";
		break;

		case "POR_M1_R2":
			dialog.text = StringFromKey("Diplomat_dialog_1020");
			link.l1 = StringFromKey("Diplomat_dialog_1021");
			link.l1.go = "POR_M2_1";
			POR_M1_Reward();
		break;

		// ---- missao 2
		case "POR_M2_1":
			dialog.text = StringFromKey("Diplomat_dialog_1022");
			link.l1 = StringFromKey("Diplomat_dialog_1023");
			link.l1.go = "exit";
			POR_M2_Start();
		break;

		case "POR_M2_R1":
			dialog.text = StringFromKey("Diplomat_dialog_1026");
			link.l1 = StringFromKey("Diplomat_dialog_1027");
			link.l1.go = "POR_M2_R2";
			if (CheckAttribute(pchar, "questTemp.PorLine.Letters"))
			{
				link.l2 = StringFromKey("Diplomat_dialog_1028");
				if (CheckAttribute(pchar, "questTemp.PorLine.M2Sunk")) link.l2 = StringFromKey("Diplomat_dialog_1057");
				link.l2.go = "POR_M2_R3";
			}
		break;

		case "POR_M2_R2":
			dialog.text = StringFromKey("Diplomat_dialog_1029");
			link.l1 = StringFromKey("Diplomat_dialog_1031");
			link.l1.go = "POR_M3_1";
			DeleteAttribute(pchar, "questTemp.PorLine.Letters");
			POR_M2_Reward();
		break;

		case "POR_M2_R3":
			dialog.text = StringFromKey("Diplomat_dialog_1030");
			link.l1 = StringFromKey("Diplomat_dialog_1031");
			link.l1.go = "POR_M3_1";
			pchar.questTemp.PorLine.LettersKept = true;
			POR_M2_Reward();
		break;

		// ---- missao 3
		case "POR_M3_1":
			dialog.text = StringFromKey("Diplomat_dialog_1032");
			link.l1 = StringFromKey("Diplomat_dialog_1033");
			link.l1.go = "POR_M3_2";
			link.l2 = StringFromKey("Diplomat_dialog_1035");
			link.l2.go = "exit";
			POR_M3_Start();
		break;

		case "POR_M3_2":
			dialog.text = StringFromKey("Diplomat_dialog_1034");
			link.l1 = StringFromKey("Diplomat_dialog_1035");
			link.l1.go = "exit";
		break;

		case "POR_M3_R1":
			if (pchar.questTemp.PorLine == "M3_lost")
			{
				dialog.text = StringFromKey("Diplomat_dialog_1040");
			}
			else
			{
				dialog.text = StringFromKey("Diplomat_dialog_1038");
			}
			link.l1 = StringFromKey("Diplomat_dialog_1021");
			link.l1.go = "POR_M4_1";
			POR_M3_Reward();
		break;

		// ---- missao 4
		case "POR_M4_1":
			dialog.text = StringFromKey("Diplomat_dialog_1041");
			link.l1 = StringFromKey("Diplomat_dialog_1042");
			link.l1.go = "POR_M4_2";
			link.l2 = StringFromKey("Diplomat_dialog_1044");
			link.l2.go = "exit";
			POR_M4_Start();
		break;

		case "POR_M4_2":
			dialog.text = StringFromKey("Diplomat_dialog_1043");
			link.l1 = StringFromKey("Diplomat_dialog_1044");
			link.l1.go = "exit";
		break;

		case "POR_M4_R1":
			dialog.text = StringFromKey("Diplomat_dialog_1047");
			link.l1 = StringFromKey("Diplomat_dialog_1048");
			link.l1.go = "POR_M4_R3";
			link.l2 = StringFromKey("Diplomat_dialog_1049");
			link.l2.go = "POR_M4_R2";
		break;

		case "POR_M4_R2":
			dialog.text = StringFromKey("Diplomat_dialog_1050");
			link.l1 = StringFromKey("Diplomat_dialog_1051");
			link.l1.go = "POR_M4_R3";
			pchar.questTemp.PorLine.LedgerGiven = true;
		break;

		case "POR_M4_R3":
			dialog.text = StringFromKey("Diplomat_dialog_1052");
			link.l1 = StringFromKey("Diplomat_dialog_1053");
			link.l1.go = "exit";
			POR_M4_Reward();
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // se sair por return dentro do switch, descarregar
}
