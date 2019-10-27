#ifndef __MBRIDGE_H__
#define __MBRIDGE_H__

#include "common.h"
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/error.h"
#include "mruby/string.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "toml/toml.h"

typedef mrb_state *LPMRBSTATE;
typedef mrb_value MRUBYVALUE;
typedef struct RClass *LPMRUBYCLASS;

WORD PAL_ExecuteMRubyScript(WORD wScriptID);

VOID PAL_InitMRubyHandlers();

WORD PAL_InitMRubyCallbacks();

WORD PAL_InitExtraGameData();

VOID PAL_FreeExtraGameData();
#endif