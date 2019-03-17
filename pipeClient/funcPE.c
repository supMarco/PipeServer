#pragma once
#include "includes.h"

WORD get_machine_type(BYTE * exe)
{
	IMAGE_DOS_HEADER * idh = exe;
	if (idh->e_magic == IMAGE_DOS_SIGNATURE) //Basic check
	{
		IMAGE_NT_HEADERS * inh = exe + idh->e_lfanew;
		if (inh->Signature == IMAGE_NT_SIGNATURE) //Basic check
		{
			IMAGE_OPTIONAL_HEADER ioh = inh->OptionalHeader;
			return ioh.Magic; //IMAGE_NT_OPTIONAL_HDR32_MAGIC / IMAGE_NT_OPTIONAL_HDR64_MAGIC
		}
	}
	return -1;
}

DWORD get_exported_functions(BYTE * dll, BYTE ** names)
{
	IMAGE_DOS_HEADER * idh = dll;
	DWORD name = NULL;
	if (idh->e_magic == IMAGE_DOS_SIGNATURE) //Basic check
	{
		IMAGE_NT_HEADERS * inh = dll + idh->e_lfanew;
		if (inh->Signature == IMAGE_NT_SIGNATURE) //Basic check
		{
			IMAGE_DATA_DIRECTORY idd = (inh->OptionalHeader).DataDirectory[0]; //Export symbols
			IMAGE_EXPORT_DIRECTORY * ied = dll + rva_to_file_offset(dll, idd.VirtualAddress);


			for (int i = 0; i < ied->NumberOfNames; i++)
			{
				name = dll + rva_to_file_offset(dll, (ied->AddressOfNames) + sizeof(DWORD) * i); //Name RVA
				name = dll + rva_to_file_offset(dll, *(DWORD *)name); //Name
				names[i] = calloc(1, strlen(name) + 1);
				strcpy(names[i], name);
			}
			return ied->NumberOfNames;
		}

	}
	return -1;

}

DWORD rva_to_file_offset(BYTE * dll, DWORD rva)
{
	IMAGE_DOS_HEADER * idh = dll;
	DWORD sections = NULL, virtualAddress = NULL, pointerToRawData = NULL;;
	if (idh->e_magic == IMAGE_DOS_SIGNATURE) //Basic check
	{
		IMAGE_NT_HEADERS * inh = dll + idh->e_lfanew;
		if (inh->Signature == IMAGE_NT_SIGNATURE) //Basic check
		{
			IMAGE_SECTION_HEADER * ish = (BYTE *)inh + sizeof(IMAGE_NT_HEADERS);
			sections = (inh->FileHeader).NumberOfSections;

			do
			{
				virtualAddress = ish->VirtualAddress;
				pointerToRawData = ish->PointerToRawData;
				(BYTE *)ish += sizeof(IMAGE_SECTION_HEADER);
			} while (sections-- && (rva >= ish->VirtualAddress));

			if (sections) return (rva - virtualAddress + pointerToRawData);
		}
	}
	return -1;
}