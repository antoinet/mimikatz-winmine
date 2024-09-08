/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kuhl_m_winmine.h"

const CHAR DISP_WINMINE[] = " 123456789*x*?F.";

const KUHL_M_C kuhl_m_c_winmine[] = {
	{kuhl_m_winmine_infos, L"infos", L"infos"},
};

const KUHL_M kuhl_m_winmine = {
	L"winmine", L"WinMine module", NULL,
	ARRAYSIZE(kuhl_m_c_winmine), kuhl_m_c_winmine, NULL, NULL
};

NTSTATUS kuhl_m_winmine_infos(int argc, wchar_t* argv[])
{
	DWORD dwPid;
	HANDLE hProcess;
	PEB Peb;
	PIMAGE_NT_HEADERS pNtHeaders;
	KULL_M_MEMORY_SEARCH sMemory = { {{NULL, NULL}, 0}, NULL };
	KULL_M_MEMORY_ADDRESS aRemote = { NULL, NULL };
	PVOID ptrTemp = NULL;
	BYTE patternStartGame[] = { 0x6a, 0x04, 0xeb, 0x02, 0x6a, 0x06, 0x5b, 0xa3 };
	KULL_M_MEMORY_ADDRESS aBuffer = { patternStartGame, &KULL_M_MEMORY_GLOBAL_OWN_HANDLE };
	STRUCT_MON_DEMINEUR monDemineur = { 0, 0, 0, 0, {0} };

	if (kull_m_process_getProcessIdForName(L"winmine.exe", &dwPid)) {
		if (hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwPid))
		{
			if (kull_m_memory_open(KULL_M_MEMORY_TYPE_PROCESS, hProcess, &aRemote.hMemory))
			{
				if (kull_m_process_peb(aRemote.hMemory, &Peb, FALSE))
				{
					aRemote.address = Peb.ImageBaseAddress;
					if (kull_m_process_ntheaders(&aRemote, &pNtHeaders))
					{
						sMemory.kull_m_memoryRange.kull_m_memoryAdress.hMemory = aRemote.hMemory;
						sMemory.kull_m_memoryRange.kull_m_memoryAdress.address = (LPVOID)pNtHeaders->OptionalHeader.ImageBase;
						sMemory.kull_m_memoryRange.size = pNtHeaders->OptionalHeader.SizeOfImage;
						if (kull_m_memory_search(&aBuffer, sizeof(patternStartGame), &sMemory, TRUE))
						{
							aRemote.address = (PBYTE)sMemory.result + sizeof(patternStartGame);
							aBuffer.address = &ptrTemp;
							if (kull_m_memory_copy(&aBuffer, &aRemote, sizeof(PVOID)))
							{
								ptrTemp = (PVOID)((char*)ptrTemp - sizeof(PVOID));
								aRemote.address = ptrTemp;
								aBuffer.address = &monDemineur;
								if (kull_m_memory_copy(&aBuffer, &aRemote, sizeof(STRUCT_MON_DEMINEUR)))
								{
									//kprintf(L"Adresse         : 0x%08X\n", ptrTemp);
									kprintf(L"Mines           : %d\n", monDemineur.nbMines);
									kprintf(L"Dimension       : %d lignes x %d colonnes\n", monDemineur.hauteur, monDemineur.longueur);
									kprintf(L"Champ           :\n\n");

									for (DWORD y = 1; y <= monDemineur.hauteur; y++)
									{
										kprintf(L"\t");
										for (DWORD x = 1; x <= monDemineur.longueur; x++)
										{
											BYTE laCase = monDemineur.tabMines[y][x];
											kprintf(L" %c", (laCase & 0x80) ? '*' : DISP_WINMINE[laCase & 0x0f]);
										}
										kprintf(L"\n");
									}
								}
								else PRINT_ERROR(L"monDemineur copy\n");

							}
							else PRINT_ERROR(L"Copy ptrTemp\n");
						}
						else PRINT_ERROR(L"Search is KO\n");
					}
					else PRINT_ERROR(L"winmine NT Headers\n");
				}
				else PRINT_ERROR(L"winmine PEB\n");
				kull_m_memory_close(aRemote.hMemory);
			}
			CloseHandle(hProcess);
		}
		else PRINT_ERROR(L"OpenProcess\n");
	}
	else PRINT_ERROR(L"No winmine in memory!\n");

	return STATUS_SUCCESS;
}
