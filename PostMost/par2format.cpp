// par2format.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "par2format.h"

#include <assert.h>
#include <math.h>
#include <search.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PAR2

namespace par2 {

u32 CRC32::table[256]
#ifdef CRC_USE_TABLE
=
#include "crc32table.h"
#endif
;

#ifdef _DEBUG
bool ok_md5hash = MD5Hash::test();
#endif


u16 Galois16::logtable[Count];
u16 Galois16::alogtable[Count];

u16 Galois16::goodbase[Count];
u32 Galois16::ngoodbase;

bool Galois16OK = Galois16::init();

PostFiles::PostFiles(const char* _basename, int _nbfiles, int _partsize)
{
	par2common_size = 0;
	par2common_data = NULL;
	strncpy(basename,_basename,_MAX_PATH);
	memset(sfvname,0,_MAX_PATH);
	memset(parname,0,_MAX_PATH);
	memset(par2name,0,_MAX_PATH);
	nbfiles = _nbfiles;
	partsize = _partsize;
	nbrecvpart = 0;
	files = new fileinfo[nbfiles];
	memset(files,0,nbfiles*sizeof(fileinfo));
}

void PostFiles::initFile(int num, const char* fname, bool selected)
{
	if ((unsigned)num>=(unsigned)nbfiles)
		return; // incorrect number
	fileinfo* f = files+num;
	strncpy(f->name,fname,_MAX_PATH);
	if (selected) f->flags|=SELECTED;
	struct stat filestat;
	memset(&filestat,0,sizeof(filestat));
	stat(fname,&filestat);
	f->modiftime=filestat.st_mtime;
	f->size=filestat.st_size;
}

void PostFiles::calcFile(int num,int flags)
{
	if ((unsigned)num>=(unsigned)nbfiles)
		return; // incorrect number
	fileinfo* f = files+num;
	CRC32 ccrc;

	if (partsize && (flags&CALC_PART))
	{
		f->nbparts=((u32)f->size+(u32)partsize-1)/(u32)partsize;
		f->parts = new partinfo[f->nbparts];
		memset(f->parts,0,f->nbparts*sizeof(partinfo));
	}

	// compute crc & hash

	int fd = open(f->name,O_RDONLY|O_SEQUENTIAL|O_BINARY);

	if (fd!=-1)
	{
		MD5Hash hash;
		u32 crc = 0;
		int pos = 0;
		int s = 0;

		if (f->nbparts==0)
		{
			u8 buffer[4096];
			if (flags&CALC_CRC) crc = ccrc.crc_begin();
			if (flags&CALC_HASH) hash.hashInit();
			while ((s = read(fd,buffer,4096))==4096)
			{
				if (flags&CALC_CRC) crc = ccrc.crc_add(crc,buffer,s);
				if (flags&CALC_HASH)
					for (int i=0;i<4096;i+=64)
						hash.hashBlock64(buffer+i);
				pos += s;
				if (pos==16*1024)
				{ // 16KB head
					if (flags&CALC_CRC) f->head16k.crc32=ccrc.crc_end(crc);
					if (flags&CALC_HASH)
					{
						MD5Hash hashcopy = hash;
						hashcopy.hashEnd(NULL,0);
						hashcopy.copyHash16(f->head16k.md5hash);
					}
				}
			}
			if (flags&CALC_CRC) crc = ccrc.crc_end(ccrc.crc_add(crc,buffer,s));
			if (flags&CALC_HASH) hash.hashEnd(buffer,s);
			if (flags&CALC_CRC) f->total.crc32=crc;
			if (flags&CALC_HASH) hash.copyHash16(f->total.md5hash);
			if (pos<16*1024)
				f->head16k=f->total;
		}
		else
		{ // multipart stat
			u8 buffer[4096];
			u32 crcpart;
			MD5Hash hashpart;
			int p0 = 0; // start of current part
			int pnum = 0; // nunber of current part
			crc = ccrc.crc_begin();
			hash.hashInit();
			while ((s = read(fd,buffer,4096))==4096)
			{
				// PARTS
				while (pos+s >= p0+partsize) // the current part is entirely read
				{ // next part
					if (pnum==0)
					{ // first part
						crcpart = crc;
						hashpart = hash;
					}
					if (p0<=pos)
					{ // already started part
						crcpart = ccrc.crc_end(ccrc.crc_add(crcpart,buffer,p0+partsize-pos));
						hashpart.hashEnd(buffer,p0+partsize-pos);
					}
					else
					{ // same buffer, new part...
						crcpart = ccrc.crc_end(ccrc.crc_add(crcpart,buffer+(p0-pos),partsize));
						hashpart.hashEnd(buffer+(p0-pos),partsize);
					}
					f->parts[pnum].crc32=crcpart;
					hashpart.copyHash16(f->parts[pnum].md5hash);
					crcpart=ccrc.crc_begin();
					hashpart.hashInit();
					++pnum;
					p0+=partsize;
				}
				// update part crc/hash
				if (pnum>0) // first part is not computed (same as total)
				{
					if (p0<=pos)
					{ // part at the beginning
						crcpart = ccrc.crc_add(crcpart,buffer,s);
						for (int i=0;i<s;i+=64)
							hashpart.hashBlock64(buffer+i);
					}
					else
					{ // part at the end
						crcpart = ccrc.crc_add(crcpart,buffer+(p0-pos),(pos+s)-p0);
						for (int i=(p0-pos);i<s;i+=64)
							hashpart.hashBlock64(buffer+i);
					}
				}
				// END PARTS
				// update total crc/hash
				crc = ccrc.crc_add(crc,buffer,s);
				for (int i=0;i<s;i+=64)
					hash.hashBlock64(buffer+i);
				pos += s;
				if (pos==16*1024)
				{ // 16KB head
					f->head16k.crc32=ccrc.crc_end(crc);
					MD5Hash hashcopy = hash;
					hashcopy.hashEnd(NULL,0);
					hashcopy.copyHash16(f->head16k.md5hash);
				}
			}
			// LAST DATA CHUNCK
			// PARTS
			while (pos+s >= p0+partsize)
			{ // next part
				if (pnum==0)
				{ // first part
					crcpart = crc;
					hashpart = hash;
				}
				if (p0<=pos)
				{ // already started part
					crcpart = ccrc.crc_end(ccrc.crc_add(crcpart,buffer,p0+partsize-pos));
					hashpart.hashEnd(buffer,p0+partsize-pos);
				}
				else
				{ // same buffer, new part...
					crcpart = ccrc.crc_end(ccrc.crc_add(crcpart,buffer+(p0-pos),partsize));
					hashpart.hashEnd(buffer+(p0-pos),partsize);
				}
				f->parts[pnum].crc32=crcpart;
				hashpart.copyHash16(f->parts[pnum].md5hash);
				crcpart=ccrc.crc_begin();
				hashpart.hashInit();
				++pnum;
				p0+=partsize;
			}
			if (pos+s>p0)
			{ // last chunck (must pad with zeros)
				if (pnum==0)
				{ // first part
					crcpart = crc;
					hashpart = hash;
				}
				int pmax=(p0<pos)?pos:p0;
				crcpart = ccrc.crc_add(crcpart,buffer+(pmax-pos),pos+s-pmax);
				// pad with zeros
				crcpart = ccrc.crc_add_null(crcpart,(p0+partsize)-(pos+s));
				f->parts[pnum].crc32 = ccrc.crc_end(crcpart);

				while (pmax+64<=pos+s)
				{
					hashpart.hashBlock64(buffer+(pmax-pos));
					pmax+=64;
				}
				if (pmax<pos+s)
				{ // less than 64 bytes left
					memset(buffer+s,0,(pmax+64)-(pos+s)); // pad with zero
					hashpart.hashBlock64(buffer+(pmax-pos)); // add in hash
					pmax+=64;
				}

				u8 buf0[64];
				memset(buf0,0,64);
				while (pmax+64<=p0+partsize)
				{
					hashpart.hashBlock64(buf0);
					pmax+=64;
				}

				hashpart.hashEnd(buf0,p0+partsize-pmax);
				hashpart.copyHash16(f->parts[pnum].md5hash);

				crcpart=ccrc.crc_begin();
				hashpart.hashInit();
				++pnum;
				p0+=partsize;
				
			}
			// END PARTS
			crc = ccrc.crc_end(ccrc.crc_add(crc,buffer,s));
			hash.hashEnd(buffer,s);
			f->total.crc32=crc;
			hash.copyHash16(f->total.md5hash);
			if (pos<16*1024)
				f->head16k=f->total;
		}
	}

	close(fd);
}


static const char* stripdir(const char* fname)
{
	const char* lastp = fname;
	while (*fname)
	{
		if (*fname=='/' || *fname=='\\') lastp=fname+1;
		++fname;
	}
	return lastp;
}

void PostFiles::calcbasename()
{
	int i=0;
	int f;
	char c;
	if (nbfiles<=0) return;
	while (1)
	{
		c=files[0].name[i];
		for (f=1;f<nbfiles;f++)
			if (files[f].name[i]!=c) break;
		if (c=='\0' || f<nbfiles)
		{ //end
			basename[i]='\0';
			break;
		}
		//continue
		basename[i]=c;
		++i;
	}
	// then remove any exception
	for (f=0;f<nbfiles;f++)
	{
		char* ext = fileext(files[f].name);
		if (ext!=files[f].name && (ext-files[f].name)<i)
		{
			i=ext-files[f].name;
			basename[i]='\0';
		}
	}
}

const char* PostFiles::generateSFV(const char* fname)
{
	if (fname[0]=='\0')
	{
		if (basename[0]=='\0') calcbasename();
		if (sfvname[0]=='\0')
			_snprintf(sfvname,_MAX_PATH,"%s.sfv",basename);
	}
	else
		strncpy(sfvname,fname,_MAX_PATH);
	FILE* fout = fopen(sfvname,"w");
	if (fout==NULL)
	{
		return NULL;
	}
	time_t now = time(NULL);
	tm stime;
	int i;

	stime = *localtime(&now);

	fprintf(fout,"; Generated by WIN-SFV32 v1.1a (" TITLE ") on %4d-%02d-%02d at %02d:%02d:%02d\n",
		stime.tm_year+1900,stime.tm_mon+1,stime.tm_mday,
		stime.tm_hour,stime.tm_min,stime.tm_sec);
	fprintf(fout,";\n");
	for (i=0;i<nbfiles;i++)
	{
		stime = *localtime(&files[i].modiftime);
		fprintf(fout,"; %12d  %02d:%02d.%02d %4d-%02d-%02d %s\n",
			(u32)files[i].size,
			stime.tm_hour,stime.tm_min,stime.tm_sec,
			stime.tm_year+1900,stime.tm_mon+1,stime.tm_mday,
			stripdir(files[i].name));
	}

	for (i=0;i<nbfiles;i++)
	{
		fprintf(fout,"%s %08X\n",stripdir(files[i].name),files[i].total.crc32);
	}
	fclose(fout);
	return sfvname;
}

#pragma warning(disable:4200)

#pragma pack(push)
#pragma pack (1)

struct par_header
{
	char id_string[8];
	u32 version;
	u32 program;
	hash16 hash;
	hash16 sethash;
	u64 volume;
	u64 nbfiles;
	u64 filelistoffset;
	u64 filelistsize;
	u64 dataoffset;
	u64 datasize;
};

struct par_file
{
	u64 entrysize;
	u64 status;
	u64 filesize;
	hash16 filehash;
	hash16 headhash;
	WCHAR name[0];
};

#pragma pack(pop)

#pragma pack(push)
#pragma pack (1)

struct par2_header
{
	static const char* MAGIC; // = "PAR2\0PKT";
	char magic[8];
	u64  length;
	hash16 hash;
	hash16 setid;
	char type[16];
	//u8 body[0];
	u8* body() { return (u8*)(this+1); }
};

const char* par2_header::MAGIC = "PAR2\0PKT";

struct par2_main
{
	static const char* TYPE; // = "PAR 2.0\0Main\0\0\0\0";
	par2_header header;
	u64 slice_size;
	int nbfiles_rec;
	//hash16 files_hash[0];
	hash16* files_hash() { return (hash16*)(this+1); }
	const hash16* files_hash() const { return (const hash16*)(this+1); }
};
const char* par2_main::TYPE = "PAR 2.0\0Main\0\0\0\0";

struct par2_creator
{
	static const char* TYPE; // = "PAR 2.0\0Creator\0";
	par2_header header;
	//char creator[0];
	char* creator() { return (char*)(this+1); }
	const char* creator() const { return (const char*)(this+1); }
};
const char* par2_creator::TYPE = "PAR 2.0\0Creator\0";

static const char* CREATOR = TITLE " contact: Assert & Alain in newsgroup alt.binaries.cartoons.french.d";

struct par2_file_desc
{
	static const char* TYPE; // = "PAR 2.0\0FileDesc";
	par2_header header;
	hash16 file_id;
	hash16 file_hash;
	hash16 head_hash;
	u64 file_length;
	//char filename[0];
	char* filename() { return (char*)(this+1); }
	const char* filename() const { return (char*)(this+1); }
};
const char* par2_file_desc::TYPE = "PAR 2.0\0FileDesc";

struct par2_file_slices
{
	static const char* TYPE; // = "PAR 2.0\0IFSC\0\0\0\0";
	par2_header header;
	hash16 file_id;
	//partinfo slices[0];
	partinfo* slices() { return (partinfo*)(this+1); }
	const partinfo* slices() const { return (const partinfo*)(this+1); }
};
const char* par2_file_slices::TYPE = "PAR 2.0\0IFSC\0\0\0\0";

struct par2_recv_slice
{
	static const char* TYPE; // = "PAR 2.0\0RecvSlic";
	par2_header header;
	u32 exponent;
	//u8 data[0];
	u8* data() { return (u8*)(this+1); }
	const u8* data() const { return (const u8*)(this+1); }
};
const char* par2_recv_slice::TYPE = "PAR 2.0\0RecvSlic";

#pragma pack(pop)

static void par2_write(FILE* f,void* packet)
{
	par2_header* header=(par2_header*) packet;
	memcpy(header->magic,par2_header::MAGIC,8);
	MD5Hash packethash;
	packethash.hashInit();
	packethash.hashEnd(((const u8*)packet)+32,(u32)header->length-32);
	packethash.copyHash16(header->hash);

	fwrite(packet,1,(u32)header->length,f);
}

int hash_compare( const void *arg1, const void *arg2 )
{
	//return memcmp(arg1,arg2,sizeof(hash16));
	const u32* d1 = (const u32*)arg1;
	const u32* d2 = (const u32*)arg2;

	if (d1[3]>d2[3]) return 1;
	if (d1[3]<d2[3]) return -1;
	if (d1[2]>d2[2]) return 1;
	if (d1[2]<d2[2]) return -1;
	if (d1[1]>d2[1]) return 1;
	if (d1[1]<d2[1]) return -1;
	if (d1[0]>d2[0]) return 1;
	if (d1[0]<d2[0]) return -1;
	return 0;
}

static inline u32 ceil4(u32 a)
{
	return (a+3)&(-4);
}

const char* PostFiles::generatePAR2(const char* fname)
{

	assert(sizeof(par2_header)==8+8+16+16+16);
	assert(sizeof(par2_main)==sizeof(par2_header)+8+4);
	assert(sizeof(par2_creator)==sizeof(par2_header));
	assert(sizeof(par2_file_desc)==sizeof(par2_header)+16+16+16+8);
	assert(sizeof(par2_file_slices)==sizeof(par2_header)+16);
	assert(sizeof(par2_recv_slice)==sizeof(par2_header)+4);

	if (fname[0]=='\0')
	{
		if (basename[0]=='\0') calcbasename();
		if (par2name[0]=='\0')
			_snprintf(par2name,_MAX_PATH,"%s.par2",basename);
	}
	else
		strncpy(par2name,fname,_MAX_PATH);

	int nbsel;
	int f,fi,i;
	nbsel=0;
	for (f=0;f<nbfiles;f++)
		if (files[f].flags&SELECTED)
			++nbsel;

	int length;

	par2_main* pmain=NULL;
	par2_file_desc** pfiledesc=new par2_file_desc*[nbfiles];
	par2_file_slices** pfileslices=new par2_file_slices*[nbfiles];
	par2_creator* pcreator=NULL;

	// FILE DESCRIPTION PACKETS

	for (f=0;f<nbfiles;f++)
	{
		const char* fname = stripdir(files[f].name);
		u32 namelength = strlen(fname);
		length = sizeof(par2_file_desc)+ceil4(namelength);
		pfiledesc[f] = (par2_file_desc*)malloc(length);
		// header
		pfiledesc[f]->header.length=length;
		memcpy(pfiledesc[f]->header.type,par2_file_desc::TYPE,16);
		// body
		pfiledesc[f]->file_hash = files[f].total.md5hash;
		pfiledesc[f]->head_hash = files[f].head16k.md5hash;
		pfiledesc[f]->file_length = files[f].size;
		memcpy(pfiledesc[f]->filename(),fname,namelength);
		for (i=namelength;i<(int)ceil4(namelength);i++)
			pfiledesc[f]->filename()[i]='\0';
		MD5Hash fileid;
		fileid.hashInit();
		fileid.hashEnd((const u8*)&(pfiledesc[f]->head_hash),16+8+namelength);
		fileid.copyHash16(pfiledesc[f]->file_id);
	}

	// MAIN PACKET

	length = sizeof(par2_main)+nbfiles*16;
	pmain=(par2_main*)malloc(length);
	// header
	pmain->header.length=length;
	memcpy(pmain->header.type,par2_main::TYPE,16);
	// body
	pmain->slice_size = partsize;
	pmain->nbfiles_rec = nbsel;

	// first add selected files	
	fi = 0;
	for (f=0;f<nbfiles;f++)
	{
		if (files[f].flags&SELECTED)
		{
			pmain->files_hash()[fi]=pfiledesc[f]->file_id;
			++fi;
		}
	}
	// and sort
	qsort(pmain->files_hash(), nbsel, sizeof(hash16), &hash_compare);

	// now add the unselected files
	for (f=0;f<nbfiles;f++)
	{
		if (!(files[f].flags&SELECTED))
		{
			pmain->files_hash()[fi]=pfiledesc[f]->file_id;
			++fi;
		}
	}
	// and sort...
	qsort(pmain->files_hash()+nbsel, nbfiles-nbsel, sizeof(hash16), &hash_compare);


	// FILE SLICES PACKETS
	//fi=0;
	for (f=0;f<nbfiles;f++)
	{
		if (files[f].flags&SELECTED)
		{
			length = sizeof(par2_file_slices)+sizeof(partinfo)*files[f].nbparts;
			pfileslices[f] = (par2_file_slices*)malloc(length);
			// header
			pfileslices[f]->header.length=length;
			memcpy(pfileslices[f]->header.type,par2_file_slices::TYPE,16);
			// body
			pfileslices[f]->file_id = pfiledesc[f]->file_id;
			memcpy(pfileslices[f]->slices(),files[f].parts,sizeof(partinfo)*files[f].nbparts);

			//++fi;
		}
		else pfileslices[f]=NULL;
	}

	// CREATOR PACKET
	const char* creator=CREATOR;
	int clen = strlen(creator);
	length = sizeof(par2_creator)+ceil4(clen);
	pcreator=(par2_creator*)malloc(length);
	// header
	pcreator->header.length=length;
	memcpy(pcreator->header.type,par2_creator::TYPE,16);
	// body
	memcpy(pcreator->creator(),creator,clen);
	for (i=clen;i<(int)ceil4(clen);i++)
		pcreator->creator()[i]='\0';


	// calculate the setid
	MD5Hash setid;
	setid.hashInit();
	setid.hashEnd(((const u8*)pmain)+sizeof(par2_header),(u32)pmain->header.length-sizeof(par2_header));
	setid.copyHash16(par2_setid);
	pmain->header.setid = par2_setid;

	// finally write everything
	const char* result = par2name;
	FILE *fout = fopen(par2name,"wb");
	if (fout==NULL)
	{
		result = NULL;
	}
	else
	{

		for (f=0;f<nbfiles;f++)
		{
			pfiledesc[f]->header.setid=par2_setid;
			par2_write(fout,pfiledesc[f]);
			if (pfileslices[f]!=NULL)
			{
				pfileslices[f]->header.setid=par2_setid;
				par2_write(fout,pfileslices[f]);
			}
		}

		par2_write(fout,pmain);

		pcreator->header.setid=par2_setid;
		par2_write(fout,pcreator);

		fclose(fout);
	}
	free(pmain);
	for (f=0;f<nbfiles;f++) free(pfiledesc[f]);
	for (f=0;f<nbfiles;f++) if (pfileslices[f]!=NULL) free(pfileslices[f]);
	free(pcreator);

	delete[] pfiledesc;
	delete[] pfileslices;

	return par2name;
}

PostFiles::~PostFiles()
{
	if (files!=NULL)
	{
		for (int i=0;i<nbfiles;i++)
			if (files[i].parts!=NULL)
				delete[] files[i].parts;
		delete[] files;
		files=NULL;
	}
	
}

class FileMemoryMappingBase
{
public:
	u32 map_size;
	//u8* map_base;
	HANDLE file;
	HANDLE mmap;
	char last_error[256];
	FileMemoryMappingBase()
	{
		map_size=0;
		//map_base=NULL;
		file = NULL;
		mmap = NULL;
		last_error[0]=0;
	}
	~FileMemoryMappingBase()
	{
		if (mmap!=NULL)
		{
			CloseHandle(mmap);
			mmap=NULL;
		}
		if (file!=NULL)
		{
			CloseHandle(file);
			file=NULL;
		}
	}
protected:
	bool open(const char* filename, bool write=false, u32 size=0, bool creat=false, void** ptr_base=NULL)
	{
		file = CreateFile(filename, // File name
			(write?GENERIC_READ|GENERIC_WRITE:GENERIC_READ), // access (read-write) mode
			FILE_SHARE_READ, // share mode
			NULL, // security attributes
			(write?(creat?CREATE_ALWAYS:OPEN_ALWAYS):OPEN_EXISTING), // how to create
			FILE_ATTRIBUTE_NORMAL, // file attributes
			NULL // handle to file with attributes to copy
		);
    if (file == INVALID_HANDLE_VALUE)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,last_error,sizeof(last_error),NULL);
			return false;
		}
		if (size==0)
			size = GetFileSize(file,NULL);
/*
		else
		{
			file
		}
*/
		map_size=size;
    mmap = CreateFileMapping(file, NULL, (write?PAGE_READWRITE:PAGE_READONLY), 0, size, NULL);
		if (mmap==NULL)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,last_error,sizeof(last_error),NULL);
			return false;
		}
		*ptr_base = (u8*) MapViewOfFile(mmap, (write?FILE_MAP_WRITE:FILE_MAP_READ), 0, 0, size);	
		if (*ptr_base==NULL)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,last_error,sizeof(last_error),NULL);
			return false;
		}
		return true;
	}
};

class FileMemoryMappingRead : public FileMemoryMappingBase
{
public:
	const u8* map_base;
	FileMemoryMappingRead()
	{
		map_base=NULL;
	}
	~FileMemoryMappingRead()
	{
		if (map_base!=NULL)
		{
			UnmapViewOfFile(map_base);
			map_base=NULL;
		}
	}
	bool open(const char* filename, u32 size=0)
	{
		return FileMemoryMappingBase::open(filename,false,size,false,(void**)&map_base);
	}
/*
	template <class T>
	const T& get(u32 pos)
	{
		return *(const T*)((map_base==NULL||pos>=map_size)?NULL:map_base+pos);
	}
*/
	const u8* ptr(u32 pos)
	{
		return ((map_base==NULL||pos>=map_size)?NULL:map_base+pos);
	}
};

template <class T>
const T& get(FileMemoryMappingRead& map,u32 pos)
{
	return *(const T*)map.ptr(pos);
}

class FileMemoryMappingWrite : public FileMemoryMappingBase
{
public:
	u8* map_base;
	FileMemoryMappingWrite()
	{
		map_base=NULL;
	}
	~FileMemoryMappingWrite()
	{
		if (map_base!=NULL)
		{
			UnmapViewOfFile(map_base);
			map_base=NULL;
		}
	}
	bool open(const char* filename, u32 size=0, bool create = false)
	{
		return FileMemoryMappingBase::open(filename,true,size,create,(void**)&map_base);
	}
/*
	template <class T>
	T& get(u32 pos)
	{
		return *(T*)((map_base==NULL||pos>=map_size)?NULL:map_base+pos);
	}
*/
	u8* ptr(u32 pos)
	{
		return ((map_base==NULL||pos>=map_size)?NULL:map_base+pos);
	}
};

template <class T>
T& get(FileMemoryMappingWrite& map,u32 pos)
{
	return *(T*)map.ptr(pos);
}

bool PostFiles::GetSetID(const char* par2file, hash16& setid)
{
	FileMemoryMappingRead par2map;
	if (par2map.open(par2file))
	{
		u32 pos = 0;
		const char* packet_id=par2_header::MAGIC;
		while (pos<par2map.map_size)
		{
			if ( get<u32>(par2map,pos)==*(u32*)packet_id
			  && get<u32>(par2map,pos+4)==*(u32*)(packet_id+4) )
			{ // found a new packet
				const par2_header& header = get<par2_header>(par2map,pos);
				setid = header.setid;
				return true;
			}
			pos+=4;
		}
	}
	return false;
}

PostFiles::PostFiles(const char* _par2name)
{
	par2common_size = 0;
	par2common_data = NULL;
	strncpy(par2name,_par2name,_MAX_PATH);
	memset(sfvname,0,_MAX_PATH);
	memset(parname,0,_MAX_PATH);
	// calc base name
	strncpy(basename,_par2name,_MAX_PATH);
	TCHAR* e = fileext(basename);
	if (!stricmp(e,".par2"))
	{
		*e='\0'; // remove extension
		e = fileext(basename);
		if (!strnicmp(e,".vol",4))
			*e='\0'; // remove volume info
	}

	char dir[_MAX_PATH];
	strncpy(dir,par2name,_MAX_PATH);

	char* dirsep=strrchr(dir,'\\');
	if (dirsep==NULL) dir[0]='\0';
	else *(dirsep+1)='\0';
	int dirlen = strlen(dir);

	nbfiles = 0;
	partsize = 0;
	files = NULL;
	nbrecvpart = 0;

	FileMemoryMappingRead par2map;
	if (par2map.open(_par2name))
	{
		CDWordArray dwa_packets;
		u32 pos = 0;
		const char* packet_id=par2_header::MAGIC;
		const par2_main* pmain;
		bool fileid_sorted;
		int f;
		while (pos<par2map.map_size)
		{
			if ( get<u32>(par2map,pos)==*(u32*)packet_id
			  && get<u32>(par2map,pos+4)==*(u32*)(packet_id+4) )
			{ // found a new packet
				//dwa_packets.Add(pos);
				const par2_header& header = get<par2_header>(par2map,pos);

				if (!memcmp(header.type,par2_recv_slice::TYPE,16))
				{
					const par2_recv_slice& recv = get<par2_recv_slice>(par2map,pos);
					u32 i=recv.exponent;
					/*
					for (i=0;i<Galois16::ngoodbase;i++)
					{
						if (recv.exponent==Galois16::goodbase[i]) break;
					}
					*/
					if (i<Galois16::ngoodbase && (int)i>=nbrecvpart)
						nbrecvpart = i+1;
				}
				else if (!memcmp(header.type,par2_main::TYPE,16))
				{ // found main header
					dwa_packets.Add(pos); // save its position
					pmain = &get<par2_main>(par2map,pos);
					nbfiles = pmain->nbfiles_rec;
					partsize = (int)pmain->slice_size;
					par2_setid = pmain->header.setid;
				}
				else if (!memcmp(header.type,par2_file_desc::TYPE,16))
				{ // found file_desc header
					dwa_packets.Add(pos); // save its position
				}
				else if (!memcmp(header.type,par2_file_slices::TYPE,16))
				{ // found file_slices header
					dwa_packets.Add(pos); // save its position
				}
				pos+=(u32)header.length;
			}
			else
				pos+=4;
		}
		if (nbfiles>0)
		{
			// check if files are sorted
			fileid_sorted = true;
			for (f=0;f<nbfiles-1;f++)
			{
				if (hash_compare(pmain->files_hash()+f,pmain->files_hash()+(f+1))>0)
				{
					CString buf; buf.Format("WARNING: PAR2 main packet in %s INVALID: files entries not sorted!",par2name);
					AfxMessageBox(buf);
					fileid_sorted = false;
					break;
				}
			}
			int p;
			files = new fileinfo[nbfiles];
			memset(files,0,nbfiles*sizeof(fileinfo));
			// find files infos
			for (p=0;p<dwa_packets.GetSize();p++)
			{
				pos = dwa_packets.GetAt(p);
				const par2_header& header = get<par2_header>(par2map,pos);
				if (!memcmp(header.type,par2_file_desc::TYPE,16))
				{ // found file desc header
					const par2_file_desc* desc = &get<par2_file_desc>(par2map,pos);
					const hash16* ptr;
					if (fileid_sorted)
						ptr = (const hash16*)bsearch(&(desc->file_id),pmain->files_hash(),nbfiles,sizeof(hash16),&hash_compare);
					else
					{
						unsigned int temp = (unsigned int)nbfiles;
						ptr = (const hash16*)lfind(&(desc->file_id),pmain->files_hash(),&temp,sizeof(hash16),&hash_compare);
					}
					if (ptr!=NULL)
					{
						f = ptr - pmain->files_hash();
						files[f].total.md5hash = desc->file_hash;
						files[f].head16k.md5hash = desc->head_hash;
						files[f].size = desc->file_length;
						strcpy(files[f].name,dir);
						memcpy(files[f].name+dirlen,desc->filename(),min(sizeof(files[f].name)-dirlen-1,(u32)desc->header.length-sizeof(par2_file_desc)));
						files[f].nbparts=((u32)files[f].size+partsize-1)/partsize;
						files[f].flags |= SELECTED;
					}
				}
			}
			// file file slices infos
			/*
			{
				else if (!memcmp(header.type,par2_file_slices::TYPE,16))
				{ // found file slices header
					const par2_file_slices* slices = &get<par2_file_slices>(par2map,pos);
					const hash16* ptr;
					if (fileid_sorted)
						ptr = (const hash16*)bsearch(&(slices->file_id),pmain->files_hash(),nbfiles,sizeof(hash16),&hash_compare);
					else
					{
						unsigned int temp = (unsigned int)nbfiles;
						ptr = (const hash16*)lfind(&(slices->file_id),pmain->files_hash(),&temp,sizeof(hash16),&hash_compare);
					}
					if (ptr!=NULL)
					{
						f = ptr - pmain->files_hash();
						files[f].nbparts = (slices->
						files[f].total.md5hash = desc->file_hash;
						files[f].head16k.md5hash = desc->head_hash;
						files[f].size = desc->file_length;
						memcpy(files[f].name,desc->filename(),min(sizeof(files[f].name)-1,(u32)desc->header.length-sizeof(par2_file_desc)));
					}
				}
			}
			*/
			/*
			for (f=0;f<nbfiles;f++)
			{
				hash16 fileid = pmain->files_hash()[f];
				int i;
				files[f].flags=SELECTED;
				for (i=0;i<nbfiles;i++)
				{
					
				}
			}
			*/

			// construct the common data

			par2common_size = 0;
			for (p=0;p<dwa_packets.GetSize();p++)
			{
				pos = dwa_packets.GetAt(p);
				const par2_header& header = get<par2_header>(par2map,pos);

				par2common_size+=(u32)header.length;
			}

			par2_creator* pcreator=NULL;

			// CREATOR PACKET
			const char* creator = CREATOR;
			int clen = strlen(creator);
			int length = sizeof(par2_creator)+ceil4(clen);
			pcreator=(par2_creator*)malloc(length);
			// header
			pcreator->header.length=length;
			pcreator->header.setid=par2_setid;
			memcpy(pcreator->header.magic,par2_header::MAGIC,8);
			memcpy(pcreator->header.type,par2_creator::TYPE,16);
			// body
			memcpy(pcreator->creator(),creator,clen);
			for (int i=clen;i<(int)ceil4(clen);i++)
				pcreator->creator()[i]='\0';
			MD5Hash packethash;
			packethash.hashInit();
			packethash.hashEnd(((const u8*)pcreator)+32,(u32)pcreator->header.length-32);
			packethash.copyHash16(pcreator->header.hash);

			par2common_size+=(u32)pcreator->header.length;

			//if (par2common_size>0)
			{
				par2common_data = new u8[par2common_size];
				par2common_size = 0;
				for (p=0;p<dwa_packets.GetSize();p++)
				{
					pos = dwa_packets.GetAt(p);
					const par2_header& header = get<par2_header>(par2map,pos);
					memcpy(par2common_data + par2common_size,par2map.ptr(pos),(u32)header.length);
					par2common_size+=(u32)header.length;
				}
				memcpy(par2common_data + par2common_size,pcreator,(u32)pcreator->header.length);
				par2common_size+=(u32)pcreator->header.length;
			}
		}
	}
	if (par2map.last_error[0]!='\0')
		AfxMessageBox(par2map.last_error);
}

/*

PAR2 encoding optimizations
NOTE: NOT IMPLEMENTED YET ;)

we have vol_i[w] = part_0[w]*base_i^0 + part_1[w]*base_i^1 + part_2[w]*base_i^2 + ...

        vol_i[w] = part_0[w] + base_i * (part_1[w] + base_i * (part_2[w] + ...))

        vol_i[w] = part_j[w] ^ alog(log(base_i) + log(vol_i[w]'))   j=N-1...0

        vol_i[w] = vol_i[w]' ^ alog(log(base_i)*j + log(part_j[w]))   j=0...N-1

				alog(log(base_i)*j + log(part_j[w])) = alog((log(base_i)-log(base_i-1))*j + log(base_i)*j + log(part_j[w]))

65535 = 3*5*17*257

        pgcd(log(base_i),65535)=1

				1 <= log(base_i+1) - log(base_i) <= 6
or (if reordered)
				1 <= base_i+1 - base_i <= 16

Update order:

				We consider that all resulting volumes fit in memory.
        We have a subset of parts in memory.

				Let volcache be the max number of volumes bytes that can fit in the CPU cache
				Then:

				   For each volcache chunck of volumes  vol_i[w1..w2]
					    For each part_j[w] in memory and for which w1<=w<=w2
							  update vol_i[w] with part_j[w]

				The goal of this order is:

					 - no swap to disk if possible (only use the data currently in memory)
					 - as few memory write as possible (a chunck is written back to memory
					     only when it requires unavailable data for further updates)
					 - base_i is constant in the inner loop, so custom LUT can be generated

				Lookup table:
					We want to use the following formula:
		        vol_i[w] = part_j[w] ^ (base_i * vol_i[w]')
		        vol_i[w] = part_j[w] ^ (base_i * High(vol_i[w]')) ^ (base_i * Low(vol_i[w]')) (High(x)=x&0xff00 and Low(x)=x&0xff)
						vol_i[w] = part_j[w] ^ lutH_i[vol_i[w]>>8] ^ lutL_i[vol_i[w]&0xff]

					This requires 1KB of data per volume chunck

*/


//class PostPar2Volumes

	// Create new par2 volumes
PostPar2Volumes::PostPar2Volumes(PostFiles* _sources,int _first_vol, int _nb_vol_files, int* _vol_files_size)
{
	sources = _sources;
	start_first_vol = _first_vol;
	nb_vol_files = _nb_vol_files;
	nb_slices=0;
	if (nb_vol_files>0)
	{
		int f;
		vol_files_size = new int[nb_vol_files];
		for (f=0;f<nb_vol_files;f++)
		{
			vol_files_size[f]=_vol_files_size[f];
			nb_slices+=vol_files_size[f];
		}
		vol_files_name = new CString[nb_vol_files];
		int last_vol = start_first_vol+nb_slices-1;
		int ndigit = 1;
		while (last_vol>=10)
		{
			++ndigit;
			last_vol/=10;
		}
		last_vol = start_first_vol;
		for (f=0;f<nb_vol_files;f++)
		{
			// old convention
			//vol_files_name[f].Format("%s.vol%0*d-%0*d.par2",sources->basename,ndigit,last_vol,ndigit,last_vol+vol_files_size[f]-1);
			// 'QuickPar' convention
			vol_files_name[f].Format("%s.vol%0*d+%0*d.par2",sources->basename,ndigit,last_vol,ndigit,vol_files_size[f]);
			last_vol+=vol_files_size[f];
		}
		vol_files_priv = new void*[nb_vol_files];
		memset(vol_files_priv,0,sizeof(void*)*nb_vol_files);
	}
	else
	{
		vol_files_size = NULL;
		vol_files_name = NULL;
		vol_files_priv = NULL;
	}
	vol_files_map = NULL;
	recv_slices = NULL;
	last_file=-1;
	last_offset=0;
}

struct par2_temp_infos
{
	int magic;
	int corrupt;
	int last_fragment;
	int last_offset;
	static const char* MAGIC;
};
const char* par2_temp_infos::MAGIC="PP2K";

// Resume creation of par2 volumes
PostPar2Volumes::PostPar2Volumes(PostFiles* _sources, CStringArray* listVolFiles, void** _vol_files_priv)
{
	CWaitCursor wc;
	sources = _sources;
	start_first_vol = 0;
	nb_vol_files = listVolFiles->GetSize();
	nb_slices=0;
	int packet_length = GetPacketLength(); //sources->partsize+sizeof(par2_recv_slice);
	int ncorrupt = 0;
	int slnum = 0;
	if (nb_vol_files>0)
	{
		int f;
		vol_files_name = new CString[nb_vol_files];
		vol_files_size = new int[nb_vol_files];
		vol_files_map = new FileMemoryMappingWrite[nb_vol_files];
		vol_files_priv = new void*[nb_vol_files];
		int fi = 0;
		for (f=0;f<nb_vol_files;f++)
		{
			vol_files_name[fi] = listVolFiles->GetAt(f);
			vol_files_size[fi] = 0;
			CString fname = CString(vol_files_name[fi])+CString(".temp");
			if (!vol_files_map[fi].open(fname))
			{
				errorMsg.Format("ERROR opening file %s: %s",(LPCTSTR)fname, vol_files_map[fi].last_error);
				// replace the file name with the error message
				listVolFiles->SetAt(f,errorMsg);
			}
			else
			{
				slnum = 0;
				while ((int)(slnum*packet_length)<=(int)(vol_files_map[fi].map_size-sizeof(par2_header)))
				{
				  par2_header& header = get<par2_header>(vol_files_map[fi],slnum*packet_length);
					if (!memcmp(header.magic,par2_header::MAGIC,8)
					 && !memcmp(header.type,par2_recv_slice::TYPE,16)
					 && header.length==packet_length)
						++vol_files_size[fi];
					++slnum;
				}
				nb_slices += vol_files_size[fi];
				if (_vol_files_priv) vol_files_priv[fi] = _vol_files_priv[f];
				++fi;
			}
		}
		nb_vol_files = fi;

		recv_slices = new par2_recv_slice*[nb_slices];
		//int pi;
		slnum = 0;
		int last_fragment = 0;
		for (f=0; f<nb_vol_files;f++)
		{
			CString corruptions;
			for (int pi=0;pi<vol_files_size[f];pi++)
			{
				par2_recv_slice* slice=&get<par2_recv_slice>(vol_files_map[f],pi*packet_length);
				if (!memcmp(slice->header.magic,par2_header::MAGIC,8)
				 && !memcmp(slice->header.type,par2_recv_slice::TYPE,16)
				 && slice->header.length==packet_length)
				{

					recv_slices[slnum]=slice;
					// verify the packet...
					par2_temp_infos* infos = (par2_temp_infos*)&(recv_slices[slnum]->header.hash);

					if (infos->corrupt==1)
					{
						char buf[16];
						sprintf(buf," %d",slice->exponent);
						corruptions+=buf;
						infos->last_fragment=0;
						infos->last_offset=0;
						memset(slice->data(),0,sources->partsize);
						infos->corrupt=0;
					}

					if (slnum==0 || last_fragment>infos->last_fragment)
						last_fragment=infos->last_fragment;
					++slnum;
				}
			}
			if (corruptions.GetLength()>0)
			{
				// add a warning
				CString warn;
				warn.Format("WARNING: packets %s corrupted. Recomputing them.",(LPCTSTR)corruptions);
				listVolFiles->SetAt(f,warn);
			}
			//memcpy(vol_files_map[fi].ptr(vol_files_size[fi]*packet_length),sources->par2common_data,sources->par2common_size);
		}

		last_file=0;
		cur_part=last_fragment;

		while (last_file<sources->nbfiles && last_fragment >= sources->files[last_file].nbparts)
		{
			last_fragment -= sources->files[last_file].nbparts;
			++last_file;
		}

		last_offset = last_fragment * sources->partsize;

		next_offset=last_offset+sources->partsize;
		if (next_offset>sources->files[last_file].size) next_offset = sources->files[last_file].size;
	}
	else
	{
		errorMsg = "ERROR: No File to process!";
		vol_files_size = NULL;
		vol_files_name = NULL;
		vol_files_map = NULL;
		recv_slices = NULL;
		last_file=-1;
		last_offset=0;
		cur_part=-1;
	}
}

int PostPar2Volumes::GetPacketLength()
{
	return sources->partsize+sizeof(par2_recv_slice);
}

bool PostPar2Volumes::Start()
{
	CWaitCursor wc;
	if (nb_vol_files<=0) return false;
	if (vol_files_map!=NULL) return false;

	vol_files_map = new FileMemoryMappingWrite[nb_vol_files];

	recv_slices = new par2_recv_slice*[nb_slices];

	int packet_length = GetPacketLength(); //sources->partsize+sizeof(par2_recv_slice);

	int fi;

	for (fi=0; fi<nb_vol_files;fi++)
	{
		if (!vol_files_map[fi].open(CString(vol_files_name[fi])+CString(".temp"),vol_files_size[fi]*packet_length+sources->par2common_size, true))
		{
			if (vol_files_map[fi].last_error[0]!='\0')
				AfxMessageBox(vol_files_map[fi].last_error);
			else
				AfxMessageBox(vol_files_name[fi]);
			return false;
		}
	}

	int slnum = 0;

	for (fi=0; fi<nb_vol_files;fi++)
	{
		for (int pi=0;pi<vol_files_size[fi];pi++)
		{
			recv_slices[slnum]=&get<par2_recv_slice>(vol_files_map[fi],pi*packet_length);

			memcpy(recv_slices[slnum]->header.magic,par2_header::MAGIC,8);
			recv_slices[slnum]->header.length=packet_length;
			recv_slices[slnum]->header.setid=sources->par2_setid;
			memcpy(recv_slices[slnum]->header.type,par2_recv_slice::TYPE,16);
			recv_slices[slnum]->exponent=slnum+start_first_vol;

			memset(recv_slices[slnum]->data(),0,sources->partsize);

			par2_temp_infos* infos = (par2_temp_infos*)&(recv_slices[slnum]->header.hash);

			infos->magic=*(int*)par2_temp_infos::MAGIC;
			infos->last_fragment=0;
			infos->last_offset=0;
			infos->corrupt=0;

			++slnum;
		}
		memcpy(vol_files_map[fi].ptr(vol_files_size[fi]*packet_length),sources->par2common_data,sources->par2common_size);
	}

	last_file=0;
	last_offset=0;

	cur_part=0;
	next_offset=sources->partsize;
	if (next_offset>sources->files[last_file].size) next_offset = sources->files[last_file].size;

	return true;
}

PostPar2Volumes::~PostPar2Volumes()
{
	if (vol_files_size!=NULL) { delete[] vol_files_size; vol_files_size=NULL; }
	if (vol_files_name!=NULL) { delete[] vol_files_name; vol_files_name=NULL; }
	if (vol_files_map!=NULL) { delete[] vol_files_map; vol_files_map=NULL; }
	if (recv_slices!=NULL) { delete[] recv_slices; recv_slices=NULL; }
}

bool PostPar2Volumes::GetNextFragmentInfo(int* file, u64* offset, u64* size)
{
	if ((unsigned)last_file>=(unsigned)sources->nbfiles)
		return false; // not ready
/*
	if (last_offset>=sources->files[last_file].size)
	{
		if (last_file==sources->nbfiles-1)
			return false; // no more;
		++last_file;
		last_offset=0;
	}
*/
	*file = last_file;
	*offset = last_offset;
	// find nextpart in file
	//u64 nextpart = (last_offset/sources->partsize + 1)*sources->partsize;
	//if (nextpart > sources->files[last_file].size) nextpart = sources->files[last_file].size;
	*size = next_offset - last_offset;
	return true;
}

bool PostPar2Volumes::ProcessFragment(int file, u64 offset, u64 size, const u8* data, bool& bSTOP, int& progress1000, u64 &stat_octets_processed)
{
	u64 size0 = size;
	if (vol_files_map==NULL)
		return false;
	if ((unsigned)last_file>=(unsigned)sources->nbfiles)
		return false; // not ready
	if (file<last_file)
		return true; // already done
	if (file==last_file)
	{
		if (offset<last_offset)
		{
			u64 diff = last_offset-offset;
			offset = last_offset;
			size-=diff;
			data+=diff;
		}
		else if (offset>last_offset)
			return false; // prior data is missing
	}
	else // if file>last_file
		return false;
	if (size<=0)
		return true; // nothing to do

	// PROCESSING!!!

	while (size > 0)
	{
		if (offset==next_offset)
		{ // next part
			++cur_part;
			// we still have some data from the current file so this can't be the end
			if (offset>=sources->files[last_file].size)
				return false; // ERROR!!!
			next_offset+=sources->partsize;
			if (next_offset>sources->files[file].size)
				next_offset=sources->files[file].size;
		}
		u64 psize = next_offset-offset;
		if (psize>size) psize=size;
		// size>0 and next_offset>offset so psize > 0
		ProcessPart(cur_part,data,((u32)offset)%sources->partsize,(u32)psize,(offset+psize==next_offset),bSTOP,stat_octets_processed);
		if (bSTOP) break;
		offset+=psize;
		data+=psize;
		size-=psize;
		progress1000 = (int)(((size0-size)*1000)/size0);
	}

	if (offset==next_offset)
	{ // next part
		++cur_part;
		if (offset>=sources->files[last_file].size)
		{ // next file
			++file;
			offset=0;
			if (file==sources->nbfiles)
				next_offset=0;
			else
			{
				next_offset=sources->partsize;
				if (next_offset>sources->files[file].size)
					next_offset=sources->files[file].size;
			}
		}
	}

	last_file = file;
	last_offset = offset; //+size;

	return true; // end of processing
}

void PostPar2Volumes::Abort()
{
	last_file=-1;
	if (vol_files_map!=NULL) { delete[] vol_files_map; vol_files_map=NULL; }
}

bool PostPar2Volumes::Finish(int& progress1000)
{
	CWaitCursor wc;
	if (last_file != sources->nbfiles || last_offset != 0)
		return false;
	if (recv_slices!=NULL)
	{
		MD5Hash packethash;
		for (int v=0;v<nb_slices;v++)
		{
			progress1000 = (v*1000)/nb_slices;
			packethash.hashInit();
			packethash.hashEnd(((const u8*)recv_slices[v])+32,(u32)recv_slices[v]->header.length-32);
			packethash.copyHash16(recv_slices[v]->header.hash);
		}
		delete[] recv_slices;
		recv_slices=NULL;
	}
	if (vol_files_map!=NULL)
	{
		delete[] vol_files_map;
		vol_files_map=NULL;
	}
	for (int f=0;f<nb_vol_files;f++)
	{
		remove(vol_files_name[f]);
		rename(vol_files_name[f]+CString(".temp"),vol_files_name[f]);
	}
	return true;
}

bool PostPar2Volumes::ProcessFile(int file, bool& bSTOP, int& progress1000,u64 &stat_octets_processed)
{
	FileMemoryMappingRead map;
	if (!map.open(sources->files[file].name))
	{
		if (map.last_error[0]!='\0')
			AfxMessageBox(map.last_error);
		return false;
	}
	return ProcessFragment(file,0,map.map_size,map.map_base, bSTOP, progress1000, stat_octets_processed);
}

void PostPar2Volumes::ProcessPart(int pnum, const u8* data, int part_ofs, int size, bool end_of_part, bool& bSTOP,u64 &stat_octets_processed)
{
	//CWaitCursor wc;
	const u16* dataend = (const u16*) (data+size);
	int i;
	u16 base = Galois16::goodbase[pnum];
	u16 w;
	//u32 l;

	u16 lutLbase[256]; // lutLbase[w1] = w1*base for 0<=w<256
	u16 lutHbase[256]; // lutHbase[w2] = (w2<<8)*base for 0<=w<=256

	int logbase = Galois16::logtable[base];
	lutLbase[0]=0;
	lutHbase[0]=0;
	for (w=1;w<256;w++)
	{
		i = Galois16::logtable[w]+logbase;
		if (i>=Galois16::Limit) i-=Galois16::Limit;
		lutLbase[w]=Galois16::alogtable[i];

		i = Galois16::logtable[w<<8]+logbase;
		if (i>=Galois16::Limit) i-=Galois16::Limit;
		lutHbase[w]=Galois16::alogtable[i];
	}

	u16 lutL[256]; // lutL[w1] = w1*base^exponent  for 0<=w<256
	u16 lutH[256]; // lutH[w2] = (w2<<8)*base^exponent for 0<=w<=256
	lutL[0]=0;
	lutH[0]=0;

	// -> W*base^exponent = ((W&0x00ff)+(W&0xff00))*base^exponent
	//                    = (W&0x00ff)*base^exponent + (W&0xff00)*base^exponent
	//                    = lutL[W&0x00ff]+lutH[W>>8]
	u32 last_exponent = 0xfffffff0;
	for (int vol=0;vol<nb_slices;vol++)
	{
		if (bSTOP) return;
		int local_ofs = ((par2_temp_infos*)&recv_slices[vol]->header.hash)->last_offset;
		// before doing expensive computations make sure we need them
		if (!( pnum == ((par2_temp_infos*)&recv_slices[vol]->header.hash)->last_fragment
				&& part_ofs <= local_ofs
				&& part_ofs+size > local_ofs
			 ))
			continue;
		((par2_temp_infos*)&recv_slices[vol]->header.hash)->corrupt=1;
		if (recv_slices[vol]->exponent == last_exponent + 1)
		{ // incremental update
			++last_exponent;
			// between each volume, exponent increases by 1, so we must update lutL&lutH:
			// lut'[i]=lut[i]*base = lutLbase(lut[i]&0x00ff) ^ lutHbase(lut[i]>>8)
#ifdef ASMENCODE
			__asm {
				xor esi,esi
				mov edi,0xff
				lut_update_loop:
					mov eax,dword ptr lutL[esi]
					mov ebx,eax
					mov ecx,eax
					mov edx,eax
					and eax,edi
					shr ebx,8
					shr ecx,16
					shr edx,24
					and ebx,edi
					and ecx,edi
					movzx eax,word ptr lutLbase[eax*2]
					movzx ebx,word ptr lutHbase[ebx*2]
					movzx ecx,word ptr lutLbase[ecx*2]
					movzx edx,word ptr lutHbase[edx*2]
					xor ecx,edx
					shl ecx,16
					xor eax,ebx
					xor eax,ecx
					mov dword ptr lutL[esi],eax

					mov eax,dword ptr lutH[esi]
					mov ebx,eax
					mov ecx,eax
					mov edx,eax
					and eax,edi
					shr ebx,8
					shr ecx,16
					shr edx,24
					and ebx,edi
					and ecx,edi
					movzx ebx,word ptr lutHbase[ebx*2]
					movzx eax,word ptr lutLbase[eax*2]
					movzx ecx,word ptr lutLbase[ecx*2]
					movzx edx,word ptr lutHbase[edx*2]
					xor ecx,edx
					shl ecx,16
					xor eax,ebx
					xor eax,ecx
					mov dword ptr lutH[esi],eax

					add esi,4
					cmp esi,512
					jnz lut_update_loop
			}
#else
			u16 bak;
			for (w=1;w<256;w++)
			{
				bak = lutL[w];
				lutL[w] = lutLbase[bak&0x00ff] ^lutHbase[(bak>>8)&0x00ff];
				bak = lutH[w];
				lutH[w] = lutLbase[bak&0x00ff] ^lutHbase[(bak>>8)&0x00ff];
			}
#endif
		}
		else
		{
			last_exponent = recv_slices[vol]->exponent;
			if (last_exponent == 1)
			{
				memcpy(lutL,lutLbase,sizeof(lutL));
				memcpy(lutH,lutHbase,sizeof(lutH));
			}
			else
			{
				if (last_exponent == 0)
				{
					for (w=1;w<256;w++)
					{
						lutL[w]=w;
						lutH[w]=w<<8;
					}
					//memset(lutL,0,sizeof(lutL));
					//memset(lutH,0,sizeof(lutH));
				}
				else
				{
					int logbaseexp = (logbase*last_exponent)%Galois16::Limit;
					for (w=1;w<256;w++)
					{
						i = Galois16::logtable[w]+logbaseexp;
						if (i>=Galois16::Limit) i-=Galois16::Limit;
						lutL[w]=Galois16::alogtable[i];

						i = Galois16::logtable[w<<8]+logbaseexp;
						if (i>=Galois16::Limit) i-=Galois16::Limit;
						lutH[w]=Galois16::alogtable[i];
					}
				}
			}
		}
		{
			u16* dest = (u16*)(recv_slices[vol]->data()+local_ofs);
			const u16* src = (const u16*)(data+(local_ofs-part_ofs));

			stat_octets_processed+=((const u8*)dataend)-((const u8*)src);

			if (local_ofs&3)
			{ // unaligned
				if (local_ofs&1)
				{ // one single (high) byte
					*(u16*)(((u8*)dest)-1) ^= lutH[*(const u8*)src];
					dest=(u16*)(((u8*)dest)+1);
					src=(const u16*)(((const u8*)src)+1);
				}
				if (local_ofs&2)
				{ // one word
					w = *src;
					*dest ^= lutL[w&0x00ff] ^ lutH[(w>>8)&0x00ff];
					++src;
					++dest;
				}
			}

			dataend=(const u16*) (((const u8*)dataend)-3);

#ifdef ASMENCODE
			__asm {
				mov esi,src
				mov edi,dest
				cmp esi,dataend
				jge encodeend
				encodeloop:
					mov eax,[esi]
					mov ecx,eax
					mov edx,eax
					mov ebx,eax
					shr ecx,16
					shr edx,24
					shr ebx,8
					and ecx,0xff
					and eax,0xff
					and ebx,0xff
					//and edx,0xff
					movzx ecx,word ptr lutL[ecx*2]
					movzx edx,word ptr lutH[edx*2]
					movzx eax,word ptr lutL[eax*2]
					movzx ebx,word ptr lutH[ebx*2]
					xor ecx,edx
					shl ecx,16
					xor eax,ebx
					xor eax,ecx
					xor [edi],eax
					add esi,4
					add edi,4
					cmp esi,dataend
					jb encodeloop
				encodeend:
				mov src,esi
				mov dest,edi

			}
#else
			while (src<dataend)
			{
				w = *src;
				*dest ^= lutL[w&0x00ff] ^ lutH[(w>>8)&0x00ff];
				++src;
				++dest;
			}
#endif

			dataend=(const u16*) (((const u8*)dataend)+3);

			if (src<=dataend-1)
			{ // one word
				w = *src;
				*dest ^= lutL[w&0x00ff] ^ lutH[(w>>8)&0x00ff];
				++src;
				++dest;
			}
			if (src<dataend)
			{ // one single (low) byte
				*dest ^= lutL[*(const u8*)src];
				//dest=(u16*)(((u8*)dest)+1);
				//src=(const u16*)(((const u8*)src)+1);
			}

		}
		if (end_of_part)
		{
			((par2_temp_infos*)&recv_slices[vol]->header.hash)->last_fragment = pnum+1;
			((par2_temp_infos*)&recv_slices[vol]->header.hash)->last_offset = 0;
		}
		else
		{
			((par2_temp_infos*)&recv_slices[vol]->header.hash)->last_fragment = pnum;
			((par2_temp_infos*)&recv_slices[vol]->header.hash)->last_offset = part_ofs+size;
		}
		((par2_temp_infos*)&recv_slices[vol]->header.hash)->corrupt=0;
	}

}

} // namespace par2
