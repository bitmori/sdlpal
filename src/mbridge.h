#ifndef __MBRIDGE_H__
#define __MBRIDGE_H__

#include "common.h"
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/error.h"
#include "mruby/string.h"

typedef mrb_state *LPMRBSTATE;
typedef mrb_value MRUBYVALUE;
typedef struct RClass *LPMRUBYCLASS;


MRUBYVALUE
give_cash(mrb_state* mrb, mrb_value self);

WORD
PAL_ExecuteMRubyScript(
   WORD          wScriptID
);

VOID
PAL_InitMRubyHandlers();
#endif