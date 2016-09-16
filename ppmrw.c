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
	if (argc != 4) {
		fprintf(stdout, "Sorry, that is not a valid number of arguments (must be exactly 3).\n");
		fprintf(stderr, "Error: Too few arguments.\n");
		return(1);
	}

	unsigned char format_to = *argv[1];

	if (format_to != '3' && format_to != '6') {
		fprintf(stdout, "Sorry, your first argument must specify a valid format to translate to (must be 3 or 6).\n");
		fprintf(stderr, "Error: Invalid translation format.\n");
		return(1);
	}

	FILE* input = fopen(argv[2], "r");

	if (!input) {
		fprintf(stdout, "Sorry, the input file does not exist.\n");
		fprintf(stderr, "Error: Invalid input filename.\n");
	}

	///////////////////////////////////////////
	//=======MAGIC NUMBER BEGINS HERE========//
	
	char magic_number[] = {fgetc(input), fgetc(input), '\0'};
	fgetc(input);

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
	fprintf(stdout, "%s\n", header);

	///////////////////////////////////////////
	//====READING IMAGE DATA BEGINS HERE=====//

	ungetc(next, input);

	RGBPixel pixelmap_array[height_num][width_num];

	if (strcmp("P3", magic_number) == 0) {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {

				char red[4] = {'\0'};
				char green[4] = {'\0'};
				char blue[4] = {'\0'};

				next = fgetc(input);

				for (int i = 0; i < 3; i++) {
					if (next == ' ' || next == '\n') {
						break;
					}

					red[i] = next;
					next = fgetc(input);
				}

				while (next == ' ' || next == '\n') {
					next = fgetc(input);
				}

				for (int i = 0; i < 3; i++) {
					if (next == ' ' || next == '\n') {
						break;
					}

					green[i] = next;
					next = fgetc(input);
				}

				while (next == ' ' || next == '\n') {
					next = fgetc(input);
				}

				for (int i = 0; i < 3; i++) {
					if (next == ' ' || next == '\n') {
						break;
					}

					blue[i] = next;
					next = fgetc(input);
				}

				while (next == ' ' || next == '\n') {
					next = fgetc(input);
				}

				ungetc(next, input);

				int red_val = atoi(red);
				int green_val = atoi(green);
				int blue_val = atoi(blue);

				pixelmap_array[row_count][col_count].r = red_val;
				pixelmap_array[row_count][col_count].g = green_val;
				pixelmap_array[row_count][col_count].b = blue_val;
			}
		}
	} else {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {
				
				int red_channel[1];
				int green_channel[1];
				int blue_channel[1];

				fread(red_channel, 1, 1, input);
				fread(green_channel, 1, 1, input);
				fread(blue_channel, 1, 1, input);

				pixelmap_array[row_count][col_count].r = red_channel[0];
				pixelmap_array[row_count][col_count].g = green_channel[0];
				pixelmap_array[row_count][col_count].b = blue_channel[0];
			}
		}
	}

	///////////////////////////////////////////
	//=======WRITING DATA BEGINS HERE========//

	FILE* output = fopen(argv[3], "w");

	fprintf(output, "%s", header);

	if (format_to == '3') {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {
				
				int red_channel = pixelmap_array[row_count][col_count].r;
				int green_channel = pixelmap_array[row_count][col_count].g;
				int blue_channel = pixelmap_array[row_count][col_count].b;

				fprintf(output, "%i %i %i\n", red_channel, green_channel, blue_channel);
			}
		}
	} else {
		for (int row_count = 0; row_count < height_num; row_count++) {
			for (int col_count = 0; col_count < width_num; col_count++) {
				
				int red_channel[1] = {pixelmap_array[row_count][col_count].r};
				int green_channel[1] = {pixelmap_array[row_count][col_count].g};
				int blue_channel[1] = {pixelmap_array[row_count][col_count].b};

				fwrite(red_channel, 1, 1, output);
				fwrite(green_channel, 1, 1, output);
				fwrite(blue_channel, 1, 1, output);
			}
		}
	}

	fclose(input);
	fclose(output);
	fflush(stdout);
	return(0);
}