#include "mpdecimal.h"
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <stdio.h>
#include <stdlib.h>

static const ssize_t PRECISION = 64;

struct decimal_t {
  mpd_context_t *context;
  mpd_t *decimal;
};

static void decimal_free(mrb_state *state, void *data) {
  if (data == NULL) {
    return;
  }

  struct decimal_t *decimal = data;
  mpd_context_t *ctx = decimal->context;
  mpd_del(ctx, decimal->decimal);
  mrb_free(state, decimal);
  mrb_free(state, ctx);
}

static const struct mrb_data_type DECIMAL_DATA_TYPE = { "Mpd", decimal_free };

static mrb_value wrap_decimal(mrb_state *state, struct RClass *klass, mpd_t *decimal) {
  struct decimal_t *wrapper = mrb_malloc(state, sizeof(struct decimal_t));
  wrapper->context = new_context(state);
  wrapper->decimal = decimal;
  struct RData *result = mrb_data_object_alloc(
    state,
    klass,
    wrapper,
    &DECIMAL_DATA_TYPE);
  return mrb_obj_value(result);
}

static struct decimal_t *unwrap_decimal(mrb_state *state, mrb_value rdecimal) {
  return mrb_data_get_ptr(state, rdecimal, &DECIMAL_DATA_TYPE);
}

static struct decimal_t *decimal_from_value(mrb_state *state, mrb_value rvalue) {
  rvalue = mrb_convert_type(state, rvalue, MRB_TT_DATA, "Decimal", "to_d");
  return unwrap_decimal(state, rvalue);
}

static uint32_t IGNORED_CONDITIONS = MPD_Inexact | MPD_Rounded;
static mrb_int HEX_BASE = 16;

static void check_status(mrb_state *state, uint32_t status) {
  status &= ~IGNORED_CONDITIONS;

  if (status) {
    mrb_value code = mrb_funcall(state, mrb_fixnum_value(status), "to_s", 1, mrb_fixnum_value(HEX_BASE));
    mrb_raisef(state, mrb_class_get(state, "RuntimeError"), "bad decimal operation (0x%S)", code);
  }
}

static void *malloc_adaptor(void *data, size_t size) {
  return mrb_malloc(data, size);
}

static void *calloc_adaptor(void *data, size_t count, size_t size) {
  return mrb_calloc(data, count, size);
}

static void *realloc_adaptor(void *data, void *mem, size_t size) {
  return mrb_realloc(data, mem, size);
}

static void free_adaptor(void *data, void *mem) {
  return mrb_free(data, mem);
}

static mpd_context_t* new_context(mrb_state *state) {
  mpd_context_t *context = mrb_malloc(state, sizeof(mpd_context_t));
  mpd_allocator_t allocator = {
      .mallocfunc = malloc_adaptor,
      .callocfunc = calloc_adaptor,
      .reallocfunc = realloc_adaptor,
      .freefunc = free_adaptor,
      .data = state,
  };
  mpd_init(context, PRECISION, allocator);
  return context;
}

static mrb_value ext_decimal_initialize(mrb_state *state, mrb_value self) {
  mrb_value value = mrb_fixnum_value(0);
  mrb_get_args(state, "|o", &value);

  if (mrb_obj_equal(state, self, value)) {
    return self;
  }

  mpd_context_t *context = new_context(state);

  struct decimal_t *decimal = mrb_malloc(state, sizeof(struct decimal_t));
  decimal->context = context;
  decimal->decimal = mpd_qnew(context);
  mrb_data_init(self, decimal, &DECIMAL_DATA_TYPE);

  uint32_t status = 0;
  if (mrb_fixnum_p(value)) {
    mpd_qset_i64(decimal->decimal, mrb_fixnum(value), context, &status);
  } else if (mrb_string_p(value)) {
    mpd_qset_string(decimal->decimal, mrb_str_to_cstr(state, value), context, &status);
  } else {
    mrb_value converted_value = mrb_convert_type(state, value, MRB_TT_STRING, "String", "to_s");
    mpd_qset_string(decimal->decimal, mrb_str_to_cstr(state, converted_value), context, &status);
  }
  if (status & MPD_Conversion_syntax) {
    mrb_raisef(state, mrb_class_get(state, "ArgumentError"), "can't convert %S into Decimal", mrb_inspect(state, value));
  }
  check_status(state, status);

  return self;
}

typedef void (*unary_op_t)(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
typedef void (*binary_op_t)(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);

static mrb_value ext_decimal_unary_op(mrb_state *state, mrb_value rself, unary_op_t op) {
  struct decimal_t *self = unwrap_decimal(state, rself);
  struct mpd_t *result = mpd_qnew(self->context);

  uint32_t status = 0;
  op(result, self->decimal, self->context, &status);
  mrb_value rresult = wrap_decimal(state, mrb_class(state, rself), result);
  check_status(state, status);

  return rresult;
}

static mrb_value ext_decimal_bin_op(mrb_state *state, mrb_value rself, binary_op_t op) {
  mrb_value rother;
  mrb_get_args(state, "o", &rother);

  struct decimal_t *self = unwrap_decimal(state, rself);
  struct decimal_t *other = decimal_from_value(state, rother);
  struct mpd_t *result = mpd_qnew(self->context);

  uint32_t status = 0;
  op(result, self->decimal, other->decimal, self->context, &status);
  mrb_value rresult = wrap_decimal(state, mrb_class(state, rself), result);
  check_status(state, status);

  return rresult;
}

static mrb_value ext_decimal_add(mrb_state *state, mrb_value rself) {
  return ext_decimal_bin_op(state, rself, mpd_qadd);
}

static mrb_value ext_decimal_sub(mrb_state *state, mrb_value rself) {
  return ext_decimal_bin_op(state, rself, mpd_qsub);
}

static mrb_value ext_decimal_mul(mrb_state *state, mrb_value rself) {
  return ext_decimal_bin_op(state, rself, mpd_qmul);
}

static mrb_value ext_decimal_div(mrb_state *state, mrb_value rself) {
  return ext_decimal_bin_op(state, rself, mpd_qdiv);
}

static mrb_value ext_decimal_negate(mrb_state *state, mrb_value rself) {
  return ext_decimal_unary_op(state, rself, mpd_qminus);
}

static mrb_value ext_decimal_round(mrb_state *state, mrb_value rself) {
  return ext_decimal_unary_op(state, rself, mpd_qround_to_int);
}

static mrb_value ext_decimal_floor(mrb_state *state, mrb_value rself) {
  return ext_decimal_unary_op(state, rself, mpd_qfloor);
}

static mrb_value ext_decimal_ceil(mrb_state *state, mrb_value rself) {
  return ext_decimal_unary_op(state, rself, mpd_qceil);
}

static mrb_value ext_decimal_spaceship(mrb_state *state, mrb_value rself) {
  mrb_value rother;
  mrb_get_args(state, "o", &rother);

  struct decimal_t *self = unwrap_decimal(state, rself);
  struct decimal_t *other = decimal_from_value(state, rother);

  uint32_t status = 0;
  int result = mpd_qcmp(self->decimal, other->decimal, &status);
  check_status(state, status);

  return mrb_fixnum_value(result);
}

static mrb_value ext_decimal_eql_p(mrb_state *state, mrb_value rself) {
  mrb_value rother;
  mrb_get_args(state, "o", &rother);

  struct decimal_t *self = unwrap_decimal(state, rself);
  struct decimal_t *other = mrb_data_check_get_ptr(state, rother, &DECIMAL_DATA_TYPE);
  if (other == NULL) {
    return mrb_false_value();
  }

  uint32_t status = 0;
  int result = mpd_qcmp(self->decimal, other->decimal, &status);
  check_status(state, status);

  if (result != 0) {
    return mrb_false_value();
  }
  return mrb_true_value();
}

static mrb_value ext_decimal_hash(mrb_state *state, mrb_value rself) {
  struct decimal_t *self = unwrap_decimal(state, rself);
  struct mpd_t *reduced = mpd_qnew(self->context);

  uint32_t status = 0;
  mpd_qreduce(reduced, self->decimal, self->context, &status);
  check_status(state, status);

  mpd_uint_t key = reduced->exp;
  for (mpd_ssize_t i = 0; i < reduced->len; ++i) {
    key = key * 65599 + reduced->data[i];
  }

  mpd_del(self->context, reduced);
  return mrb_fixnum_value(key + (key >> 5));
}

static mrb_value ext_decimal_to_d(mrb_state *state, mrb_value rself) {
  (void)state;
  return rself;
}

static mrb_value ext_decimal_to_s(mrb_state *state, mrb_value rself) {
  struct decimal_t *self = unwrap_decimal(state, rself);

  uint32_t status = 0;
  char *s = mpd_qformat(self->decimal, "f", self->context, &status);
  check_status(state, status);

  mrb_value result = mrb_str_new_cstr(state, s);
  mpd_free(self->context, s);
  return result;
}

void mrb_mruby_mpdecimal_gem_init(mrb_state *state) {
  struct RClass *c_decimal = mrb_define_class(state, "Decimal", state->object_class);
  MRB_SET_INSTANCE_TT(c_decimal, MRB_TT_DATA);

  mrb_define_method(state, c_decimal, "initialize", ext_decimal_initialize, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "+", ext_decimal_add, MRB_ARGS_REQ(1));
  mrb_define_method(state, c_decimal, "-", ext_decimal_sub, MRB_ARGS_REQ(1));
  mrb_define_method(state, c_decimal, "*", ext_decimal_mul, MRB_ARGS_REQ(1));
  mrb_define_method(state, c_decimal, "/", ext_decimal_div, MRB_ARGS_REQ(1));
  mrb_define_method(state, c_decimal, "-@", ext_decimal_negate, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "round", ext_decimal_round, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "floor", ext_decimal_floor, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "ceil", ext_decimal_ceil, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "<=>", ext_decimal_spaceship, MRB_ARGS_REQ(1));
  mrb_define_method(state, c_decimal, "eql?", ext_decimal_eql_p, MRB_ARGS_REQ(1));
  mrb_define_method(state, c_decimal, "hash", ext_decimal_hash, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "to_d", ext_decimal_to_d, MRB_ARGS_NONE());
  mrb_define_method(state, c_decimal, "to_s", ext_decimal_to_s, MRB_ARGS_NONE());

  mrb_define_const(state, c_decimal, "PRECISION", mrb_fixnum_value(PRECISION));
}

void mrb_mruby_mpdecimal_gem_final(mrb_state *state) {
}
