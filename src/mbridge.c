#include "main.h"

static MRUBYVALUE give_cash(LPMRBSTATE mrb, MRUBYVALUE self)
{
   mrb_value money;
   mrb_get_args(mrb, "i", &money);
   int m = mrb_fixnum(money);
   gpGlobals->dwCash += m;
   return self;
}

static MRUBYVALUE set_exp_multiplier(LPMRBSTATE mrb, MRUBYVALUE self)
{
   mrb_value em;
   mrb_get_args(mrb, "i", &em);
   int m = mrb_fixnum(em);
   gpGlobals->nExpMultiplier = abs(m);
   return self;
}

static MRUBYVALUE lock_team(LPMRBSTATE mrb, MRUBYVALUE self) {
   gpGlobals->fLockTeamMember = TRUE;
   return self;
}

static MRUBYVALUE unlock_team(LPMRBSTATE mrb, MRUBYVALUE self) {
   gpGlobals->fLockTeamMember = FALSE;
   return self;
}

static MRUBYVALUE add_inventory(LPMRBSTATE mrb, MRUBYVALUE self) {
   mrb_value inventory_id;
   mrb_value count;
   mrb_get_args(mrb, "ii", &inventory_id, &count);
   int id = mrb_fixnum(inventory_id);
   int cn = mrb_fixnum(count);
   PAL_AddItemToInventory(id, cn);
   return self;
}

static MRUBYVALUE make_fantastic4(LPMRBSTATE mrb, MRUBYVALUE self)
{
   //
   // Set the player party
   //
   gpGlobals->wMaxPartyMemberIndex = 3;
   gpGlobals->rgParty[0].wPlayerRole = 0; // => XY
   gpGlobals->rgParty[1].wPlayerRole = 1; // => 02
   gpGlobals->rgParty[2].wPlayerRole = 4; // => ANU
   gpGlobals->rgParty[3].wPlayerRole = 2; // => Miss
   g_Battle.rgPlayer[0].action.ActionType = kBattleActionAttack;
   g_Battle.rgPlayer[1].action.ActionType = kBattleActionAttack;
   g_Battle.rgPlayer[2].action.ActionType = kBattleActionAttack;
   g_Battle.rgPlayer[3].action.ActionType = kBattleActionAttack;

   if (gpGlobals->wMaxPartyMemberIndex == 0)
   {
      // HACK for Dream 2.11
      gpGlobals->rgParty[0].wPlayerRole = 0;
      gpGlobals->wMaxPartyMemberIndex = 1;
   }

   //
   // Reload the player sprites
   //
   PAL_SetLoadFlags(kLoadPlayerSprite);
   PAL_LoadResources();

   memset(gpGlobals->rgPoisonStatus, 0, sizeof(gpGlobals->rgPoisonStatus));
   PAL_UpdateEquipments();
   return self;
}

VOID PAL_InitMRubyHandlers()
{
   LPMRUBYCLASS sdlpal_module = mrb_define_module(gpGlobals->mrb, "Sdlpal");
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "give_cash", give_cash, MRB_ARGS_REQ(1));
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "set_exp_multiplier", set_exp_multiplier, MRB_ARGS_REQ(1));
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "make_team", make_fantastic4, MRB_ARGS_NONE());
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "lock_team", lock_team, MRB_ARGS_NONE());
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "unlock_team", unlock_team, MRB_ARGS_NONE());
   mrb_define_module_function(gpGlobals->mrb, sdlpal_module, "add_item", add_inventory, MRB_ARGS_REQ(2));
}

WORD PAL_ExecuteMRubyScript(WORD wScriptID)
{
   if (gpGlobals->mrb == NULL)
   {
      return -1;
   }
   FILE *file = fopen(va("%s%d%s", PAL_SAVE_PREFIX, wScriptID, ".rb"), "r");
   if (file == NULL)
   {
      return -1;
   }
   mrb_load_file(gpGlobals->mrb, file);
   if (gpGlobals->mrb->exc)
   {
      mrb_value bt = mrb_get_backtrace(gpGlobals->mrb);
      mrb_value exc = mrb_funcall(gpGlobals->mrb, mrb_obj_value(gpGlobals->mrb->exc), "inspect", 0);
      if (mrb_string_p(exc))
      {
         puts(va("%s %s\n", RSTRING_PTR(exc), RSTRING_PTR(bt)));
      }
   }
   fclose(file);
   return 0;
}