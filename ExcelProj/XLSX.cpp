#include "XLSX.h"

#define CHECK_ERR(err, msg) { \
    if (err != Z_OK) { \
        fprintf(stderr, "%s error: %d\n", msg, err); \
        exit(1); \
    } \
}

static void test_inflate(Byte* compr, uLong comprLen, Byte* uncompr, uLong uncomprLen)
{
	int err;
	z_stream stream; /* decompression stream */
	/* allocate inflate state */
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = comprLen; // size of input
	stream.next_in = compr; // input char array
	stream.avail_out = uncomprLen; // size of output
	stream.next_out = uncompr; // output char array

	err = inflateInit(&stream);
	inflate(&stream, Z_NO_FLUSH);
	inflateEnd(&stream);
}

bool XLSX::Load(const char* fn, bool dumpFiles)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		fread(&header, sizeof(ZIP_HEADER), 1, in);
		header.filename = (char*)malloc(header.zipHeader.fileNameLength + 1);
		fread(&header.filename[0], header.zipHeader.fileNameLength, 1, in);
		//add null terminator to our string to remove garbage
		header.filename[header.zipHeader.fileNameLength] = 0;
		//skip over the empty data
		fseek(in, header.zipHeader.extraFieldLength, SEEK_CUR);
		header.compressedData = (char*)malloc(header.zipHeader.compressedSize);
		//read compressed data into the buffer
		fread(&header.compressedData[0], header.zipHeader.compressedSize, 1, in);
		int headerSize = ftell(in);
		if (dumpFiles)
		{
			//Work out how to decompress XML file from compressedSize -> uncompressSize

			//char* uncompressedData = (char*)malloc(header.zipHeader.uncompressedSize);
			//test_inflate((Byte*)header.compressedData, header.zipHeader.compressedSize,
				//(Byte*)uncompressedData, header.zipHeader.uncompressedSize);

			//FILE* out = fopen("TEST.ZIP", "wb");
			//fwrite(&header, headerSize, 1, out);
			//fclose(out);
		}

		fclose(in);
		return true;
	}

	return false;
}

void XLSX::Destroy()
{
	free(header.filename);
	free(header.compressedData);
}