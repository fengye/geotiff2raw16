#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void read_subimage(uint16_t* destBuf, const uint16_t* srcBuf, uint32_t input_width, uint32_t input_height, 
					uint32_t sub_width, uint32_t sub_height, uint32_t row, uint32_t col )
{
	uint32_t subdivision_count = (input_width / sub_width ) * (input_height / sub_height);
	uint32_t subdivision_idx = (input_width / sub_width ) * row + col;
	for(uint32_t y = 0; y < sub_height; ++y)
	{
		for(uint32_t x = 0; x < sub_width; ++x)
		{
			destBuf[y * sub_width + x] = srcBuf[((y + row * sub_height) * input_width) + (col * sub_width) + x];
		}
	}
}

int main(int argc, char* argv[])
{
	const char* input_file = NULL;
	const char* output_file_prefix = NULL;

	bool parse_input_files = false;
	uint32_t div_h = 1;
	uint32_t div_v = 1;
	uint32_t input_width = 0;
	uint32_t input_height = 0;
	uint32_t output_base_row = 1;
	uint32_t output_base_col = 1;
	// parsing argument
	for(int i = 1; i < argc; ++i)
	{
		// input file
		if (strcmp(argv[i], "-i") == 0)
		{
			input_file = argv[i + 1];
		}
		// output file
		if (strcmp(argv[i], "-o") == 0)
		{
			output_file_prefix = argv[i+1];
		}
		// output naming conventions
		if (strcmp(argv[i], "-filename_base_row") == 0)
		{
			output_base_row = (uint32_t)atoi(argv[i+1]);
			++i;
		}
		if (strcmp(argv[i], "-filename_base_col") == 0)
		{
			output_base_col = (uint32_t)atoi(argv[i+1]);
			++i;
		}
		// input width
		if (strcmp(argv[i], "-input_w") == 0)
		{
			input_width = (uint32_t)atoi(argv[i+1]);
			++i;
		}
		// input height
		if (strcmp(argv[i], "-input_h") == 0)
		{
			input_height = (uint32_t)atoi(argv[i+1]);
			++i;
		}
		// division in horizontal
		if (strcmp(argv[i], "-div_h") == 0)
		{
			div_h = (uint32_t)atoi(argv[i+1]);
			// skip next 
			i++;
		}
		// division in vertical
		if (strcmp(argv[i] , "-div_v") == 0)
		{
			div_v = (uint32_t)atoi(argv[i+1]);
			// skip next 
			i++;
		}
	}

	uint32_t sub_width = input_width / div_h;
	uint32_t sub_height = input_height / div_v;

	printf("Divide into %d x %d images\n", div_h, div_v);
	printf("Input: %s\n", input_file);
	printf("Input dimension: (%d, %d)\n", input_width, input_height);
	printf("Output will be:\n");
	for(uint32_t i = 0; i < div_v; ++i)
	{
		for(uint32_t j = 0; j < div_h; ++j)
		{
			printf("%s_%d_%d.r16\t(%d, %d)\n", output_file_prefix, i+output_base_row, j+output_base_col, sub_width, sub_height);
		}
	}

	FILE* fp = fopen(input_file, "r");
	if (fp)
	{
		printf("Opened: %s\n", input_file);
		uint32_t fileSizeSize = input_width * input_height * sizeof(uint16_t);
		uint16_t* fileBuf = (uint16_t*)malloc(fileSizeSize);
		int itemRead = fread(fileBuf, fileSizeSize, 1, fp);
		fclose(fp);

		if (itemRead != 1)
		{
			printf("Warning: bytes read(%d) inconsistent with dimension specified(%d, %d)\n", itemRead * fileSizeSize, input_width, input_height);
		}

		for(uint32_t i = 0; i < div_v; ++i)
		{
			for(uint32_t j = 0; j < div_h; ++j)
			{
				uint32_t destBufSize = sub_width * sub_height * sizeof(uint16_t);
				uint16_t* destBuf = (uint16_t*)malloc(destBufSize);

				read_subimage(destBuf, fileBuf, input_width, input_height, sub_width, sub_height, i, j);

				char output_filename[256];
				sprintf(output_filename, "%s_%d_%d.r16", output_file_prefix, i + output_base_row, j + output_base_col);
				FILE* output_fp = fopen(output_filename, "w");
				if (output_fp)
				{
					fwrite(destBuf, destBufSize, 1, output_fp);
					fclose(output_fp);
					printf("Successfully written to %s\n", output_filename);
				}

				free(destBuf);
			}
		}

		
	}

	return 0;
}