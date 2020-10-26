#include <geotiff/geotiff.h>
#include <geotiff/xtiffio.h>
#include "tiffio.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>

#define PRINT_DEBUG_1 0
#define PRINT_DEBUG_2 0

static const float CLIPPED_MAX_HEIGHT = 511.5f;
static const float CLIPPED_MIN_HEIGHT = 0.0f;

void write_subimage(float* output_buf, TIFF *tif, int input_idx, int input_count, bool is_horizontal)
{
	uint32 width, height;

	TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &height );

    float* dest_buf = (float*)malloc(width * height * sizeof(float));

	uint32 imageLength;
    uint32 tileWidth = 0, tileHeight = 0;
    uint32 tileDepth = 0;
    uint32 x, y;
    tdata_t buf;
    TIFFGetField(tif, TIFFTAG_TILEDEPTH, &tileDepth);
    printf("Tile depth: %d\n", tileDepth);
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileHeight);
    printf("Tile dimension: %dx%d\n", tileWidth, tileHeight);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
    printf("Image length: %d\n", imageLength);

    if (tileWidth != 0 && tileHeight != 0)
    {
    	printf("Image is tiled\n");
	    buf = _TIFFmalloc(TIFFTileSize(tif));
	    printf("Tile size in bytes: %ld\n", TIFFTileSize(tif));

	    for (y = 0; y < imageLength; y += tileHeight)
	    {
	        for (x = 0; x < width; x += tileWidth)
	        {
	            TIFFReadTile(tif, buf, x, y, 0, 0);

	            float* data = (float*)buf;
	            for(uint32 i = y, idy = 0; i < y + tileHeight; ++i, ++idy)
	            {
	            	if ( i == height)
	            		break;

	                for(uint32 j = x, idx=0; j < x + tileWidth; ++j, ++idx)
	                {
	                	if ( j == width)
	                		break;

	                	dest_buf[i*width+j] = data[idy*tileWidth+idx];
	                }
	            }
	        }
	    }
	    _TIFFfree(buf);
	}
	else
	{
		printf("Image is stripped or scanlined. Assuming it's stripped!\n");

		tstrip_t strip;
		tsize_t strip_size = TIFFStripSize(tif);
		printf("Strip size in bytes: %ld\n", strip_size);
		printf("Strip number: %d\n", TIFFNumberOfStrips(tif));

		int ptr = 0;
		buf = _TIFFmalloc(strip_size);
		for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++)
		{
			tsize_t read_bytes = TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1);
			// dest_buf is type float* thus the casting
			memcpy(&((uint8*)dest_buf)[ptr], buf, read_bytes);
			ptr += read_bytes;
			
		}

		_TIFFfree(buf);
	}

    // copy dest_buf to subimage of output_buf
    for (y = 0; y < imageLength; ++y)
    {
        for (x = 0; x < width; ++x)
        {
        	float v = dest_buf[y * width + x];
        	if (is_horizontal)
    		{
        		output_buf[y * (width * input_count) + input_idx * width + x] = v;
        	}
        	else
        	{
        		output_buf[(input_idx * width * height) + y * width +  x] = v;
        	}
        }
    }


    free(dest_buf);
}

// NB: This will map float range [0 - 512] to uint16 range [0, 65536]
// rather than [-256, 256] to [0, 65536]
// so in Unreal you need to shift down on Z axis by 256 meters or 25600 units
uint16 encode_float_to_uint16(float f)
{
	uint16 iheight = (uint16)(f * 128.0f);
	return iheight;
}

int main(int argc, char* argv[])
{
	bool parse_input_files = false;
	char* const* input_files = NULL;
	char* const* input_files_end = NULL;
	const char* output_file = NULL;
	bool parse_mode = false;
	bool mode_crop_square = false;
	bool mode_stitch_horizontal = false;
	bool mode_stitch_vertical = false;
	uint32 padding_width = 0;
	uint32 padding_height = 0;
	float padding_val = 0.0f;

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
			if (strcmp(argv[i], "crop_square") == 0)
			{
				mode_crop_square = true;
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
			// padding width
			if (strcmp(argv[i], "-padding_w") == 0)
			{
				padding_width = (uint32)atoi(argv[i+1]);
				// skip next 
				i++;
			}
			// padding height
			if (strcmp(argv[i] , "-padding_h") == 0)
			{
				padding_height = (uint32)atoi(argv[i+1]);
				// skip next 
				i++;
			}
			// padding value
			if (strcmp(argv[i], "-padding_val") == 0)
			{
				padding_val = (float)atof(argv[i+1]);
				// skip next
				i++;
			}
		}
	}

	printf("Mode: \n");
	printf("Crop to square: %d\n", mode_crop_square);
	printf("Stitch horizontal: %d\n", mode_stitch_horizontal);
	printf("Stitch vertical: %d\n", mode_stitch_vertical);
	printf("Padding to (%d, %d) with height %f\n", padding_width, padding_height, padding_val);
	printf("Input: \n");

	int input_file_count = 0;
	for(int i = 0; input_files + i != input_files_end; ++i)
	{
		const char* input_file = input_files[i];
		printf("%s\n", input_file);
		++input_file_count;
	}
	printf("Output: %s\n", output_file);

	uint32 check_width = -1, check_height = -1;
	bool input_consistent = true;
	// check input files consistency
	for(int input_idx = 0; input_files + input_idx != input_files_end; ++input_idx)
	{
		const char* input_file = input_files[input_idx];
		TIFF *tif = (TIFF*)NULL;

		tif = XTIFFOpen(input_file, "r");
		if (tif)
		{
			printf("Opened: %s\n", input_file);

			uint32 width, height;

			TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &width );
	        TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &height );

	        XTIFFClose(tif);

	        if (check_width == -1)
	        {
	        	check_width = width;
	        }
	        else
	        {
	        	if (check_width != width)
	        	{
	        		printf("Input image has different width: %d against %d\n", check_width, width);
	        		input_consistent = false;
	        		break;
	        	}
	        }

	        if (check_height == -1)
	        {
	        	check_height = height;
	        }
	        else
	        {
	        	if (check_height != height)
	        	{
	        		printf("Input image has different height: %d against %d\n", check_height, height);
	        		input_consistent = false;
	        		break;
	        	}
	        }

	        printf("Image dimension: %dx%d\n", width, height);
	    }
	    else
	    {
	    	input_consistent = false;
	    	break;
	    }
	}

	if (!input_consistent)
		return -1;

	// prepare destBuf
	uint32 dest_width, dest_height;

	if (mode_stitch_horizontal)
	{
		dest_width = check_width * input_file_count;
		dest_height = check_height;
	}
	else if (mode_stitch_vertical)
	{
		dest_width = check_width;
		dest_height = check_height * input_file_count;
	}
	else
	{
		dest_width = check_width;
		dest_height = check_height;
	}

	float* destBuf = (float*)malloc(dest_width * dest_height * sizeof(float));

	for(int input_idx = 0; input_files + input_idx != input_files_end; ++input_idx)
	{
		const char* input_file = input_files[input_idx];
		TIFF *tif = (TIFF*)NULL;

		tif = XTIFFOpen(input_file, "r");
		if (tif)
		{
			printf("Opened: %s\n", input_file);

			write_subimage(destBuf, tif, input_idx, input_file_count, mode_stitch_horizontal);

			XTIFFClose(tif);
		}
	}
#if PRINT_DEBUG_1
    for (int y = 0; y < dest_height; ++y)
    {
        for(int x = 0; x < dest_width; ++x)
        {
        	printf("Pixel at (%d, %d) = %f\n", x, y, destBuf[y * dest_width + x]);
        }
    }
#endif

    // encode into r16 raw format

    // padding
    uint32 padded_width = padding_width != 0 ? padding_width : dest_width;
    uint32 padded_height = padding_height != 0 ? padding_height : dest_height;

    uint32 output_width = padded_width, output_height = padded_height;

    // ?? padding and crop_square are compatible ??
    if (mode_crop_square)
    {
        if (output_width != output_height)
        {
        	if (output_width > output_height)
        	{
        		output_width = output_height;
        	}
        	else
        	{
        		output_height = output_width;
        	}
        }
    }

    uint32 cropped_width_diff = padded_width - output_width;
    uint32 cropped_height_diff = padded_height - output_height;
    float max_height = -FLT_MAX, min_height = FLT_MAX;

    printf("Intermediate buffer dimension = (%d, %d)\n", dest_width, dest_height);
   	printf("Padded dimension = (%d, %d)\n", padded_width, padded_height);
   	printf("Output dimension = (%d, %d)\n", output_width, output_height);
   	printf("Cropped dimension diff = (%d, %d)\n", cropped_width_diff, cropped_height_diff);

    uint16* r16Buf = (uint16*)malloc(output_width * output_height * sizeof(uint16));
    if (r16Buf)
    {
		// cropped nothing, the input is a square
		// convert float to 16bit integer
    	for (uint32 y = 0; y < output_height; ++y)
        {
            for(uint32 x = 0; x < output_width; ++x)
            {
            	if ( y >= dest_height || x >= dest_width)
            	{
            		//uint16 iheight_padded = (uint16)((padding_val + 256.0f) * 128.0f);
            		uint16 iheight_padded = encode_float_to_uint16(padding_val);
#if PRINT_DEBUG_2
            		// padding
            		printf("Padding at (%d, %d) = %d\n", x, y, iheight_padded);
#endif
            		r16Buf[y * output_width + x] = iheight_padded;

            		continue;
            	}

            	float height = destBuf[(y + cropped_height_diff/2) * dest_width + (x + cropped_width_diff/2)];

            	// TODO: clipping arguments
            	if (height > CLIPPED_MAX_HEIGHT)
            		height = CLIPPED_MAX_HEIGHT;
            	if (height < CLIPPED_MIN_HEIGHT)
            		height = CLIPPED_MIN_HEIGHT;


            	if (max_height < height)
            		max_height = height;

            	if (height < min_height)
            		min_height = height;
            	
            	//uint16 iheight = (uint16)((height + 256.0f) * 128.0f);
            	uint16 iheight = encode_float_to_uint16(height);
            	r16Buf[y * output_width + x] = iheight;
#if PRINT_DEBUG_2
            	printf("Encoded at (%d, %d) = %d\n", x, y, r16Buf[y * output_width + x]);
#endif
            }
        }

        if (cropped_width_diff != 0)
        	printf("Cropped width\n");

        if (cropped_height_diff != 0)
        	printf("Cropped height\n");
    	
    	printf("Maxmimum height: %f\n", max_height);
    	printf("Minimum height: %f\n", min_height);

        FILE* rawFP = fopen(output_file, "w");
        if (rawFP)
        {
        	fwrite(r16Buf, output_width * output_height * sizeof(uint16), 1, rawFP);
        	fflush(rawFP);
        	fclose(rawFP);

        	printf("Writing to %s successfully. Dimension: (%d, %d).\n", output_file, output_width, output_height);
        }
        else
        {
        	printf("Error writing to %s\n", output_file);
        }

        free(r16Buf);
    }
	free(destBuf);

	return 0;
}