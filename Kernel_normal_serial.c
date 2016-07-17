#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>

struct dimensions { /* struct for the dimensions of the arrays */
	int ncolumn;
	int nline;
};

struct parameters { /* struct for the mean and standard deviation of the data */
	double mean;
	double variance;
};

int count(struct dimensions *dim); /* lines 54-70*/
float ** input(struct dimensions *dim); /* lines 72-96 */
int getpars(struct parameters *par);/* lines 98-116 */
double ** calc(float **value, struct dimensions *dim, struct parameters *var);/* lines 118-139*/
void freevalue(float **value, struct dimensions *dim);/* lines 141-147 */
int output(double **result, struct dimensions *dim); /* lines 149 -164 */
void freeresult(double **result, struct dimensions *dim); /* lines 166-172 */

int main(void) {
	struct dimensions dim; /* creating pointer for the struct in which the dimension of the array of the data are stores */
	struct parameters par; /* creating pointer for the struct in which the mean and standard deviation are stores */
	float **value; /* array of raw data stored in a file */
	double **result; /* array of the calculated data */
	if (count(&dim) == 1) { /* initializing method for counting the number of columns and checking if it was successful */
		perror("Error: could not access data"); /* message shown if an error occurred */
		return 1; /* terminating the program if an error occurred */
	}
	if ((value = input(&dim)) == NULL) { /* initializing method for inputting the raw data and storing it into a local array if it was successful */
		perror("Error: could not write input"); /* message shown if an error occurred */
		return 1; /* terminating the program if an error occurred */
	}
	if (getpars(&par) == 1) { /* initializing method for getting the mean and standard deviation and checking if it was successful */
		perror("Error: could not get parameters"); /* message shown if an error occurred */
		return 1; /* terminating the program if an error occurred */
	}
	if ((result = calc(value, &dim, &par)) == NULL) { /* initializing method for inputting the processed data and storing it into a local array if it was successful */
		perror("Error: could not allocate memory"); /* message shown if an error occurred */
		return 1; /* terminating the program if an error occurred */
	}
	freevalue(value, &dim); /* initializing method for freeing the raw data array from memory */
	if (output(result, &dim) == 1) {  /* initializing method for writing the processed data array into a file and checking if it was successful */
		perror("Error: could not allocate memory"); /* message shown if an error occurred */
		return 1; /* terminating the program if an error occurred */
	}
	freeresult(result, &dim); /* initializing method for freeing the processed data from memory */
	return 0; /*successfully ending the program if no errors occured */
}

int count(struct dimensions *dim) { /*this method counts the number of columns in the array of raw data */
	FILE *count = fopen("data", "r"); /* creating pointer for the file in which the raw data is stored */
	if (count == NULL) { /* checking if the pointer was created successfully */
		return 1; /* value returned if the operation results in a null pointer */
	}
	float buff; /* variable where useless data is stored */
	int ncolumn;/* variable where the number of columns will be stored */
	fscanf(count, "%f", &buff);
	ncolumn = 1;
	while (fgetc(count) != '\n') { /* this loop will increment the value ncolums until a newline character is found */
		fscanf(count, "%f", &buff);
		ncolumn++;
	}
	fclose(count);/* closing the file */
	dim->ncolumn = ncolumn; /*transferring the value of the variable to a struct in order for it to be accessed by the other methods */
	return 0; /* value returned if the operation was successful */
}

float ** input(struct dimensions *dim) { /* this method inputs the raw data found in the file "data" into an array */
	static float **value; /* two-dimensional array in which the raw data will be stored */
	int i;
	int nline; /* number of lines in the raw data array */
	nline = 0;
	FILE *input = fopen("data", "r"); /* opening the file in which the raw data is stored */
	value = calloc(dim->ncolumn, sizeof(float*)); /* allocating memory for the array of raw data */
	if (value == NULL) { /*checking if the allocation was successful */
		return NULL; /*value returned if the allocation failed */
	}
	do {
		for (i = 0; i < dim->ncolumn; i++) { /* this loop will allocate an array for each column of raw data within the previously allocated array */
			value[i] = realloc(value[i], (nline + 1) * sizeof(float*));
			if (value == NULL) {
				return NULL;
			}
			fscanf(input, "%f", &value[i][nline]); /* inputting the raw data in the two-dimensional array */
		}
		nline++;
	} while (fgetc(input) != EOF); /* loop will end when the end of the file is reached */
	nline--; /* the variable storing the number of lines needs to be decremented since the loop will always result in a value which is one unit greater */
	dim->nline = nline; /*transferring the value of the variable to a struct in order for it to be accessed by the other methods */
	fclose(input); /* file containing raw data is closed */
	return value; /* the method returns the array of raw data if the operation was successful */
}

int getpars(struct parameters *par) { /* this function will get the mean and the standard deviation of the set of raw data*/
	float mu; /* variable for the mean */
	float tau; /* variable for the standard deviation */
	FILE *mean = fopen("mean", "r"); /*opening the file in which the mean is stored */
	if (mean == NULL) { /* checking if the pointer was created successfully */
		return 1; /* value returned if the operation results in a null pointer */
	}
	fscanf(mean, "%f", &mu); /* inputing the value of the mean */
	fclose(mean); /*closing the file */
	FILE *std_dev = fopen("standard_deviation", "r"); /*opening the file in which the standard deviation is stored */
	if (std_dev == NULL) { /* checking if the pointer was created successfully */
		return 1; /* value returned if the operation results in a null pointer */
	}
	fscanf(std_dev, "%f", &tau); /* inputing the value of the standard deviation */
	fclose(std_dev); /*closing the file */
	par->mean = mu; /*transferring the value of the variable to a struct in order for it to be accessed by the other methods */
	par->variance = tau; /* same as above */
	return 0; /* value returned if the operation was successful*/
}

double ** calc(float **value, struct dimensions *dim, struct parameters *par) { /* this method will determine the value of the probability density function (pdf) associated with each value of the raw data */
	static double **result; /* creating a two-dimensional array for the result of the operation */
	result = calloc(dim->ncolumn, sizeof(double*)); /* allocating memory for the array of the processed data */
	if (result == NULL) { /*checking if the allocation was successful */
		return NULL; /*value returned if the allocation failed */
	}
	int i;
	int j;
	for (i = 0; i < dim->ncolumn; i++) { /* this loop will allocate an array for each column of the processed data within the previously allocated array */
		result[i] = calloc(dim->nline, sizeof(double*));
		if (result == NULL) {
			return NULL;
		}
	}
	for (j = 0; j < dim->nline; j++) { /* this loop will perform the calculation for each value of the processed data and store it into the previouly created array */
		for (i = 0; i < dim->ncolumn; i++) {
			result[i][j] = pow(M_E / sqrt(2 * M_PI * pow(par->variance, 2)),
					-pow(value[i][j] - par->mean, 2) / pow(par->variance, 2));
		}
	}
	return result; /* the method will return the processed data if it was successful */
}

void freevalue(float **value, struct dimensions *dim) { /* this method will free the raw data array from the memory */
	int i;
	for (i = 0; i < dim->ncolumn; i++) { /*this loop is freeing the arrays within the main array */
		free(value[i]);
	}
	free(value); /* this frees the main array from memory */
}

int output(double **result, struct dimensions *dim) { /* this method will output the processed data into a file */
	FILE *output = fopen("output", "w+"); /* the output file is created */
	if (output == NULL) { /* checking if the pointer was created successfully */
		return 1; /* value returned if the operation results in a null pointer */
	}
	int i;
	int j;
	for (j = 0; j < dim->nline; j++) { /* these loops will write the processed data in a file */
		for (i = 0; i < dim->ncolumn; i++) {
			fprintf(output, "%f ", result[i][j]);
		}
		fprintf(output, "\n");
	}
	fclose(output); /* closing the file*/
	return 0; /* value returned if the operation was sucessfull */
}

void freeresult(double **result, struct dimensions *dim) { /* this method will free the processed data array from the memory */
	int i;
	for (i = 0; i < dim->ncolumn; i++) { /*this loop is freeing the arrays within the main array */
		free(result[i]);
	}
	free(result); /* this frees the main array from memory */
}
