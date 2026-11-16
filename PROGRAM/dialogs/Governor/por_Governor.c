// Nova Nacao: dialogo do Governador-Geral de Portugal e linha de missoes "Pela Gloria da Coroa"
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
				dialog.text = StringFromKey("por_Governor_8");
				link.l1 = StringFromKey("por_Governor_9");
				link.l1.go = "POR_M1_1";
			}
			if (sSt == "M1_go" || sSt == "M2_go" || sSt == "M2_aruba" || sSt == "M3_go" || sSt == "M3_amaro" || sSt == "M4_go")
			{
				dialog.text = StringFromKey("por_Governor_5");
				link.l1 = StringFromKey("por_Governor_6");
				link.l1.go = "exit";
			}
			if (sSt == "M1_done")
			{
				dialog.text = StringFromKey("por_Governor_16");
				link.l1 = StringFromKey("por_Governor_17");
				link.l1.go = "POR_M1_R1";
			}
			if (sSt == "M2_done")
			{
				dialog.text = StringFromKey("por_Governor_24");
				link.l1 = StringFromKey("por_Governor_25");
				link.l1.go = "POR_M2_R1";
			}
			if (sSt == "M3_done")
			{
				dialog.text = StringFromKey("por_Governor_36");
				link.l1 = StringFromKey("por_Governor_37");
				link.l1.go = "POR_M3_R1";
			}
			if (sSt == "M3_lost")
			{
				dialog.text = StringFromKey("por_Governor_36");
				link.l1 = StringFromKey("por_Governor_39");
				link.l1.go = "POR_M3_R1";
			}
			if (sSt == "M4_done")
			{
				dialog.text = StringFromKey("por_Governor_45");
				link.l1 = StringFromKey("por_Governor_46");
				link.l1.go = "POR_M4_R1";
			}
			if (sSt == "M5_godinho" || sSt == "M5_secretary" || sSt == "M6_go" || sSt == "M6_rep")
			{
				dialog.text = StringFromKey("por_Governor_7");
				link.l1 = StringFromKey("por_Governor_6");
				link.l1.go = "exit";
			}
		break;

		// ---- missao 1
		case "POR_M1_1":
			dialog.text = StringFromKey("por_Governor_10");
			link.l1 = StringFromKey("por_Governor_11");
			link.l1.go = "POR_M1_2";
		break;

		case "POR_M1_2":
			dialog.text = StringFromKey("por_Governor_12");
			link.l1 = StringFromKey("por_Governor_13");
			link.l1.go = "POR_M1_3";
			link.l2 = StringFromKey("por_Governor_15");
			link.l2.go = "exit";
			POR_M1_Start();
		break;

		case "POR_M1_3":
			dialog.text = StringFromKey("por_Governor_14");
			link.l1 = StringFromKey("por_Governor_15");
			link.l1.go = "exit";
		break;

		case "POR_M1_R1":
			dialog.text = StringFromKey("por_Governor_18");
			link.l1 = StringFromKey("por_Governor_19");
			link.l1.go = "POR_M1_R2";
		break;

		case "POR_M1_R2":
			dialog.text = StringFromKey("por_Governor_20");
			link.l1 = StringFromKey("por_Governor_21");
			link.l1.go = "POR_M2_1";
			POR_M1_Reward();
		break;

		// ---- missao 2
		case "POR_M2_1":
			dialog.text = StringFromKey("por_Governor_22");
			link.l1 = StringFromKey("por_Governor_23");
			link.l1.go = "exit";
			POR_M2_Start();
		break;

		case "POR_M2_R1":
			dialog.text = StringFromKey("por_Governor_26");
			link.l1 = StringFromKey("por_Governor_27");
			link.l1.go = "POR_M2_R2";
			if (CheckAttribute(pchar, "questTemp.PorLine.Letters"))
			{
				link.l2 = StringFromKey("por_Governor_28");
				link.l2.go = "POR_M2_R3";
			}
		break;

		case "POR_M2_R2":
			dialog.text = StringFromKey("por_Governor_29");
			link.l1 = StringFromKey("por_Governor_31");
			link.l1.go = "POR_M3_1";
			DeleteAttribute(pchar, "questTemp.PorLine.Letters");
			POR_M2_Reward();
		break;

		case "POR_M2_R3":
			dialog.text = StringFromKey("por_Governor_30");
			link.l1 = StringFromKey("por_Governor_31");
			link.l1.go = "POR_M3_1";
			pchar.questTemp.PorLine.LettersKept = true;
			POR_M2_Reward();
		break;

		// ---- missao 3
		case "POR_M3_1":
			dialog.text = StringFromKey("por_Governor_32");
			link.l1 = StringFromKey("por_Governor_33");
			link.l1.go = "POR_M3_2";
			link.l2 = StringFromKey("por_Governor_35");
			link.l2.go = "exit";
			POR_M3_Start();
		break;

		case "POR_M3_2":
			dialog.text = StringFromKey("por_Governor_34");
			link.l1 = StringFromKey("por_Governor_35");
			link.l1.go = "exit";
		break;

		case "POR_M3_R1":
			if (pchar.questTemp.PorLine == "M3_lost")
			{
				dialog.text = StringFromKey("por_Governor_40");
			}
			else
			{
				dialog.text = StringFromKey("por_Governor_38");
			}
			link.l1 = StringFromKey("por_Governor_21");
			link.l1.go = "POR_M4_1";
			POR_M3_Reward();
		break;

		// ---- missao 4
		case "POR_M4_1":
			dialog.text = StringFromKey("por_Governor_41");
			link.l1 = StringFromKey("por_Governor_42");
			link.l1.go = "POR_M4_2";
			link.l2 = StringFromKey("por_Governor_44");
			link.l2.go = "exit";
			POR_M4_Start();
		break;

		case "POR_M4_2":
			dialog.text = StringFromKey("por_Governor_43");
			link.l1 = StringFromKey("por_Governor_44");
			link.l1.go = "exit";
		break;

		case "POR_M4_R1":
			dialog.text = StringFromKey("por_Governor_47");
			link.l1 = StringFromKey("por_Governor_48");
			link.l1.go = "POR_M4_R3";
			link.l2 = StringFromKey("por_Governor_49");
			link.l2.go = "POR_M4_R2";
		break;

		case "POR_M4_R2":
			dialog.text = StringFromKey("por_Governor_50");
			link.l1 = StringFromKey("por_Governor_51");
			link.l1.go = "POR_M4_R3";
			pchar.questTemp.PorLine.LedgerGiven = true;
		break;

		case "POR_M4_R3":
			dialog.text = StringFromKey("por_Governor_52");
			link.l1 = StringFromKey("por_Governor_53");
			link.l1.go = "exit";
			POR_M4_Reward();
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // se sair por return dentro do switch, descarregar
}
