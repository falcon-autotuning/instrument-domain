#include <cmocka.h>
#include <stdarg.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instrument-domain/instrument-domain.h"

/* =========================
   Test Helpers
   ========================= */

static const double VALID_MIN = -1.0;
static const double VALID_MAX = 1.0;
static const double EPSILON = 1e-6;

/* Generate long string (longer than max) */
static void fill_long_string(char *buf, size_t size) {
  for (size_t i = 0; i < size - 1; i++) {
    buf[i] = 'A' + (i % 26);
  }
  buf[size - 1] = '\0';
}

/* =========================
   CREATE + FREE TESTS
   ========================= */

static void test_create_valid(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_create(VALID_MIN, VALID_MAX);

  assert_non_null(cs);

  instrument_domain_free(cs);
}

static void test_create_null_inputs(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_create(0, 0);
  assert_non_null(cs);

  assert_double_equal(instrument_domain_get_min(cs), 0, EPSILON);
  assert_double_equal(instrument_domain_get_max(cs), 0, EPSILON);

  instrument_domain_free(cs);
}

static void test_free_null(void **state) {
  (void)state;

  /* Should not crash */
  instrument_domain_free(NULL);
}

/* =========================
   GETTERS TESTS
   ========================= */

static void test_getters_valid(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_create(VALID_MIN, VALID_MAX);

  assert_double_equal(instrument_domain_get_min(cs), VALID_MIN, EPSILON);
  assert_double_equal(instrument_domain_get_max(cs), VALID_MAX, EPSILON);

  instrument_domain_free(cs);
}

static void test_getters_null_stack(void **state) {
  (void)state;

  assert_null(instrument_domain_get_min(NULL));
  assert_null(instrument_domain_get_max(NULL));
}

/* =========================
   SERIALIZATION TESTS
   ========================= */

static void test_serialize_valid(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_create(VALID_MIN, VALID_MAX);

  char *blob = instrument_domain_serialize(cs);

  assert_non_null(blob);

  /* Ensure expected size behavior */
  /* We cannot directly check size, but we know it's fixed */
  free(blob);
  instrument_domain_free(cs);
}

static void test_serialize_null(void **state) {
  (void)state;

  const char *blob = instrument_domain_serialize(NULL);
  assert_null(blob);
}

/* =========================
   DESERIALIZATION TESTS
   ========================= */

static void test_deserialize_valid(void **state) {
  (void)state;

  InstrumentDomain *original = instrument_domain_create(VALID_MIN, VALID_MAX);

  char *blob = instrument_domain_serialize(original);
  assert_non_null(blob);

  InstrumentDomain *copy = instrument_domain_deserialize(blob);
  assert_non_null(copy);

  assert_double_equal(instrument_domain_get_min(copy), VALID_MIN, EPSILON);
  assert_double_equal(instrument_domain_get_max(copy), VALID_MAX, EPSILON);

  free(blob);
  instrument_domain_free(original);
  instrument_domain_free(copy);
}

static void test_deserialize_null(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_deserialize(NULL);
  assert_null(cs);
}

/* =========================
   ROUNDTRIP TEST
   ========================= */

static void test_roundtrip(void **state) {
  (void)state;

  InstrumentDomain *original = instrument_domain_create(VALID_MIN, VALID_MAX);

  char *blob = instrument_domain_serialize(original);
  InstrumentDomain *copy = instrument_domain_deserialize(blob);

  assert_double_equal(instrument_domain_get_min(copy),
                      instrument_domain_get_min(original), EPSILON);

  assert_double_equal(instrument_domain_get_max(copy),
                      instrument_domain_get_max(original), EPSILON);

  free(blob);
  instrument_domain_free(original);
  instrument_domain_free(copy);
}

/* =========================
   CORRUPTED BUFFER TEST
   ========================= */

static void test_deserialize_corrupted(void **state) {
  (void)state;

  char *blob = malloc(INST_DOMAIN_SERIALIZED_MAX_SIZE);
  assert_non_null(blob);

  /* Fill with garbage */
  memset(blob, 0xFF, INST_DOMAIN_SERIALIZED_MAX_SIZE);

  InstrumentDomain *cs = instrument_domain_deserialize(blob);
  assert_null(cs);

  free(blob);
  instrument_domain_free(cs);
}

static void test_serialization_is_string_safe(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_create(VALID_MIN, VALID_MAX);

  char *blob = instrument_domain_serialize(cs);
  assert_non_null(blob);

  size_t len = strlen(blob);

  assert_true(len > 0);

  for (size_t i = 0; i < len; i++) {
    assert_true(blob[i] != '\0');
  }

  assert_int_equal(blob[len], '\0');

  assert_non_null(strchr(blob, '|'));
  char min_str[32];
  char max_str[32];
  snprintf(min_str, sizeof(min_str), "%lf", VALID_MIN);
  snprintf(max_str, sizeof(max_str), "%lf", VALID_MAX);

  assert_non_null(strstr(blob, min_str));
  assert_non_null(strstr(blob, max_str));

  free(blob);
  instrument_domain_free(cs);
}

static void test_deserialize_invalid_format(void **state) {
  (void)state;

  const char *bad = "not|enough";

  InstrumentDomain *cs = instrument_domain_deserialize(bad);

  assert_null(cs);
}
static void test_deserialize_empty_string(void **state) {
  (void)state;

  InstrumentDomain *cs = instrument_domain_deserialize("");

  assert_null(cs);
}

/* =========================
   MAIN
   ========================= */

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_create_valid),
      cmocka_unit_test(test_create_null_inputs),
      cmocka_unit_test(test_free_null),

      cmocka_unit_test(test_getters_valid),
      cmocka_unit_test(test_getters_null_stack),

      cmocka_unit_test(test_serialize_valid),
      cmocka_unit_test(test_serialize_null),

      cmocka_unit_test(test_deserialize_valid),
      cmocka_unit_test(test_deserialize_null),

      cmocka_unit_test(test_roundtrip),
      cmocka_unit_test(test_deserialize_corrupted),
      cmocka_unit_test(test_deserialize_corrupted),
      cmocka_unit_test(test_deserialize_invalid_format),
      cmocka_unit_test(test_serialization_is_string_safe),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
