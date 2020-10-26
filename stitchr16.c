#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void write_subimage(uint16_t* destBuf, const uint16_t* srcBuf, uint32_t input_single_width, uint32_t input_single_height, 
					int input_file_idx, int input_file_count, bool is_stitch_horizontal )
{

	for(uint32_t y = 0; y < input_single_height; ++y)
	{
		for(uint32_t x = 0; x < input_single_width; ++x)
		{
			uint16_t val = srcBuf[y * input_single_width + x];
			if (is_stitch_horizontal)
			{
				destBuf[y * (input_file_count * input_single_width) + input_file_idx * input_single_width + x] = val;
			}
			else
			{
				destBuf[input_file_idx * input_single_width * input_single_height + y * input_single_width + x]	= val;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	char* const* input_files = NULL;
	char* const* input_files_end = NULL;
	const char* output_file = NULL;

	bool parse_input_files = false;
	bool parse_mode = false;
	bool mode_stitch_horizontal = false;
	bool mode_stitch_vertical = false;
	uint32_t single_width = 0;
	uint32_t single_height = 0;

	// parsing argument
	for(int i = 1; i < argc; ++i)
	{
		if (parse_mode)
		{
			if (strcmp(argv[i], "stitch_horizontal") == 0)
			{
				mode_stitch_horizontal = true;
			}
			if (strcmp(argv[i], "stitch_vertical") == 0)
			{
				mode_stitch_vertical = true;
			}
			parse_mode = false;
		}
		else if (parse_input_files)
		{
			if (argv[i][0] == '-')
			{
				input_files_end = argv + i;
				parse_input_files = false;
				// backstep to parse other arguments
				--i;
				continue;
			}
			else
			{
				// keep parsing more input files
			}
		}
		else
		{
			// input files
			if (strcmp(argv[i], "-i") == 0)
			{
				input_files = argv + i + 1;
				parse_input_files = true;
			}
			// output files
			if (strcmp(argv[i], "-o") == 0)
			{
				output_file = argv[i+1];
			}
			// mode
			if (strcmp(argv[i], "-m") == 0)
			{
				parse_mode = true;
			}
			// raw file width
			if (strcmp(argv[i], "-single_w") == 0)
			{
				single_width = (uint32_t)atoi(argv[i+1]);
				// skip next 
				i++;
			}
			// raw file height
			if (strcmp(argv[i] , "-single_h") == 0)
			{
				single_height = (uint32_t)atoi(argv[i+1]);
				// skip next 
				i++;
			}
		}
	}

	printf("Mode: \n");
	printf("Stitch horizontal: %d\n", mode_stitch_horizontal);
	printf("Stitch vertical: %d\n", mode_stitch_vertical);
	printf("Raw file dimension: (%d, %d)\n", single_width, single_height);
	printf("Input: \n");

	int input_file_count = 0;
	for(int i = 0; input_files + i != input_files_end; ++i)
	{
		const char* input_file = input_files[i];
		printf("%s\n", input_file);
		++input_file_count;
	}
	printf("Output: %s\n", output_file);

	uint32_t dest_width = single_width;
	uint32_t dest_height = single_height;
	if (mode_stitch_horizontal)
	{
		dest_width = single_width * input_file_count;
	}
	if (mode_stitch_vertical)
	{
		dest_height = single_height * input_file_count;
	}

	printf("Output dimension: (%d, %d)\n", dest_width, dest_height);

	uint32_t destBufSize = dest_width * dest_height * sizeof(uint16_t);
	uint16_t* destBuf = (uint16_t*)malloc(destBufSize);

	// NB: no consistency check as they are raw files
	for(int input_idx = 0; input_files + input_idx != input_files_end; ++input_idx)
	{
		const char* input_file = input_files[input_idx];
		
		FILE* fp = fopen(input_file, "r");
		if (fp)
		{
			printf("Opened: %s\n", input_file);
			uint32_t fileSizeSize = single_width * single_height * sizeof(uint16_t);
			uint16_t* fileBuf = (uint16_t*)malloc(fileSizeSize);
			int itemRead = fread(fileBuf, fileSizeSize, 1, fp);

			if (itemRead != 1)
				printf("Warning: bytes read(%d) inconsistent with dimension specified(%d, %d)\n", itemRead * fileSizeSize, single_width, single_height);

			write_subimage(destBuf, fileBuf, single_width, single_height, input_idx, input_file_count, mode_stitch_horizontal);

			free(fileBuf);

			fclose(fp);
		}
		else
		{
			printf("Error opening: %s\n", input_file);
			break;
		}
	}

	FILE* outputFP = fopen(output_file, "w");
	if (outputFP)
	{
		int itemWritten = fwrite(destBuf, destBufSize, 1, outputFP);
		fclose(outputFP);

		if (itemWritten == 1)
		{
			printf("Successfully written to %s\n", output_file);
		}
		else
		{
			printf("Error writing to %s\n", output_file);	
		}
	}

	free(destBuf);

	return 0;
}