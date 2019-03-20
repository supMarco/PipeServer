#pragma once
#include "includes.h"


WORD get_machine_type(BYTE * pe)
{
	IMAGE_DOS_HEADER * idh = (IMAGE_DOS_HEADER*)pe;
	if (idh->e_magic == IMAGE_DOS_SIGNATURE)
	{
		IMAGE_NT_HEADERS * inh = (IMAGE_NT_HEADERS*)(pe + idh->e_lfanew);
		if (inh->Signature == IMAGE_NT_SIGNATURE)
		{
			IMAGE_OPTIONAL_HEADER ioh = inh->OptionalHeader;
			return ioh.Magic; //IMAGE_NT_OPTIONAL_HDR32_MAGIC / IMAGE_NT_OPTIONAL_HDR64_MAGIC
		}
	}
	return -1;
}


DWORD get_exported_functions_x64_x86(BYTE * dll, BYTE ** names, WORD machine)
{
#ifdef BUILD64
	DWORD64 name = NULL;
#else
	DWORD name = NULL;
#endif

	DWORD signature = NULL;
	IMAGE_NT_HEADERS32 * image_nt_headers32 = NULL;
	IMAGE_NT_HEADERS * image_nt_headers64 = NULL;
	IMAGE_DOS_HEADER * image_dos_header = NULL;
	IMAGE_DATA_DIRECTORY image_data_directory = { 0 };
	IMAGE_EXPORT_DIRECTORY * image_export_directory = { 0 };

	image_dos_header = (IMAGE_DOS_HEADER*)dll;
	if ((image_dos_header->e_magic == IMAGE_DOS_SIGNATURE) && (machine == IMAGE_NT_OPTIONAL_HDR64_MAGIC || machine == IMAGE_NT_OPTIONAL_HDR32_MAGIC))
	{
		if (machine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			image_nt_headers64 = (IMAGE_NT_HEADERS64*)(dll + image_dos_header->e_lfanew);
			signature = image_nt_headers64->Signature;
		}
		else
		{
			image_nt_headers32 = (IMAGE_NT_HEADERS32*)(dll + image_dos_header->e_lfanew);
			signature = image_nt_headers32->Signature;
		}

		if (signature == IMAGE_NT_SIGNATURE)
		{
			if (machine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
				image_data_directory = (image_nt_headers64->OptionalHeader).DataDirectory[0]; //Export symbols
			else
				image_data_directory = (image_nt_headers32->OptionalHeader).DataDirectory[0]; //Export symbols

			image_export_directory = (IMAGE_EXPORT_DIRECTORY*)(dll + rva_to_file_offset_x64_x86(dll, image_data_directory.VirtualAddress, machine));

			for (unsigned int i = 0; i < image_export_directory->NumberOfNames; i++)
			{
#ifdef BUILD64
				name = (DWORD64)(dll + rva_to_file_offset_x64_x86(dll, (image_export_directory->AddressOfNames) + sizeof(DWORD) * i, machine)); //Name RVA
				name = (DWORD64)(dll + rva_to_file_offset_x64_x86(dll, *(DWORD *)name, machine)); //Name
#else
				name = (DWORD)(dll + rva_to_file_offset_x64_x86(dll, (image_export_directory->AddressOfNames) + sizeof(DWORD) * i, machine)); //Name RVA
				name = (DWORD)(dll + rva_to_file_offset_x64_x86(dll, *(DWORD *)name, machine)); //Name
#endif
				names[i] = (BYTE*)calloc(1, strlen((const char *)name) + 1);
				strcpy((char *)names[i], (const char *)name);
			}
			return image_export_directory->NumberOfNames;
		}
	}
	return -1;

}

DWORD rva_to_file_offset_x64_x86(BYTE * dll, DWORD rva, WORD machine)
{
	DWORD signature = NULL;
	DWORD sections = NULL;
	DWORD virtualAddress = NULL;
	DWORD pointerToRawData = NULL;
	IMAGE_NT_HEADERS32 * image_nt_headers32 = NULL;
	IMAGE_NT_HEADERS * image_nt_headers64 = NULL;
	IMAGE_DOS_HEADER * image_dos_header = NULL;
	IMAGE_SECTION_HEADER * image_section_header = NULL;
	BYTE * temp;

	image_dos_header = (IMAGE_DOS_HEADER*)dll;

	if ((image_dos_header->e_magic == IMAGE_DOS_SIGNATURE) && (machine == IMAGE_NT_OPTIONAL_HDR64_MAGIC || machine == IMAGE_NT_OPTIONAL_HDR32_MAGIC))
	{
		if (machine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			image_nt_headers64 = (IMAGE_NT_HEADERS64*)(dll + image_dos_header->e_lfanew);
			signature = image_nt_headers64->Signature;
		}
		else
		{
			image_nt_headers32 = (IMAGE_NT_HEADERS32*)(dll + image_dos_header->e_lfanew);
			signature = image_nt_headers32->Signature;
		}

		if (signature == IMAGE_NT_SIGNATURE)
		{
			if (machine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
			{
				image_section_header = (IMAGE_SECTION_HEADER*)((BYTE *)image_nt_headers64 + sizeof(IMAGE_NT_HEADERS));
				sections = (image_nt_headers64->FileHeader).NumberOfSections;
			}
			else
			{
				image_section_header = (IMAGE_SECTION_HEADER*)((BYTE *)image_nt_headers32 + sizeof(IMAGE_NT_HEADERS32));
				sections = (image_nt_headers32->FileHeader).NumberOfSections;
			}

			do
			{
				virtualAddress = image_section_header->VirtualAddress;
				pointerToRawData = image_section_header->PointerToRawData;
				temp = (BYTE *)image_section_header;
				temp += sizeof(IMAGE_SECTION_HEADER);
				image_section_header = (IMAGE_SECTION_HEADER*)temp;
			} while (sections-- && (rva >= image_section_header->VirtualAddress));

			if (sections) return (rva - virtualAddress + pointerToRawData);
		}
	}
	return -1;
}