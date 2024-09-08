/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "kuhl_m.h"
#include "../modules/kull_m_process.h"

const KUHL_M kuhl_m_winmine;

NTSTATUS kuhl_m_winmine_infos(int argc, wchar_t* argv[]);

typedef struct _STRUCT_MON_DEMINEUR {
	DWORD nbMines;
	DWORD longueur;
	DWORD hauteur;
	DWORD alignOffset;
	BYTE tabMines[26][32];
} STRUCT_MON_DEMINEUR, *PSTRUCT_MON_DEMINEUR;
