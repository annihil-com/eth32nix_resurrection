// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

#include "CDetour.h"
#include "unixtypes.h"
#include <dis-asm.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

typedef uintptr_t ptr_size_t;

#define ASM_JUMP 0xe9
#define ASM_CALL 0xe8
#define ASM_NOP 0x90
#define ASM_MOV_VALUE_TO_EBX 0xbb
#define ASM_GET_PC_THUNK "\x8b\x1c\x24\xc3"
#define ASM_JUMP_SIZE 5
#define ASM_CALL_SIZE 5

#define GET_PAGE(addr) ((void *)(((uintptr_t)addr) & ~((uintptr_t)(getpagesize() - 1))))
#define unprotect(addr) (mprotect(GET_PAGE(addr), getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC))
#define reprotect(addr) (mprotect(GET_PAGE(addr), getpagesize(), PROT_READ | PROT_EXEC))

CDetour Detour;

// Disassembler callbacks
int my_buffer_read_memory(bfd_vma from, bfd_byte *to, unsigned int length, struct disassemble_info *info) {
	memcpy((void *)to, (void *)(ptr_size_t)from, length);
	return 0;
}

void my_perror_memory(int status, bfd_vma memaddr, struct disassemble_info *info) {
	info->fprintf_func(info->stream, "CDetour: Unknown error %d\n", status);
	exit(1);
}

// For disable disassembler output
fprintf_ftype my_fprintf(FILE *stream, const char *format, ...) {
	return 0;
}

CDetour::CDetour()
{

}

CDetour::~CDetour()
{

}

// Return value:
//  1 if stripped
//  0 if not stripped
// -1 if error
int CDetour::IsStripped(char *filename)
{
	Elf *elf = initElf(filename);
	if (!elf)
		return -1;

	Elf_Scn *scn1 = getSection(elf, ".symtab");
	Elf_Scn *scn2 = getSection(elf, ".dynsym");

	elf_end(elf);

	return ((scn1 == NULL) && (scn2 == NULL));
}

void *CDetour::CreateDetour(void *orig, void *dest)
{
	// Sanity check
	if (!orig || !dest) FATAL_ERROR("bad args, orig: %p tramp: %p", orig, dest)

	// Get instructions length
	int iLen = 0;
	while (iLen < ASM_JUMP_SIZE)
		iLen += disassemble_x86(orig + iLen);

	// Backup instructions before it's override by the jump
	void *tramp = malloc(iLen + ASM_JUMP_SIZE);
	// Sanity check
	if (!tramp) FATAL_ERROR("malloc failed")
	int iCount = 0;
	while (iCount < iLen) {
		int len = disassemble_x86(orig + iCount);

		// Sanity check
		if (len < 1) FATAL_ERROR("can't disassemble at %p", orig)

		// Search for a call in the backuped instructions and correct his value (because call are relative)
		if ((len == ASM_CALL_SIZE) && (*(unsigned char *)(orig + iCount) == ASM_CALL)) {
			// Check if the call is "__i686.get_pc_thunk.bx", if yes replace it by mov $offset, %ebx
			if (!memcmp((void *)((orig + iCount + 5) + *(uintptr_t *)(orig + iCount + 1)), ASM_GET_PC_THUNK, sizeof(ASM_GET_PC_THUNK) - 1)) {
				*(unsigned char *)(tramp + iCount) = ASM_MOV_VALUE_TO_EBX;
				*(void **)(tramp + iCount + 1) = (void *)(orig + iCount + 5);
			} else {
				*(unsigned char *)(tramp + iCount) = ASM_CALL;
				*(void **)((uintptr_t)tramp + iCount + 1) = *(uintptr_t **)(((uintptr_t)orig + iCount + 1) - (((uintptr_t)tramp + iCount + 1) - ((uintptr_t)orig + iCount + 1)));
			}
		// If not a call, simply copy the instruction
		} else {
			memcpy(tramp + iCount, orig + iCount, len);
		}
		iCount += len;
	}

	// Write a jump to the original function after the backuped intructions
	*(unsigned char *)(tramp + iLen) = ASM_JUMP;
	*(void **)(tramp + iLen + 1) = (void *)((((uintptr_t)orig) + iLen) - (uintptr_t)(tramp + iLen + ASM_JUMP_SIZE));

	// Write a jump to the detour at the original function
	unprotect(orig);
	*(unsigned char *)orig = ASM_JUMP;
	*(void **)((uintptr_t)orig + 1) = (void *)(((uintptr_t)dest) - (((uintptr_t)orig) + ASM_JUMP_SIZE));
	reprotect(orig);

	return tramp;
}

void CDetour::RemoveDetour(void *orig, void *tramp)
{
	int iLen = 0;
	while (iLen < ASM_JUMP_SIZE)
		iLen += disassemble_x86(tramp + iLen);

	unprotect(orig);
	memcpy(orig, tramp, iLen);
	reprotect(orig);

	free(tramp);
}

int CDetour::disassemble_x86(void *addr)
{
	// Initialize info for disassembler
	disassemble_info info;
	init_disassemble_info(&info, stdout, (fprintf_ftype)my_fprintf);
	info.mach = bfd_mach_i386_i386;
	info.read_memory_func = my_buffer_read_memory;
	info.memory_error_func = my_perror_memory;

	// Disassemble instruction
	return print_insn_i386((bfd_vma)(ptr_size_t)addr, &info);
}

Elf *CDetour::initElf(const char *filename)
{
	if (elf_version(EV_CURRENT) == EV_NONE) FATAL_ERROR("outdated libelf")

	if (!filename)
		filename = "/proc/self/exe";

	int fd = open(filename, O_RDONLY);
	Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
	
	/*
	 * Alexplay: Don't close the file, otherwise it segfaults!!!
	 * More info: http://www.gnu.org/software/libc/manual/html_node/Opening-and-Closing-Files.html
	 * int close (int filedes):
	 * This function is a cancellation point in multi-threaded programs.
	 * This is a problem if the thread allocates some resources (like memory, file descriptors, semaphores or whatever)
	 * at the time close is called. If the thread gets canceled these resources stay allocated until the program ends.
	 * To avoid this, calls to close should be protected using cancellation handlers.
	*/

	//close(fd);

	if (!elf) FATAL_ERROR("%s invalid elf", filename)

	return elf;
}

Elf_Scn *CDetour::getSection(Elf *elf, const char *sectionName)
{	
	GElf_Ehdr ehdr;
	gelf_getehdr(elf, &ehdr);

	// Loop through sections
	Elf_Scn *scn = NULL;
	while ((scn = elf_nextscn(elf, scn)) != NULL) {
		GElf_Shdr shdr;
		gelf_getshdr(scn, &shdr);

		if (!strcmp(elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name), sectionName))
			break;
	}

	return scn;
}

GElf_Shdr *CDetour::getSectionHeader(const char *sectionName) {return getSectionHeader(NULL, sectionName);}
GElf_Shdr *CDetour::getSectionHeader(const char *filename, const char *sectionName)
{
	Elf *elf = initElf(filename);
	if (!elf) FATAL_ERROR("initElf failed (arg=%s)", sectionName)
				GElf_Ehdr ehdr;
	gelf_getehdr(elf, &ehdr);

	static GElf_Shdr shdr;
	memset(&shdr, 0, sizeof(GElf_Shdr));

	Elf_Scn *scn = getSection(elf, sectionName);
	if (!scn)
		return NULL;

	gelf_getshdr(scn, &shdr);

	return &shdr;
}

GElf_Sym *CDetour::getSymbol(char *filename, char *symbol)
{
	Elf *elf = initElf(filename);

	if (!elf)
		return NULL;

	static GElf_Sym sym;

	// Sections where to search for symbol
	const char *sections[] = {
		".symtab",
		".dynsym"
	};

	int sCount = 0;
	for (; sCount < (sizeof(sections) / sizeof(char *)); sCount++) {
		Elf_Scn *scn = getSection(elf, sections[sCount]);
		if (!scn)
			continue;

		GElf_Shdr shdr;
		gelf_getshdr(scn, &shdr);

		Elf_Data *data = elf_getdata(scn, NULL);
		if (!data)
			continue;

		// Search the symbol
		int count = 0;
		for (; count < (shdr.sh_size / shdr.sh_entsize); count++) {
			gelf_getsym(data, count, &sym);

			// Symbol found
			if (!strcmp(elf_strptr(elf, shdr.sh_link, sym.st_name), symbol)) {
				elf_end(elf);
				return &sym;
			}
		}
	}

	// Symbol not found
	elf_end(elf);
	return NULL;
}

bool CDetour::GOTHook(const char *symbol, void *hook)
{
	Elf *elf = initElf(NULL);
	if (!elf)
		return 0;

	// Get symbol hash
	int symbolHash = -1;

	Elf_Scn *scn = getSection(elf, ".dynsym");
	if (!scn) FATAL_ERROR(".dynsym section not found")

	GElf_Shdr shdr;
	gelf_getshdr(scn, &shdr);

	Elf_Data *data = elf_getdata(scn, NULL);
	if (!data)
		return 0;

	// Search the symbol
	int count = 0;
	for (; count < (shdr.sh_size / shdr.sh_entsize); count++) {
		GElf_Sym sym;
		gelf_getsym(data, count, &sym);
		if (!strcmp(elf_strptr(elf, shdr.sh_link, sym.st_name), symbol)) {
			symbolHash = count;
			break;
		}
	}

	// Sanity check
	if (symbolHash == -1)
		return 0;

	// Search symbol hash in "Relocation Procedure Linkage Table" section

	scn = getSection(elf, ".rel.plt");
	if (!scn) FATAL_ERROR(".rel.plt section not found")

	gelf_getshdr(scn, &shdr);

	data = elf_getdata(scn, NULL);
	if (!data)
		return 0;

	// Search the symbol
	count = 0;
	for (; count < (shdr.sh_size / shdr.sh_entsize); count++) {
		GElf_Rel rel;
		gelf_getrel(data, count, &rel);

		// If symbol found
		if (ELF64_R_SYM(rel.r_info) == symbolHash) {
			*(void **)(unsigned int)rel.r_offset = hook;
			elf_end(elf);
			return 1;
		}
	}

	// Symbol not found
	elf_end(elf);
	return 0;
}
