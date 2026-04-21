#include <check.h>
#include <math.h>
#include <stdlib.h>

#include "../matrix.h"

matrix_t create_matrix_from_array(int rows, int cols, double data[]) {
  matrix_t m = {0};
  ck_assert_int_eq(create_matrix(rows, cols, &m), OK);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      m.matrix[i][j] = data[i * cols + j];
    }
  }
  return m;
}

int is_identity(matrix_t *M) {
  if (M->rows != M->columns) return 0;
  for (int i = 0; i < M->rows; i++) {
    for (int j = 0; j < M->columns; j++) {
      double expected = (i == j) ? 1.0 : 0.0;
      if (fabs(M->matrix[i][j] - expected) > EPSILON) {
        return 0;
      }
    }
  }
  return 1;
}

START_TEST(test_create_valid) {
  matrix_t m = {0};
  ck_assert_int_eq(create_matrix(3, 4, &m), OK);
  ck_assert_int_eq(m.rows, 3);
  ck_assert_int_eq(m.columns, 4);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert(fabs(m.matrix[i][j]) < EPSILON);
    }
  }
  remove_matrix(&m);
}
END_TEST

START_TEST(test_create_invalid) {
  matrix_t m = {0};
  ck_assert_int_eq(create_matrix(0, 5, &m), ERROR);
  ck_assert_int_eq(create_matrix(-1, 2, &m), ERROR);
  ck_assert_int_eq(create_matrix(2, 0, &m), ERROR);
  ck_assert_int_eq(create_matrix(1, -3, &m), ERROR);
  ck_assert_int_eq(create_matrix(0, 0, &m), ERROR);
}
END_TEST

START_TEST(test_remove_valid) {
  matrix_t m = {0};
  create_matrix(2, 2, &m);
  m.matrix[0][0] = 42.0;
  remove_matrix(&m);
  ck_assert_ptr_eq(m.matrix, NULL);
  ck_assert_int_eq(m.rows, 0);
  ck_assert_int_eq(m.columns, 0);
}
END_TEST

START_TEST(test_remove_null) { remove_matrix(NULL); }
END_TEST

START_TEST(test_remove_already_empty) {
  matrix_t m = {0};
  remove_matrix(&m);
}
END_TEST

START_TEST(test_eq_same) {
  double d[] = {1.1, 2.2, 3.3, 4.4};
  matrix_t A = create_matrix_from_array(2, 2, d);
  matrix_t B = create_matrix_from_array(2, 2, d);
  ck_assert_int_eq(eq_matrix(&A, &B), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_different) {
  double d1[] = {1.0, 2.0, 3.0, 4.0};
  double d2[] = {1.0, 2.0, 3.0, 4.000001};
  matrix_t A = create_matrix_from_array(2, 2, d1);
  matrix_t B = create_matrix_from_array(2, 2, d2);
  ck_assert_int_eq(eq_matrix(&A, &B), FAILURE);
  remove_matrix(&A);
  remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_null) {
  matrix_t A = {0}, B = {0};
  create_matrix(1, 1, &A);
  ck_assert_int_eq(eq_matrix(NULL, &B), FAILURE);
  ck_assert_int_eq(eq_matrix(&A, NULL), FAILURE);
  ck_assert_int_eq(eq_matrix(NULL, NULL), FAILURE);
  remove_matrix(&A);
}
END_TEST

START_TEST(test_eq_size_mismatch) {
  matrix_t A = {0}, B = {0};
  create_matrix(2, 2, &A);
  create_matrix(2, 3, &B);
  ck_assert_int_eq(eq_matrix(&A, &B), FAILURE);
  remove_matrix(&A);
  remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_valid) {
  double a[] = {1, 2, 3, 4};
  double b[] = {5, 6, 7, 8};
  double r[] = {6, 8, 10, 12};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t B = create_matrix_from_array(2, 2, b);
  matrix_t R = {0}, Exp = create_matrix_from_array(2, 2, r);
  ck_assert_int_eq(sum_matrix(&A, &B, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&B);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_sum_invalid) {
  matrix_t A = {0}, B = {0}, R = {0};
  create_matrix(2, 2, &A);
  ck_assert_int_eq(sum_matrix(NULL, &B, &R), ERROR);
  ck_assert_int_eq(sum_matrix(&A, NULL, &R), ERROR);
  ck_assert_int_eq(sum_matrix(&A, &B, NULL), ERROR);
  remove_matrix(&A);
}
END_TEST

START_TEST(test_sub_valid) {
  double a[] = {10, 20, 30, 40};
  double b[] = {1, 2, 3, 4};
  double r[] = {9, 18, 27, 36};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t B = create_matrix_from_array(2, 2, b);
  matrix_t R = {0}, Exp = create_matrix_from_array(2, 2, r);
  ck_assert_int_eq(sub_matrix(&A, &B, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&B);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_mult_number_valid) {
  double a[] = {1, 2, 3, 4};
  double r[] = {2.5, 5.0, 7.5, 10.0};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t R = {0}, Exp = create_matrix_from_array(2, 2, r);
  ck_assert_int_eq(mult_number(&A, 2.5, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_mult_valid) {
  double a[] = {1, 2, 3, 4};
  double b[] = {5, 6, 7, 8};
  double r[] = {19, 22, 43, 50};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t B = create_matrix_from_array(2, 2, b);
  matrix_t R = {0}, Exp = create_matrix_from_array(2, 2, r);
  ck_assert_int_eq(mult_matrix(&A, &B, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&B);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_mult_incompatible) {
  matrix_t A = {0}, B = {0}, R = {0};
  create_matrix(2, 3, &A);
  create_matrix(4, 2, &B);
  ck_assert_int_eq(mult_matrix(&A, &B, &R), CALC_ERROR);
  remove_matrix(&A);
  remove_matrix(&B);
}
END_TEST

START_TEST(test_transpose_valid) {
  double a[] = {1, 2, 3, 4, 5, 6};
  double r[] = {1, 4, 2, 5, 3, 6};
  matrix_t A = create_matrix_from_array(2, 3, a);
  matrix_t R = {0}, Exp = create_matrix_from_array(3, 2, r);
  ck_assert_int_eq(transpose(&A, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_det_1x1) {
  double a[] = {5.5};
  matrix_t A = create_matrix_from_array(1, 1, a);
  double det = 0.0;
  ck_assert_int_eq(determinant(&A, &det), OK);
  ck_assert(fabs(det - 5.5) < EPSILON);
  remove_matrix(&A);
}
END_TEST

START_TEST(test_det_2x2) {
  double a[] = {1, 2, 3, 4};
  matrix_t A = create_matrix_from_array(2, 2, a);
  double det = 0.0;
  ck_assert_int_eq(determinant(&A, &det), OK);
  ck_assert(fabs(det - (1 * 4 - 2 * 3)) < EPSILON);  // -2
  remove_matrix(&A);
}
END_TEST

START_TEST(test_det_3x3) {
  double a[] = {6, 1, 1, 4, -2, 5, 2, 8, 7};
  matrix_t A = create_matrix_from_array(3, 3, a);
  double det = 0.0;
  ck_assert_int_eq(determinant(&A, &det), OK);
  ck_assert(fabs(det + 306.0) < EPSILON);
  remove_matrix(&A);
}
END_TEST

START_TEST(test_det_non_square) {
  matrix_t A = {0};
  create_matrix(2, 3, &A);
  double det = 0.0;
  ck_assert_int_eq(determinant(&A, &det), CALC_ERROR);
  remove_matrix(&A);
}
END_TEST

START_TEST(test_det_null) {
  double det = 0.0;
  ck_assert_int_eq(determinant(NULL, &det), ERROR);
  ck_assert_int_eq(determinant(NULL, NULL), ERROR);
}
END_TEST

START_TEST(test_complements_1x1) {
  double a[] = {7.0};
  matrix_t A = create_matrix_from_array(1, 1, a);
  matrix_t R = {0};
  ck_assert_int_eq(calc_complements(&A, &R), OK);
  ck_assert_int_eq(R.rows, 1);
  ck_assert_int_eq(R.columns, 1);
  ck_assert(fabs(R.matrix[0][0] - 1.0) < EPSILON);
  remove_matrix(&A);
  remove_matrix(&R);
}
END_TEST

START_TEST(test_complements_2x2) {
  double a[] = {1, 2, 3, 4};
  double r[] = {4, -3, -2, 1};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t R = {0}, Exp = create_matrix_from_array(2, 2, r);
  ck_assert_int_eq(calc_complements(&A, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_inverse_1x1) {
  double a[] = {5.0};
  double r[] = {0.2};
  matrix_t A = create_matrix_from_array(1, 1, a);
  matrix_t R = {0}, Exp = create_matrix_from_array(1, 1, r);
  ck_assert_int_eq(inverse_matrix(&A, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);
  remove_matrix(&A);
  remove_matrix(&R);
  remove_matrix(&Exp);
}
END_TEST

START_TEST(test_inverse_2x2) {
  double a[] = {4, 7, 2, 6};
  double inv[] = {0.6, -0.7, -0.2, 0.4};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t R = {0}, Exp = create_matrix_from_array(2, 2, inv);
  ck_assert_int_eq(inverse_matrix(&A, &R), OK);
  ck_assert_int_eq(eq_matrix(&R, &Exp), SUCCESS);

  matrix_t I = {0};
  mult_matrix(&A, &R, &I);
  ck_assert(is_identity(&I) == 1);

  remove_matrix(&A);
  remove_matrix(&R);
  remove_matrix(&Exp);
  remove_matrix(&I);
}
END_TEST

START_TEST(test_inverse_singular) {
  double a[] = {1, 2, 2, 4};
  matrix_t A = create_matrix_from_array(2, 2, a);
  matrix_t R = {0};
  ck_assert_int_eq(inverse_matrix(&A, &R), CALC_ERROR);
  remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_non_square) {
  matrix_t A = {0}, R = {0};
  create_matrix(2, 3, &A);
  ck_assert_int_eq(inverse_matrix(&A, &R), CALC_ERROR);
  remove_matrix(&A);
}
END_TEST

Suite *matrix_suite(void) {
  Suite *s = suite_create("matrix");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_create_valid);
  tcase_add_test(tc_core, test_create_invalid);
  tcase_add_test(tc_core, test_remove_valid);
  tcase_add_test(tc_core, test_remove_null);
  tcase_add_test(tc_core, test_remove_already_empty);

  tcase_add_test(tc_core, test_eq_same);
  tcase_add_test(tc_core, test_eq_different);
  tcase_add_test(tc_core, test_eq_null);
  tcase_add_test(tc_core, test_eq_size_mismatch);

  tcase_add_test(tc_core, test_sum_valid);
  tcase_add_test(tc_core, test_sum_invalid);
  tcase_add_test(tc_core, test_sub_valid);
  tcase_add_test(tc_core, test_mult_number_valid);
  tcase_add_test(tc_core, test_mult_valid);
  tcase_add_test(tc_core, test_mult_incompatible);

  tcase_add_test(tc_core, test_transpose_valid);

  tcase_add_test(tc_core, test_det_1x1);
  tcase_add_test(tc_core, test_det_2x2);
  tcase_add_test(tc_core, test_det_3x3);
  tcase_add_test(tc_core, test_det_non_square);
  tcase_add_test(tc_core, test_det_null);

  tcase_add_test(tc_core, test_complements_1x1);
  tcase_add_test(tc_core, test_complements_2x2);

  tcase_add_test(tc_core, test_inverse_1x1);
  tcase_add_test(tc_core, test_inverse_2x2);
  tcase_add_test(tc_core, test_inverse_singular);
  tcase_add_test(tc_core, test_inverse_non_square);

  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = matrix_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);
  int nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}