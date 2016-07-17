#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>

struct dimensions {
	int ncolumn;
	int nline;
};

struct parameters {
	double mean;
	double variance;
};

int count(struct dimensions *dim);
float ** input(struct dimensions *dim);
int getpars(struct parameters *par);
double ** calc(float **value, struct dimensions *dim, struct parameters *var);
void freevalue(float **value, struct dimensions *dim);
int output(double **result, struct dimensions *dim);
void freeresult(double **result, struct dimensions *dim);

int main(void) {
	struct dimensions dim;
	struct parameters par;
	float **value;
	double **result;
	if (count(&dim) == 1) {
		perror("Error: could not access data");
		return 1;
	}
	if ((value = input(&dim)) == NULL) {
		perror("Error: could not write input");
		return 1;
	}
	if(getpars(&par) == 1){
		perror("Error: could not get parameters");
		return 1;
	}
	if ((result = calc(value, &dim, &par)) == NULL) {
		perror("Error: could not allocate memory");
		return 1;
	}
	freevalue(value, &dim);
	if (output(result, &dim) == 1) {
		perror("Error: could not allocate memory");
		return 1;
	}
	freeresult(result, &dim);
	return 0;
}

int count(struct dimensions *dim) {
	FILE *count = fopen("data", "r");
	if (count == NULL) {
		return 1;
	}
	int buff;
	int ncolumn;
	fscanf(count, "%d", &buff);
	ncolumn = 1;
	while (fgetc(count) != '\n') {
		fscanf(count, "%d", &buff);
		ncolumn++;
	}
	fclose(count);
	dim->ncolumn = ncolumn;
	return 0;
}

float ** input(struct dimensions *dim) {
	static float **value;
	int i;
	int nline;
	nline = 0;
	FILE *input = fopen("data", "r");
	value = calloc(dim->ncolumn, sizeof(float*));
	if (value == NULL) {
		return NULL;
	}
	do {
		for (i = 0; i < dim->ncolumn; i++) {
			value[i] = realloc(value[i], (nline + 1) * sizeof(float*));
			if (value == NULL) {
				return NULL;
			}
			fscanf(input, "%f", &value[i][nline]);
		}
		nline++;
	} while (fgetc(input) != EOF);
	nline--;
	dim->nline = nline;
	fclose(input);
	return value;
}

int getpars(struct parameters *par) {
	float mu;
	float tau;
	FILE *mean = fopen("mean", "r");
	if (mean == NULL) {
		return 1;
	}
	fscanf(mean, "%f", &mu);
	fclose(mean);
	FILE *std_dev = fopen("standard_deviation", "r");
	if (std_dev == NULL) {
		return 1;
	}
	fscanf(std_dev, "%f", &tau);
	fclose(std_dev);
	par->mean = mu;
	par->variance = tau;
	return 0;
}

double ** calc(float **value, struct dimensions *dim, struct parameters *par) {
	static double **result;
	result = calloc(dim->ncolumn, sizeof(double*));
	if (result == NULL) {
		return NULL;
	}
	int i;
	int j;
	for (i = 0; i < dim->ncolumn; i++) {
		result[i] = calloc(dim->nline, sizeof(double*));
		if (result == NULL) {
			return NULL;
		}
	}
	for (j = 0; j < dim->nline; j++) {
		for (i = 0; i < dim->ncolumn; i++) {
			result[i][j] = pow(M_E / sqrt(2 * M_PI * pow(par->variance, 2)),
					-pow(value[i][j] - par->mean, 2) / pow(par->variance, 2));
		}
	}
	return result;
}

void freevalue(float **value, struct dimensions *dim) {
	int i;
	for (i = 0; i < dim->ncolumn; i++) {
		free(value[i]);
	}
	free(value);
}

int output(double **result, struct dimensions *dim) {
	FILE *output = fopen("output", "w+");
	if (output == NULL) {
		return 1;
	}
	int i;
	int j;
	for (j = 0; j < dim->nline; j++) {
		for (i = 0; i < dim->ncolumn; i++) {
			fprintf(output, "%f ", result[i][j]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
	return 0;
}

void freeresult(double **result, struct dimensions *dim) {
	int i;
	for (i = 0; i < dim->ncolumn; i++) {
		free(result[i]);
	}
	free(result);
}
