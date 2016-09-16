#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// A struct to contain pixel information.
typedef struct RGBPixel {
	unsigned int r, g, b;
} RGBPixel;

// A simple main function to parse command line arguments and do some error checking.
// Format translation will be handled by separate files that the main will call.
int main(int argc, char *argv[]) {
	// check for enough command line arguments
	if (argc != 4) {
		fprintf(stdout, "Sorry, that is not a valid number of arguments (must be exactly 3).\n");
		fprintf(stderr, "Error: Too few arguments.\n");
		return(1);
	}

	unsigned char format_to = *argv[1];

	// check that format to translate to is valid
	if (format_to != '3' && format_to != '6') {
		fprintf(stdout, "Sorry, your first argument must specify a valid format to translate to (must be 3 or 6).\n");
		fprintf(stderr, "Error: Invalid translation format.\n");
		return(1);
	}

	FILE* input = fopen(argv[2], "r");

	// check that input file exists
	if (!input) {
		fprintf(stdout, "Sorry, the input file does not exist.\n");
		fprintf(stderr, "Error: Invalid input filename.\n");
		fclose(input);
		return(1);
	}

	// check that file extension is .ppm
	char output_name[30];
	strcpy(output_name, argv[3]);
	int output_name_length = strlen(output_name);
	char output_ext[5] = {'\0'};
	int m = 0;
	for (int i = output_name_length - 4; i < output_name_length; i++) {
		output_ext[m] = output_name[i];
		m++;
	}
	if (strcmp(output_ext, ".ppm") != 0) {
		fprintf(stdout, "Sorry, the output file does not have a valid extension (must be .ppm).\n");
		fprintf(stderr, "Error: Invalid output filename.\n");
		fclose(input);
		return(1);
	}

	///////////////////////////////////////////
	//=======MAGIC NUMBER BEGINS HERE========//
	
	char magic_number[] = {fgetc(input), fgetc(input), '\0'};
	fgetc(input);

	// check that input file is a valid type
	if (strcmp("P3", magic_number) != 0 && strcmp("P6", magic_number) != 0) {
		fprintf(stdout, "Sorry, the input file is not a supported file type (must be .ppm, P3 or P6).\n");
		fprintf(stderr, "Error: Invalid input file type.\n");
		fclose(input);
		return(1);
	}

	///////////////////////////////////////////
	//=======COMMENT LINES BEGINS HERE=======//

	char next = fgetc(input);
	int comment_counter = 0;
	while (next == '#') {
		comment_counter++;
		char comment_line[100];
		fgets(comment_line, 100, input);
		next = fgetc(input);
	}

	int line_counter = comment_counter + 3;

	///////////////////////////////////////////
	//========IMAGE WIDTH BEGINS HERE========//

	char width[4] = {'\0'};
	int i = 0;
	while (next != ' ') {
		width[i++] = next;
		next = fgetc(input);
	}

	int width_num;
	sscanf(width, "%d", &width_num);

	///////////////////////////////////////////
	//=======IMAGE HEIGHT BEGINS HERE========//
	next = fgetc(input);

	char height[4] = {'\0'};
	int j = 0;
	while (next != '\n') {
		height[j++] = next;
		next = fgetc(input);
	}

	int height_num;
	sscanf(height, "%d", &height_num);
	
	///////////////////////////////////////////
	//=======CHANNEL SIZE BEGINS HERE========//

	next = fgetc(input);

	char channel_size[4] = {'\0'};
	int k = 0;
	while (next != '\n') {
		channel_size[k++] = next;
		next = fgetc(input);
	}

	int channel_size_num;
	sscanf(channel_size, "%d", &channel_size_num);

	// check that channel size is 8 bits
	if (channel_size_num != 255) {
		fprintf(stdout, "Sorry, the input file does not support a valid maximum RGB channel size (must be 255).\n");
		fprintf(stderr, "Error: Invalid RGB channel size.\n");
		fclose(input);
		return(1);
	}

	///////////////////////////////////////////
	//======READING HEADER BEGINS HERE=======//

	rewind(input);
	char header[10000] = {'\0'};
	next = fgetc(input);
	int header_pos = 0;
	while (line_counter > 0) {
		if (next == '\n') {
			line_counter--;
		}
		header[header_pos++] = next;
		next = fgetc(input);
	}
	header[1] = format_to;

	///////////////////////////////////////////
	//====READING IMAGE DATA BEGINS HERE=====//

	ungetc(next, input);

	// buffer to hold all of the pixel data
	RGBPixel pixelmap_array[height_num][width_num];

	if (strcmp("P3", magic_number) == 0) {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {

				char red[4] = {'\0'};
				char green[4] = {'\0'};
				char blue[4] = {'\0'};

				next = fgetc(input);

				// read chars for red channel
				for (int i = 0; i < 3; i++) {
					if (next == ' ' || next == '\n') {
						break;
					}

					red[i] = next;
					next = fgetc(input);
				}

				// cycle through any amount of whitespace
				while (next == ' ' || next == '\n') {
					next = fgetc(input);
				}

				// read chars for green channel
				for (int i = 0; i < 3; i++) {
					if (next == ' ' || next == '\n') {
						break;
					}

					green[i] = next;
					next = fgetc(input);
				}

				// cycle through any amount of whitespace
				while (next == ' ' || next == '\n') {
					next = fgetc(input);
				}

				// read chars for blue channel
				for (int i = 0; i < 3; i++) {
					if (next == ' ' || next == '\n') {
						break;
					}

					blue[i] = next;
					next = fgetc(input);
				}

				// cycle through any amount of whitespace
				while (next == ' ' || next == '\n') {
					next = fgetc(input);
				}

				ungetc(next, input);

				// convert channel char arrays to ints
				int red_val = atoi(red);
				int green_val = atoi(green);
				int blue_val = atoi(blue);

				// check that channel values are not above given maximum
				if (red_val > channel_size_num || green_val > channel_size_num || blue_val > channel_size_num) {
					fprintf(stdout, "Sorry, one or more pixel RGB channels have a value larger than the specified maximum.\n");
					fprintf(stderr, "Error: Invalid RGB channel value.\n");
					fclose(input);
					return(1);
				}

				// store pixel data into buffer
				pixelmap_array[row_count][col_count].r = red_val;
				pixelmap_array[row_count][col_count].g = green_val;
				pixelmap_array[row_count][col_count].b = blue_val;
			}
		}
	} else {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {
				
				// read binary pixel data into arrays
				int red_channel[1];
				int green_channel[1];
				int blue_channel[1];

				fread(red_channel, 1, 1, input);
				fread(green_channel, 1, 1, input);
				fread(blue_channel, 1, 1, input);

				// store pixel data into buffe
				pixelmap_array[row_count][col_count].r = red_channel[0];
				pixelmap_array[row_count][col_count].g = green_channel[0];
				pixelmap_array[row_count][col_count].b = blue_channel[0];
			}
		}
	}

	///////////////////////////////////////////
	//=======WRITING DATA BEGINS HERE========//

	FILE* output = fopen(argv[3], "w");

	// write header to output file
	fprintf(output, "%s", header);

	if (format_to == '3') {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {
				
				// write pixel data in ASCII characters
				int red_channel = pixelmap_array[row_count][col_count].r;
				int green_channel = pixelmap_array[row_count][col_count].g;
				int blue_channel = pixelmap_array[row_count][col_count].b;

				fprintf(output, "%i %i %i\n", red_channel, green_channel, blue_channel);
			}
		}
	} else {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {
				
				// write pixel data in raw bytes
				int red_channel[1] = {pixelmap_array[row_count][col_count].r};
				int green_channel[1] = {pixelmap_array[row_count][col_count].g};
				int blue_channel[1] = {pixelmap_array[row_count][col_count].b};

				fwrite(red_channel, 1, 1, output);
				fwrite(green_channel, 1, 1, output);
				fwrite(blue_channel, 1, 1, output);
			}
		}
	}

	// close files and clean up
	fclose(input);
	fclose(output);
	fflush(stdout);
	return(0);
}