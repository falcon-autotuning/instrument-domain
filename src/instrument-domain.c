#include "instrument-domain/instrument-domain.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct InstrumentDomain {
  double min;
  double max;
};

static void copy_string(char *dest, const char *src) {
  if (!dest)
    return;

  if (!src) {
    dest[0] = '\0';
    return;
  }

  strncpy(dest, src, INST_DOMAIN_MAX_STRING_LEN - 1);
  dest[INST_DOMAIN_MAX_STRING_LEN - 1] = '\0';
}

/* =========================
   Public API
   ========================= */

InstrumentDomain *instrument_domain_create(double min, double max) {
  InstrumentDomain *domain =
      (InstrumentDomain *)malloc(sizeof(InstrumentDomain));
  if (!domain) {
    return NULL;
  }

  domain->min = min;
  domain->max = max;

  return domain;
}

void instrument_domain_free(InstrumentDomain *domain) {
  if (domain) {
    free(domain);
  }
}

double instrument_domain_get_min(const InstrumentDomain *domain) {
  if (!domain)
    return 0.0;
  return domain->min;
}

double instrument_domain_get_max(const InstrumentDomain *domain) {
  if (!domain)
    return 0.0;
  return domain->max;
}

char *instrument_domain_serialize(const InstrumentDomain *domain) {
  if (!domain)
    return NULL;

  char *buffer = (char *)malloc(INST_DOMAIN_SERIALIZED_MAX_SIZE);
  if (!buffer)
    return NULL;

  snprintf(buffer, INST_DOMAIN_SERIALIZED_MAX_SIZE, "%lf|%lf", domain->min,
           domain->max);

  return buffer;
}

InstrumentDomain *instrument_domain_deserialize(const char *buffer) {
  if (!buffer)
    return NULL;

  InstrumentDomain *domain =
      (InstrumentDomain *)malloc(sizeof(InstrumentDomain));
  if (!domain)
    return NULL;

  memset(domain, 0, sizeof(InstrumentDomain));
  double min;
  double max;

  int parsed = sscanf(buffer, "%lf|%lf", &min, &max);

  if (parsed != 2) {
    free(domain);
    return NULL;
  }
  domain->min = min;
  domain->max = max;

  return domain;
}

InstrumentDomain *instrument_domain_clone(const InstrumentDomain *domain) {
  if (!domain)
    return NULL;

  return instrument_domain_create(instrument_domain_get_min(domain),
                                  instrument_domain_get_max(domain));
}
