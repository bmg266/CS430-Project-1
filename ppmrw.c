#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
		//fprintf(output, "%c", next);
		//fprintf(output, "%s", comment_line);
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
	//======WRITING OUTPUT BEGINS HERE=======//

	FILE* output = fopen(argv[3], "w");
	/*fprintf(output, "P%c\n", format_to);

	fprintf(output, "%s", width);
	fprintf(output, " ");

	fprintf(output, "%s", height);
	fprintf(output, "\n");

	fprintf(output, "%s", channel_size);
	fprintf(output, "\n", next);*/

	fprintf(output, "%s", header);

	if (strcmp("P3", magic_number) == 0) {
		fprintf(stdout, "Input: P3, ");
		// read P3
	} else {
		fprintf(stdout, "Input: P6, ");
		// read P6
	}

	if (format_to == '3') {
		fprintf(stdout, "Output: P3\n");
		// write P3
	} else {
		fprintf(stdout, "Output: P6\n");
		// write P4
	}

	/*if (strcmp("P3", magic_number) == 0) {
		if (format_to == '3') {
			fprintf(stdout, "Input: P3, Output: P3\n");
			// send to p3_to_p3
		} else {
			fprintf(stdout, "Input: P3, Output: P6\n");
			// send to p3_to_p6
		}
	} else if (strcmp("P6", magic_number) == 0) {
		if (format_to == '3') {
			fprintf(stdout, "Input: P6, Output: P3\n");
			// send to p6_to_p3
		} else {
			fprintf(stdout, "Input: P6, Output: P6\n");
			// send to p6_to_p6
		}
	}*/

	fclose(input);
	fclose(output);
	fflush(stdout);
	return(0);
}