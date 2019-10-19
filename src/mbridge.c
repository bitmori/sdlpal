#include "main.h"

MRUBYVALUE
give_cash(mrb_state* mrb, mrb_value self)
{
   mrb_value money;
   mrb_get_args(mrb, "i", &money);
   int m = mrb_fixnum(money);
   gpGlobals->dwCash += m;
   return self;
}



VOID
PAL_InitMRubyHandlers() {
   LPMRUBYCLASS sdlpal_module = mrb_define_module(gpGlobals->mrb, "Sdlpal");
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "give_cash", give_cash, MRB_ARGS_REQ(1));
}

WORD
PAL_ExecuteMRubyScript(
   WORD wScriptID
) {
   if (gpGlobals->mrb == NULL) {
      return -1;
   }
   FILE *file = fopen(va("%s%d%s", PAL_SAVE_PREFIX, wScriptID, ".rb"), "r");
   if (file == NULL) {
      return -1;
   }
   mrb_load_file(gpGlobals->mrb, file);
   if (gpGlobals->mrb->exc) {
      mrb_value bt  = mrb_get_backtrace(gpGlobals->mrb);
      mrb_value exc = mrb_funcall(gpGlobals->mrb, mrb_obj_value(gpGlobals->mrb->exc), "inspect", 0);
      if (mrb_string_p(exc)) {
         puts(va("%s %s\n", RSTRING_PTR(exc), RSTRING_PTR(bt)));
      }
   }
   fclose(file);
   return 0;
}