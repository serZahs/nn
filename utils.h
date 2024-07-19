typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

void
Panic(char *FormatString, ...) {
    va_list Args;
    va_start(Args, FormatString);
    fprintf(stderr, "Panic: ");
    vfprintf(stderr, FormatString, Args);
    va_end(Args);
    
    exit(1);
}

enum allocator_mode
{
    DEFAULT,
    TEMPORARY_ARENA,
};
enum allocator_mode GlobalAllocatorMode = DEFAULT;

typedef struct
{
    size_t Size;
    size_t Used;
    u8 *Data;
} arena;

void
ArenaInit(arena *Arena)
{
    size_t DefaultArenaSize = 4*1024*1024;
    Arena->Size = DefaultArenaSize;
    Arena->Used = 0;
    Arena->Data = (u8*)malloc(DefaultArenaSize);
}

void
ArenaDeinit(arena *Arena)
{
    Arena->Size = 0;
    Arena->Used = 0;
    free(Arena->Data);
}

void*
ArenaAllocate(arena *Arena, size_t Size)
{
    if ((Arena->Used + Size) > Arena->Size)
        Panic("Arena allocation of size %zd failed\n", Size);
    u8 *Result = Arena->Data + Arena->Used;
    Arena->Used += Size;
    return Result;
}

void
ArenaClear(arena *Arena)
{
    Arena->Used = 0;
}

arena TemporaryArena;

void
InitTemporaryArena()
{
    ArenaInit(&TemporaryArena);
}

void
DeinitTemporaryArena()
{
    ArenaDeinit(&TemporaryArena);
}

void
SetAllocator(enum allocator_mode Mode)
{
    GlobalAllocatorMode = Mode;
}

void
ClearTemporaryArena()
{
    ArenaClear(&TemporaryArena);
}

void*
_Allocate(size_t Size)
{
    if (GlobalAllocatorMode == DEFAULT)
        return malloc(Size);
    else if (GlobalAllocatorMode == TEMPORARY_ARENA)
        return ArenaAllocate(&TemporaryArena, Size);
    else
        return NULL;
}

#define Alloc(x) _Allocate(x)

u32
ByteSwapU32(u32 Value)
{
    u32 Result = ((Value << 8) & 0xFF00FF00) | ((Value >> 8) & 0xFF00FF); 
    Result = (Result << 16) | ((Result >> 16) & 0xFFFF);
    return Result;
}

f32*
RandomFloatArray(s32 Count, f32 V)
{
    f32 Min = -1.0f / sqrtf(V);
    f32 Max = 1.0f / sqrtf(V);
    f32 *Result = (f32*)Alloc(Count * sizeof(f32));
    for (s32 Index = 0; Index < Count; Index++)
    {
        f32 Scale = rand() / (f32)RAND_MAX;
        Result[Index] = Min + Scale * (Max - Min);
    }
    return Result;
}

int
ReadMNISTImageFile(char *Filename, u32 *ImageCount, u32 *RowCount, u32 *ColumnCount, f32 **ImageData)
{
    FILE* File = fopen(Filename, "rb");
	if (!File)
	{
		printf("Error: Cannot open file %s\n", Filename);
		return(0);
	}
    u32 ReadMagicNumber, ReadImageCount, ReadRowCount, ReadColumnCount;
	fread(&ReadMagicNumber, sizeof(u32), 1, File); // Unused!
    fread(&ReadImageCount, sizeof(u32), 1, File);
    ReadImageCount = ByteSwapU32(ReadImageCount);
    fread(&ReadRowCount, sizeof(u32), 1, File);
    ReadRowCount = ByteSwapU32(ReadRowCount);
    fread(&ReadColumnCount, sizeof(u32), 1, File);
    ReadColumnCount = ByteSwapU32(ReadColumnCount);
    
    *ImageCount = ReadImageCount;
    *RowCount = ReadRowCount;
    *ColumnCount = ReadColumnCount;
    
    size_t ImageDataSize = ReadImageCount * ReadRowCount * ReadColumnCount;
    *ImageData = (f32*)Alloc(ImageDataSize * sizeof(f32));
    // Convert from u8 (0 to 255) to float (0.0 to 1.0)
    for (size_t Index = 0; Index < ImageDataSize; Index++)
    {
        u8 Byte;
        fread(&Byte, sizeof(u8), 1, File);
        (*ImageData)[Index] = (f32)Byte / 255.0f;
    }
    
    fclose(File);
    return(1);
}

int
ReadMNISTLabelFile(char *Filename, u32 *LabelCount, u8 **LabelData)
{
    FILE* File = fopen(Filename, "rb");
	if (!File)
	{
		printf("Error: Cannot open file %s\n", Filename);
		return(0);
	}
    u32 ReadMagicNumber, ReadLabelCount;
	fread(&ReadMagicNumber, sizeof(u32), 1, File); // Unused
    fread(&ReadLabelCount, sizeof(u32), 1, File);
    ReadLabelCount = ByteSwapU32(ReadLabelCount);
    *LabelCount = ReadLabelCount;
    *LabelData = (u8*)Alloc(ReadLabelCount);
    fread(*LabelData, ReadLabelCount, 1, File); 
    fclose(File);
    return(1);
}

