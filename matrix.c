#include "matrix.h"

#include <math.h>
#include <stdlib.h>

int create_matrix(int rows, int columns, matrix_t *result) {
  int status = OK;
  if (result == NULL || rows < 1 || columns < 1) {
    status = ERROR;
  }
  if (status == OK) {
    result->rows = 0;
    result->columns = 0;
    result->matrix = NULL;
    result->matrix = (double **)calloc((size_t)rows, sizeof(double *));
    if (result->matrix == NULL) {
      status = ERROR;
    } else {
      result->rows = rows;
      result->columns = columns;
    }
  }
  if (status == OK) {
    for (int i = 0; i < rows; i++) {
      result->matrix[i] = (double *)calloc((size_t)columns, sizeof(double));
      if (result->matrix[i] == NULL) {
        status = ERROR;
        break;
      }
    }
  }
  if (status != OK && result != NULL) {
    remove_matrix(result);
  }
  return status;
}

void remove_matrix(matrix_t *A) {
  if (A != NULL && A->matrix != NULL) {
    for (int i = 0; i < A->rows; i++) {
      free(A->matrix[i]);
    }
    free(A->matrix);
    A->matrix = NULL;
    A->rows = 0;
    A->columns = 0;
  }
}

int eq_matrix(matrix_t *A, matrix_t *B) {
  int result = SUCCESS;
  if (A == NULL || B == NULL || A->matrix == NULL || B->matrix == NULL ||
      A->rows != B->rows || A->columns != B->columns) {
    result = FAILURE;
  } else {
    for (int i = 0; i < A->rows && result == SUCCESS; i++) {
      for (int j = 0; j < A->columns && result == SUCCESS; j++) {
        double diff = fabs(A->matrix[i][j] - B->matrix[i][j]);
        if (diff > EPSILON) {
          result = FAILURE;
        }
      }
    }
  }
  return result;
}

int sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = OK;
  if (A == NULL || B == NULL || result == NULL || A->matrix == NULL ||
      B->matrix == NULL) {
    status = ERROR;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    status = CALC_ERROR;
  } else {
    status = create_matrix(A->rows, A->columns, result);
    if (status == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
        }
      }
    }
  }
  return status;
}

int sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = OK;
  if (A == NULL || B == NULL || result == NULL || A->matrix == NULL ||
      B->matrix == NULL) {
    status = ERROR;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    status = CALC_ERROR;
  } else {
    status = create_matrix(A->rows, A->columns, result);
    if (status == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
        }
      }
    }
  }
  return status;
}

int mult_number(matrix_t *A, double number, matrix_t *result) {
  int status = OK;
  if (A == NULL || result == NULL || A->matrix == NULL) {
    status = ERROR;
  } else {
    status = create_matrix(A->rows, A->columns, result);
    if (status == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = number * A->matrix[i][j];
        }
      }
    }
  }
  return status;
}

int mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int status = OK;
  if (A == NULL || B == NULL || result == NULL || A->matrix == NULL ||
      B->matrix == NULL) {
    status = ERROR;
  } else if (A->columns != B->rows) {
    status = CALC_ERROR;
  } else {
    status = create_matrix(A->rows, B->columns, result);
    if (status == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->columns; j++) {
          result->matrix[i][j] = 0.0;
          for (int k = 0; k < A->columns; k++) {
            result->matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
          }
        }
      }
    }
  }
  return status;
}

int transpose(matrix_t *A, matrix_t *result) {
  int status = OK;
  if (A == NULL || result == NULL || A->matrix == NULL) {
    status = ERROR;
  } else {
    status = create_matrix(A->columns, A->rows, result);
    if (status == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[j][i] = A->matrix[i][j];
        }
      }
    }
  }
  return status;
}

static int get_minor(matrix_t *A, int row_skip, int column_skip,
                     matrix_t *minor) {
  int status = create_matrix(A->rows - 1, A->columns - 1, minor);
  if (status == OK) {
    int mi = 0;
    for (int i = 0; i < A->rows; i++) {
      if (i == row_skip) continue;
      int mj = 0;
      for (int j = 0; j < A->columns; j++) {
        if (j == column_skip) continue;
        minor->matrix[mi][mj] = A->matrix[i][j];
        mj++;
      }
      mi++;
    }
  }
  return status;
}

static int algebr_complements(matrix_t *A, matrix_t *result) {
  int status = OK;
  for (int i = 0; i < A->rows && status == OK; i++) {
    for (int j = 0; j < A->columns && status == OK; j++) {
      matrix_t minor;
      status = get_minor(A, i, j, &minor);
      if (status == OK) {
        double det_minor = 0.0;
        status = determinant(&minor, &det_minor);
        if (status == OK) {
          double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
          result->matrix[i][j] = sign * det_minor;
        }
      }
      remove_matrix(&minor);
    }
  }
  return status;
}

int calc_complements(matrix_t *A, matrix_t *result) {
  int status = OK;
  if (A == NULL || result == NULL || A->matrix == NULL) {
    status = ERROR;
  } else if (A->rows < 1 || A->rows != A->columns) {
    status = CALC_ERROR;
  } else if (A->rows == 1) {
    status = create_matrix(1, 1, result);
    if (status == OK) {
      result->matrix[0][0] = 1.0;
    }
  } else {
    status = create_matrix(A->rows, A->columns, result);
    if (status == OK) {
      status = algebr_complements(A, result);
    }
  }
  return status;
}

int determinant(matrix_t *A, double *result) {
  int status = OK;
  if (A == NULL || result == NULL || A->matrix == NULL) {
    status = ERROR;
  } else if (A->rows < 1 || A->rows != A->columns) {
    status = CALC_ERROR;
  } else if (A->rows == 1) {
    *result = A->matrix[0][0];
  } else if (A->rows == 2) {
    *result =
        A->matrix[0][0] * A->matrix[1][1] - A->matrix[0][1] * A->matrix[1][0];
  } else {
    *result = 0.0;
    for (int j = 0; j < A->columns && status == OK; j++) {
      matrix_t minor;
      status = get_minor(A, 0, j, &minor);
      if (status == OK) {
        double det_minor = 0.0;
        status = determinant(&minor, &det_minor);
        if (status == OK) {
          double sign = (j % 2 == 0) ? 1.0 : -1.0;
          *result += A->matrix[0][j] * sign * det_minor;
        }
        remove_matrix(&minor);
      }
    }
  }
  return status;
}

static int calc_inverse_general(matrix_t *A, double det, matrix_t *result) {
  matrix_t complements = {0};
  matrix_t transposed = {0};
  int status = calc_complements(A, &complements);
  if (status == OK) {
    status = transpose(&complements, &transposed);
  }
  if (status == OK) {
    status = mult_number(&transposed, 1.0 / det, result);
  }
  remove_matrix(&complements);
  remove_matrix(&transposed);
  return status;
}

int inverse_matrix(matrix_t *A, matrix_t *result) {
  int status = OK;
  if (A == NULL || result == NULL || A->matrix == NULL) {
    status = ERROR;
  } else if (A->rows < 1 || A->rows != A->columns) {
    status = CALC_ERROR;
  } else {
    double det = 0.0;
    status = determinant(A, &det);
    if (status == OK) {
      if (fabs(det) <= EPSILON) {
        status = CALC_ERROR;
      } else if (A->rows == 1) {
        status = create_matrix(1, 1, result);
        if (status == OK) {
          result->matrix[0][0] = 1.0 / det;
        }
      } else {
        status = calc_inverse_general(A, det, result);
      }
    }
  }
  return status;
}