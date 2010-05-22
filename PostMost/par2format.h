#ifndef PAR2FORMAT_H
#define PAR2FORMAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

// CRC32 calculations

namespace par2 {

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned __int64 u64;

#define CRC_USE_TABLE

class CRC32
{
public:
	enum { INIT=0xffffffff };
	enum { XOR_OUT=0xffffffff };
	enum { POLY=0x04C11DB7 };
	// poly:      0000-0100-1100-0001-0001-1101-1011-0111
	// reflected: 1110-1101-1011-1000-1000-0011-0010-0000
	enum { REF_POLY=0xEDB88320 };

	static u32 table[256];

	u32 crc_begin()
	{
	  return INIT;
	}

	u32 crc_add(u32 crc, u8 data)
	{
		// for non reflected case
		//return table[((crc>>24) ^ data) & 0xFFL] ^ (crc << 8);

		// for reflected case
		return table[(crc ^ data) & 0xFFL] ^ (crc >> 8);
	}

	u32 crc_add(u32 crc, const u8* data, u32 datalength)
	{
#ifdef ASMENCODE
	  while (((int)data)&3 && datalength)
		{
			crc = table[(crc ^ *data++) & 0xFFL] ^ (crc >> 8);
			--datalength;
		}
		u32* t = table;
		__asm {
		  mov ebx,t
			mov ecx,crc
		  mov edi,datalength
			mov esi,data
			sub edi,3        // do not loop if less than 3
			add esi,edi      // esi = data+length
			neg edi          // edi = -length
			jge crc_add_end
			crc_add_loop:
				//crc = table[(crc ^ *data++) & 0xFFL] ^ (crc >> 8);
				mov edx,[esi+edi]
				// 1
				mov eax,ecx
				shr ecx,8
				xor eax,edx
				shr edx,8
				and eax,0xff
				xor ecx,[ebx+eax*4]
				// 2
				mov eax,ecx
				shr ecx,8
				xor eax,edx
				shr edx,8
				and eax,0xff
				xor ecx,[ebx+eax*4]
				// 3
				mov eax,ecx
				shr ecx,8
				xor eax,edx
				shr edx,8
				and eax,0xff
				xor ecx,[ebx+eax*4]
				// 4
				mov eax,ecx
				shr ecx,8
				xor eax,edx
				//shr edx,8
				and eax,0xff
				xor ecx,[ebx+eax*4]

				add edi,4
				jb crc_add_loop
			crc_add_end:
			mov crc,ecx
			add esi,edi
			neg edi
			add edi,3
			mov data,esi
			mov datalength,edi
		}
#endif
	  while (datalength--)
		{
			// for non reflected case
			//crc = table[((crc>>24) ^ *data++) & 0xFFL] ^ (crc << 8);

			// for reflected case
			crc = table[(crc ^ *data++) & 0xFFL] ^ (crc >> 8);
		}
		return crc;
	}

	u32 crc_add_null(u32 crc, u32 length)
	{
	  while (length--)
		{
			// for non reflected case
			//crc = table[((crc>>24)) & 0xFFL] ^ (crc << 8);

			// for reflected case
			crc = table[(crc) & 0xFFL] ^ (crc >> 8);
		}
		return crc;
	}

	u32 crc_end(u32 crc)
	{
		return crc ^ XOR_OUT;
	}

	u32 calc_crc(const u8* data, u32 length)
	{
		return crc_end(crc_add(crc_begin(),data,length));
	}

	void generate_table()
	{
		for (int i=0;i<256;i++)
		{
			// for non reflected case
			//u32 r = i<<24;

			// for reflected case
			u32 r = i;

			for (int b=0;b<8;b++)
			{
				// for non reflected case
				//if (r&0x80000000L)
				//	r=(r<<1)^POLY;
				//else
				//	r<<=1;

				// for reflected case
				if (r&1L)
					r=(r>>1)^REF_POLY;
				else
					r>>=1;
			}
			table[i]=r;
		}
	}

	void print_table(FILE* f)
	{
		fprintf(f,"// CRC32 generated table (POLY=0x%08X)\n",POLY);
		u32 check=calc_crc((const u8*) "123456789",9);
		fprintf(f,"// Check of \"123456789\" using INIT=0x%08X and XOR_OUT=0x%08X: 0x%08X\n",INIT,XOR_OUT,check);
		fputc('{',f);
		for (int i=0;i<256;i++)
		{
			fprintf(f,"0x%08X,",table[i]);
			if ((i&7)==7) fputc('\n',f);
		}
		fprintf(f,"}\n");
	}

	CRC32()
	{
#ifndef CRC_USE_TABLE
		generate_table();
#ifdef _DEBUG
		FILE* f = fopen("crc32table.h","w");
		print_table(f);
		fclose(f);
#endif
#endif
	}

};

// MD5 Hash

struct hash16
{
	u8 b[16];
};

class MD5Hash
{
public:

	enum { INIT_A=0x67452301 };
	enum { INIT_B=0xEFCDAB89 };
	enum { INIT_C=0x98BADCFE };
	enum { INIT_D=0x10325476 };

/*
	enum { S11=7  };
	enum { S12=12 };
	enum { S13=17 };
	enum { S14=22 };
	enum { S21=5  };
	enum { S22=9  };
	enum { S23=14 };
	enum { S24=20 };
	enum { S31=4  };
	enum { S32=11 };
	enum { S33=16 };
	enum { S34=23 };
	enum { S41=6  };
	enum { S42=10 };
	enum { S43=15 };
	enum { S44=21 };
*/

	u32 state[4];
	u64 length;

	MD5Hash()
	{
		hashInit();
	}

	void hashInit()
	{
		state[0]=INIT_A;
		state[1]=INIT_B;
		state[2]=INIT_C;
		state[3]=INIT_D;
		length=0;
	}

	static bool test(const char* refmsg, const char* refhash)
	{
		MD5Hash hash;
		char result[33];
		hash.hashInit();
		hash.hashEnd((const u8*)refmsg,strlen(refmsg));
		//hash.copyHash16(result);
		hash.toString33(result);
		if (strcmp(result,refhash)==0)
			return true;
		else
			return false;
	}

	static bool test()
	{
		return test("","d41d8cd98f00b204e9800998ecf8427e")
		    & test("a","0cc175b9c0f1b6a831c399e269772661")
		    & test("abc","900150983cd24fb0d6963f7d28e17f72")
		    & test("message digest","f96b697d7cb7938d525a2f31aaf161d0")
				;
	}

/* These are the four functions used in the four steps of the MD5 algorithm
   and defined in the RFC 1321.  The first function is a little bit optimized
   (as found in Colin Plumbs public domain implementation).  */
//	u32 F(u32 X, u32 Y, u32 Z) { return (X&Y)|((~X)&Z); }
//	u32 G(u32 X, u32 Y, u32 Z) { return (Z&X)|((~Z)&Y); }
	u32 F(u32 X, u32 Y, u32 Z) { return Z^(X&(Y^Z)); }
	u32 G(u32 X, u32 Y, u32 Z) { return Y^(Z&(X^Y)); }
	u32 H(u32 X, u32 Y, u32 Z) { return X^Y^Z;       }
	u32 I(u32 X, u32 Y, u32 Z) { return Y^(X|(~Z));  }
	u32 rotleft(u32 X, u32 s) { return (X<<s)|(X>>(32-s)); }

	void hashBlock64(const u8* bdata) // 64 bytes block
	{
		const u32* data = (const u32*) bdata;
		u32 a=state[0],b=state[1],c=state[2],d=state[3];

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#ifdef ASMENCODE

#define asmF(X,Y,Z) \
  __asm {           \
	  __asm mov edi,Z \
	  __asm xor edi,Y \
	  __asm and edi,X \
	  __asm xor edi,Z \
	}

#define asmG(X,Y,Z) \
  __asm {           \
	  __asm mov edi,Y \
	  __asm xor edi,X \
	  __asm and edi,Z \
	  __asm xor edi,Y \
	}

#define asmH(X,Y,Z) \
  __asm {           \
	  __asm mov edi,Z \
	  __asm xor edi,Y \
	  __asm xor edi,X \
	}

#define asmI(X,Y,Z) \
  __asm {           \
	  __asm mov edi,Z \
	  __asm not edi   \
	  __asm or  edi,X \
	  __asm xor edi,Y \
	}

#define asmSTEP(R,X,datapos,shift,addition) \
  __asm {                                   \
	  __asm lea R,[R+edi+addition]            \
		__asm add R,[esi+datapos*4]             \
		__asm rol R,shift                       \
		__asm add R,X                           \
	}

#define MD5_STEP_F(R,X,Y,Z,datapos,shift,add) asmF(X,Y,Z); asmSTEP(R,X,datapos,shift,add);

#define MD5_STEP_G(R,X,Y,Z,datapos,shift,add) asmG(X,Y,Z); asmSTEP(R,X,datapos,shift,add);

#define MD5_STEP_H(R,X,Y,Z,datapos,shift,add) asmH(X,Y,Z); asmSTEP(R,X,datapos,shift,add);

#define MD5_STEP_I(R,X,Y,Z,datapos,shift,add) asmI(X,Y,Z); asmSTEP(R,X,datapos,shift,add);

	// INIT
	__asm {
		mov eax,a
		mov ebx,b
		mov ecx,c
		mov edx,d
		mov esi,data
	}

#define a eax
#define b ebx
#define c ecx
#define d edx

#else

#define MD5_STEP(F,R,X,Y,Z,datapos,shift,add) ((R)=rotleft((R)+F((X),(Y),(Z))+data[datapos]+(add),(shift))+(X))

#define MD5_STEP_F(R,X,Y,Z,datapos,shift,add) MD5_STEP(F,R,X,Y,Z,datapos,shift,add)
#define MD5_STEP_G(R,X,Y,Z,datapos,shift,add) MD5_STEP(G,R,X,Y,Z,datapos,shift,add)
#define MD5_STEP_H(R,X,Y,Z,datapos,shift,add) MD5_STEP(H,R,X,Y,Z,datapos,shift,add)
#define MD5_STEP_I(R,X,Y,Z,datapos,shift,add) MD5_STEP(I,R,X,Y,Z,datapos,shift,add)

#endif

	  /* Round 1 */
		MD5_STEP_F(a, b, c, d,  0, S11, 0xd76aa478);
		MD5_STEP_F(d, a, b, c,  1, S12, 0xe8c7b756);
		MD5_STEP_F(c, d, a, b,  2, S13, 0x242070db);
		MD5_STEP_F(b, c, d, a,  3, S14, 0xc1bdceee);
		MD5_STEP_F(a, b, c, d,  4, S11, 0xf57c0faf);
		MD5_STEP_F(d, a, b, c,  5, S12, 0x4787c62a);
		MD5_STEP_F(c, d, a, b,  6, S13, 0xa8304613);
		MD5_STEP_F(b, c, d, a,  7, S14, 0xfd469501);
		MD5_STEP_F(a, b, c, d,  8, S11, 0x698098d8);
		MD5_STEP_F(d, a, b, c,  9, S12, 0x8b44f7af);
		MD5_STEP_F(c, d, a, b, 10, S13, 0xffff5bb1);
		MD5_STEP_F(b, c, d, a, 11, S14, 0x895cd7be);
		MD5_STEP_F(a, b, c, d, 12, S11, 0x6b901122);
		MD5_STEP_F(d, a, b, c, 13, S12, 0xfd987193);
		MD5_STEP_F(c, d, a, b, 14, S13, 0xa679438e);
		MD5_STEP_F(b, c, d, a, 15, S14, 0x49b40821);

	 /* Round 2 */
		MD5_STEP_G(a, b, c, d,  1, S21, 0xf61e2562);
		MD5_STEP_G(d, a, b, c,  6, S22, 0xc040b340);
		MD5_STEP_G(c, d, a, b, 11, S23, 0x265e5a51);
		MD5_STEP_G(b, c, d, a,  0, S24, 0xe9b6c7aa);
		MD5_STEP_G(a, b, c, d,  5, S21, 0xd62f105d);
		MD5_STEP_G(d, a, b, c, 10, S22, 0x02441453);
		MD5_STEP_G(c, d, a, b, 15, S23, 0xd8a1e681);
		MD5_STEP_G(b, c, d, a,  4, S24, 0xe7d3fbc8);
		MD5_STEP_G(a, b, c, d,  9, S21, 0x21e1cde6);
		MD5_STEP_G(d, a, b, c, 14, S22, 0xc33707d6);
		MD5_STEP_G(c, d, a, b,  3, S23, 0xf4d50d87);
		MD5_STEP_G(b, c, d, a,  8, S24, 0x455a14ed);
		MD5_STEP_G(a, b, c, d, 13, S21, 0xa9e3e905);
		MD5_STEP_G(d, a, b, c,  2, S22, 0xfcefa3f8);
		MD5_STEP_G(c, d, a, b,  7, S23, 0x676f02d9);
		MD5_STEP_G(b, c, d, a, 12, S24, 0x8d2a4c8a);

		/* Round 3 */
		MD5_STEP_H(a, b, c, d,  5, S31, 0xfffa3942);
		MD5_STEP_H(d, a, b, c,  8, S32, 0x8771f681);
		MD5_STEP_H(c, d, a, b, 11, S33, 0x6d9d6122);
		MD5_STEP_H(b, c, d, a, 14, S34, 0xfde5380c);
		MD5_STEP_H(a, b, c, d,  1, S31, 0xa4beea44);
		MD5_STEP_H(d, a, b, c,  4, S32, 0x4bdecfa9);
		MD5_STEP_H(c, d, a, b,  7, S33, 0xf6bb4b60);
		MD5_STEP_H(b, c, d, a, 10, S34, 0xbebfbc70);
		MD5_STEP_H(a, b, c, d, 13, S31, 0x289b7ec6);
		MD5_STEP_H(d, a, b, c,  0, S32, 0xeaa127fa);
		MD5_STEP_H(c, d, a, b,  3, S33, 0xd4ef3085);
		MD5_STEP_H(b, c, d, a,  6, S34, 0x04881d05);
		MD5_STEP_H(a, b, c, d,  9, S31, 0xd9d4d039);
		MD5_STEP_H(d, a, b, c, 12, S32, 0xe6db99e5);
		MD5_STEP_H(c, d, a, b, 15, S33, 0x1fa27cf8);
		MD5_STEP_H(b, c, d, a,  2, S34, 0xc4ac5665);

		/* Round 4 */
		MD5_STEP_I(a, b, c, d,  0, S41, 0xf4292244);
		MD5_STEP_I(d, a, b, c,  7, S42, 0x432aff97);
		MD5_STEP_I(c, d, a, b, 14, S43, 0xab9423a7);
		MD5_STEP_I(b, c, d, a,  5, S44, 0xfc93a039);
		MD5_STEP_I(a, b, c, d, 12, S41, 0x655b59c3);
		MD5_STEP_I(d, a, b, c,  3, S42, 0x8f0ccc92);
		MD5_STEP_I(c, d, a, b, 10, S43, 0xffeff47d);
		MD5_STEP_I(b, c, d, a,  1, S44, 0x85845dd1);
		MD5_STEP_I(a, b, c, d,  8, S41, 0x6fa87e4f);
		MD5_STEP_I(d, a, b, c, 15, S42, 0xfe2ce6e0);
		MD5_STEP_I(c, d, a, b,  6, S43, 0xa3014314);
		MD5_STEP_I(b, c, d, a, 13, S44, 0x4e0811a1);
		MD5_STEP_I(a, b, c, d,  4, S41, 0xf7537e82);
		MD5_STEP_I(d, a, b, c, 11, S42, 0xbd3af235);
		MD5_STEP_I(c, d, a, b,  2, S43, 0x2ad7d2bb);
		MD5_STEP_I(b, c, d, a,  9, S44, 0xeb86d391);

#ifdef ASMENCODE

#undef a
#undef b
#undef c
#undef d

		__asm {
			mov a,eax;
			mov b,ebx;
			mov c,ecx;
			mov d,edx;
		}

#undef MD5_STEP_F
#undef MD5_STEP_G
#undef MD5_STEP_H
#undef MD5_STEP_I
#undef asmSTEP
#undef asmF
#undef asmG
#undef asmH
#undef asmI

#else

#undef MD5_STEP_F
#undef MD5_STEP_G
#undef MD5_STEP_H
#undef MD5_STEP_I
#undef MD5_STEP

#endif

#undef S11
#undef S12
#undef S13
#undef S14
#undef S21
#undef S22
#undef S23
#undef S24
#undef S31
#undef S32
#undef S33
#undef S34
#undef S41
#undef S42
#undef S43
#undef S44

		state[0]+=a;
		state[1]+=b;
		state[2]+=c;
		state[3]+=d;
		length+=64*8; // length is in BITS not bytes
	}

	void hashEnd(const u8* data, int len)
	{
		u8 buffer[64];
		u64 totallength = length+len*8; // take into consideration the previous data blocks
		// totallength is in BITS not bytes

		while (len>=64)
		{
			hashBlock64(data);
			data+=64;
			len-=64;
		}
		memcpy(buffer, data, len);
		if (len>=64-8) // not enough room for the size
		{
			// first pad up to 64
			buffer[len]=0x80;
			memset(buffer+len+1,0,64-(len+1));
			hashBlock64(buffer);
			// then pad everything with 0
			memset(buffer,0,len+1);
		}
		else //if (len<64-8) // need padding
		{
			buffer[len]=0x80;
			memset(buffer+len+1,0,64-8-(len+1));
		}
		// add length
		*(u32*)&(buffer[64-8])=(u32)totallength;
		*(u32*)&(buffer[64-4])=(u32)(totallength>>32);
		// hash last block
		hashBlock64(buffer);
		// set length to correct value
		length=totallength;
	}

	void copyHash16(u8* dest) const
	{
		memcpy(dest,state,16);
	}

	void copyHash16(hash16& dest) const
	{
		copyHash16(dest.b);
	}

	void toString33(char* dest) const
	{
		static const char digits[17]="0123456789abcdef";
		for (int i=0;i<16;i++)
		{
			u8 c = ((const u8*)state)[i];
			dest[2*i+0]=digits[(c>>4)&15];
			dest[2*i+1]=digits[(c   )&15];
		}
		dest[32]='\0';
	}

};

struct partinfo
{
	hash16 md5hash;
	u32 crc32;
};

int hash_compare( const void *arg1, const void *arg2 );

class PostFiles
{
public:

	char basename[_MAX_PATH];
	char sfvname[_MAX_PATH];
	char parname[_MAX_PATH];
	char par2name[_MAX_PATH];

	enum { SELECTED=1 };

	struct fileinfo
	{
		int flags;
		char name[_MAX_PATH];
		time_t modiftime;

		u64  size;
		//u32  crc32;
		//char md5hash[16];
		//char md5hash16K[16];

		partinfo head16k;
		partinfo total;

		int nbparts;
		partinfo *parts;

	} *files;

	int nbfiles;
	int partsize;
	int nbrecvpart;

	hash16 par2_setid;
	int par2common_size;
	u8* par2common_data;

	// create a new set
	PostFiles(const char* _basename, int _nbfiles, int _partsize=0);
	// create from an existing par2file
	PostFiles(const char* par2file);
	// get the setid from a par2file
	static bool GetSetID(const char* par2file, hash16& setid);

	void initFile(int num, const char* fname, bool selected);

	enum { CALC_CRC = 1 };
	enum { CALC_HASH = 2 };
	enum { CALC_PART = 4 };

	void calcFile(int num,int flags=-1);
	void calcbasename();
	const char* generateSFV(const char* fname);
	const char* generatePAR(const char* fname);
	const char* generatePAR2(const char* fname);
	~PostFiles();
/*
	bool generatePAR2Volumes(int first_vol, int nb_vol_files, int* vol_files_size)
	{
		return false;
	}
*/
};

class FileMemoryMappingWrite;
struct par2_recv_slice;

class PostPar2Volumes
{
public:
	PostFiles * sources;
	int start_first_vol;
	int nb_vol_files;
	int *vol_files_size;
	CString* vol_files_name;
	void**   vol_files_priv;
	int nb_slices;

	CString errorMsg;

	// Create new par2 volumes
	PostPar2Volumes(PostFiles* _sources,int _first_vol, int _nb_vol_files, int* _vol_files_size);
	// Resume creation of par2 volumes
	PostPar2Volumes(PostFiles* _sources, CStringArray* listVolFiles, void** _vol_files_priv);
	~PostPar2Volumes();

	bool isok() { return errorMsg.GetLength()==0; }

	bool Start();

	int GetPacketLength();

	bool GetNextFragmentInfo(int* file, u64* offset, u64* size);
	bool ProcessFragment(int file, u64 offset, u64 size, const u8* data, bool& bSTOP, int& progress1000, u64 &stat_octets_processed);

	bool ProcessFile(int file, bool& bSTOP, int& progress1000,u64 &stat_octets_processed);

	void Abort();

	bool isReady() { return last_file==sources->nbfiles; } // ready to finish

	bool Finish(int& progress1000);

protected:
	
	FileMemoryMappingWrite* vol_files_map;
	par2_recv_slice** recv_slices;

	int last_file;
	u64 last_offset;
	int cur_part;
	u64 next_offset;

	void ProcessPart(int pnum, const u8* data, int part_ofs, int size, bool end_of_part, bool& bSTOP,u64 &stat_octets_processed);
};

class Galois16
{
public:
	enum { Limit = 0xFFFF };
	enum { Count = 0x10000 };
	enum { Generator = 0x1100B }; // binary: 10001000000001011

	static u16 logtable[Count];
	static u16 alogtable[Count];

	static u16 goodbase[Count];
	static u32 ngoodbase;

	static bool init()
	{ // generate tables
		{
			// first log/alog tables
			u32 b = 1;
			for (u32 l=0;l<Limit; l++)
			{
				logtable[b] = (u16)l;
				alogtable[l] = (u16)b;
				b <<= 1;
				if (b & Count) b ^= Generator;
			}
			logtable[0] = (u16)Limit;
			alogtable[Limit] = (u16)0;
		}
		// then goodbase
		{
			bool logisbad[Count];
			memset(logisbad,0,sizeof(logisbad));
			u32 l;
			// remove all multiples of 3,5,17 and 257 (prime factors of Limit)
			logisbad[0]=true;
			for (l=3;l<Count;l+=3) logisbad[l]=true;
			for (l=5;l<Count;l+=5) logisbad[l]=true;
			for (l=17;l<Count;l+=17) logisbad[l]=true;
			for (l=257;l<Count;l+=257) logisbad[l]=true;
			// then find all number for which log is not bad...
			ngoodbase=0;
			/*
			int in_bad = 0;
			int in_good = 0;
			int max_bad = 0;
			int max_good = 0;
			for (l=1;l<Count;l++)
			{
				if (!logisbad[logtable[l]])
				{
					goodbase[ngoodbase++]=(u16)l;
					in_bad=0; ++in_good;
					if (in_good>max_good)
						max_good=in_good;
				}
				else
				{
					in_good=0; ++in_bad;
					if (in_bad>max_bad)
						max_bad=in_bad;
				}
			}
			CString log;
			log.Format("Galois16: Found %d good bases, max %d adjacent bases and max %d skip length\n",ngoodbase,max_good,max_bad);
			*/
			for (l=0;l<Count-1;l++)
			{
				if (!logisbad[l])
				{
					goodbase[ngoodbase++]=(u16)alogtable[l];
				}
			}
			CString log;
			log.Format("Galois16: Found %d good bases\n",ngoodbase);
			OutputDebugString(log);
/*
			in_bad = 0;
			in_good = 0;
			max_bad = 0;
			max_good = 0;
			for (l=0;l<Count;l++)
				if (!logisbad[l])
				{
					in_bad=0; ++in_good;
					if (in_good>max_good)
						max_good=in_good;
				}
				else
				{
					in_good=0; ++in_bad;
					if (in_bad>max_bad)
						max_bad=in_bad;
				}
			log.Format("Galois16: max %d adjacent log(bases) and max %d skip length in log(bases)\n",max_good,max_bad);
*/
			OutputDebugString(log);
		}
		return true;
	}

	static u16 add(u16 a, u16 b)
	{
		return a^b;
	}

	static u16 mult(u16 a, u16 b)
	{
		if (!(a|b)) return 0;
		int i = logtable[a]+logtable[b];
		if (i>=Limit) return i-=Limit;
		return alogtable[i];
	}

	static u16 div(u16 a, u16 b)
	{
		if (!(a|b)) return 0;
		int i = logtable[a]-logtable[b];
		if (i<0) return i+=Limit;
		return alogtable[i];
	}
	static u16 power(u16 a, int p)
	{
		if (!a) return 0;
		//if (!p) return 1;
		u32 i = logtable[a]*p;

		// we have (alog(i+Limit)=alog(i))
		//
		// -> alog(i)=alog(i%Limit)
		//
		//                b
		// i = sum (A  * 2 ) for b=0..31
		//           b
		//
		//                               b                                b
		// i = I1+I2 where I1 = sum (A *2 ) for b=0..15 and I2 = sum (A *2 ) for b=16..31
		//                            b                                b
		//
		//                 b-16                       b-16
		// I2 = sum (A  * 2     * Count) = sum (A  * 2     * (Limit+1))
		//            b                          b
		//                                           b-16
		// I2 = IH * (Limit-1)  with IH = sum (A  * 2    ) <= Limit
		//                                      b
		// i % Limit = (I1 + I2) % Limit = (I1 + IH*Limit + IH) % Limit
		// i % Limit = (I1 + IH) % Limit

		i = (i&Limit)+(i>>16);
		if (i>=Limit) i-=Limit;
		return alogtable[i];
	}

};

} // namespace par2

#endif
