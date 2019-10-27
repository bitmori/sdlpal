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

static MRUBYVALUE lock_team(LPMRBSTATE mrb, MRUBYVALUE self)
{
   gpGlobals->fLockTeamMember = TRUE;
   return self;
}

static MRUBYVALUE unlock_team(LPMRBSTATE mrb, MRUBYVALUE self)
{
   gpGlobals->fLockTeamMember = FALSE;
   return self;
}

static MRUBYVALUE add_inventory(LPMRBSTATE mrb, MRUBYVALUE self)
{
   mrb_value inventory_id;
   mrb_value count;
   mrb_get_args(mrb, "ii", &inventory_id, &count);
   int id = mrb_fixnum(inventory_id);
   int cn = mrb_fixnum(count);
   PAL_AddItemToInventory(id, cn);
   return self;
}

static MRUBYVALUE add_magic(LPMRBSTATE mrb, MRUBYVALUE self) {
   mrb_value player_role;
   mrb_value magic_id;
   mrb_get_args(mrb, "ii", &player_role, &magic_id);
   int role = mrb_fixnum(player_role);
   int id = mrb_fixnum(magic_id);
   PAL_AddMagic(role, id);
   return self;
}

static MRUBYVALUE del_magic(LPMRBSTATE mrb, MRUBYVALUE self) {
   mrb_value player_role;
   mrb_value magic_id;
   mrb_get_args(mrb, "ii", &player_role, &magic_id);
   int role = mrb_fixnum(player_role);
   int id = mrb_fixnum(magic_id);
   PAL_RemoveMagic(role, id);
   return self;
}

static MRUBYVALUE pp_magic(LPMRBSTATE mrb, MRUBYVALUE self) {
   mrb_value role;
   mrb_get_args(mrb, "i", &role);
   int r = mrb_fixnum(role);
   puts(va("Player %d has magic: ", r));
   for (int i = 0; i < MAX_PLAYER_MAGICS; i++)
   {
      if (gpGlobals->g.PlayerRoles.rgwMagic[i][r] != 0)
      {
         int obj_id = gpGlobals->g.PlayerRoles.rgwMagic[i][r];
         int magic_num = gpGlobals->g.rgObject[obj_id].magic.wMagicNumber;
         puts(va(" - %d -> 0x%x @ word.dat -> meta_id = %d", obj_id, obj_id * 10, magic_num));
      }
   }
   return self;
}

static MRUBYVALUE pi_magic(LPMRBSTATE mrb, MRUBYVALUE self) {
   mrb_value _id;
   mrb_get_args(mrb, "i", &_id);
   int obj_id = mrb_fixnum(_id);
   int magic_num = gpGlobals->g.rgObject[obj_id].magic.wMagicNumber;
   puts(va("Magic %d -> meta_id = %d", obj_id, magic_num));
   return self;
}

// static MRUBYVALUE pall_magic(LPMRBSTATE mrb, MRUBYVALUE self) {
//    int magic_num = gpGlobals->g.rgObject[obj_id].magic.wMagicNumber;
//    puts(va("Magic %d -> meta_id = %d", obj_id, magic_num));
//    return self;
// }


static WORD array_value_at(MRUBYVALUE arr, int64_t pos) {
   mrb_value val = mrb_ary_entry(arr, pos);
   if (mrb_nil_p(val) || !mrb_fixnum_p(val)) {
      return 0;
   } else {
      return mrb_fixnum(val);
   }
}

static MRUBYVALUE add_meta_magic(LPMRBSTATE mrb, MRUBYVALUE self) {
   mrb_value _id;
   mrb_value _data;
   mrb_get_args(mrb, "iA", &_id, &_data);
   int obj_id = mrb_fixnum(_id);
   WORD cost = array_value_at(_data, 0);
   int magic_num = gpGlobals->g.rgObject[obj_id].magic.wMagicNumber;
   gpGlobals->g.lprgMagic[magic_num].wCostMP = 3;
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

static VOID report() {
   LPMRBSTATE mrb = gpGlobals->mrb;
   if (mrb->exc) {
      mrb_value bt = mrb_get_backtrace(mrb);
      mrb_value exc = mrb_funcall(gpGlobals->mrb, mrb_obj_value(gpGlobals->mrb->exc), "inspect", 0);
      if (mrb_string_p(exc))
      {
         puts(va("%s %s\n", RSTRING_PTR(exc), RSTRING_PTR(bt)));
      }
   }
}

VOID PAL_InitMRubyHandlers()
{
   LPMRBSTATE mrb = gpGlobals->mrb;
   LPMRUBYCLASS sdlpal_module = mrb_define_module(mrb, "Sdlpal");
   mrb_define_module_function(mrb, sdlpal_module, "give_cash", give_cash, MRB_ARGS_REQ(1));
   mrb_define_module_function(mrb, sdlpal_module, "set_exp_multiplier", set_exp_multiplier, MRB_ARGS_REQ(1));
   mrb_define_module_function(mrb, sdlpal_module, "make_team", make_fantastic4, MRB_ARGS_NONE());
   mrb_define_module_function(mrb, sdlpal_module, "lock_team", lock_team, MRB_ARGS_NONE());
   mrb_define_module_function(mrb, sdlpal_module, "unlock_team", unlock_team, MRB_ARGS_NONE());
   mrb_define_module_function(mrb, sdlpal_module, "add_item", add_inventory, MRB_ARGS_REQ(2));

   mrb_define_module_function(mrb, sdlpal_module, "add_magic", add_magic, MRB_ARGS_REQ(2));
   mrb_define_module_function(mrb, sdlpal_module, "del_magic", del_magic, MRB_ARGS_REQ(2));
   mrb_define_module_function(mrb, sdlpal_module, "pp_magic", pp_magic, MRB_ARGS_REQ(1));
   mrb_define_module_function(mrb, sdlpal_module, "pi_magic", pi_magic, MRB_ARGS_REQ(1));
}

WORD PAL_ExecuteMRubyScript(WORD wScriptID)
{
   LPMRBSTATE mrb = gpGlobals->mrb;
   mrb_funcall(mrb, mrb_top_self(mrb), va("redstone_scroll_%d", wScriptID), 0);
   report();
   return 0;
}

WORD PAL_InitMRubyCallbacks()
{
   LPMRBSTATE mrb = gpGlobals->mrb;
   if (mrb == NULL)
   {
      return -1;
   }
   FILE *file = fopen(va("%s%s%s", PAL_PREFIX, "sdlpal", ".rb"), "r");
   if (file == NULL) {
      return -1;
   }
   mrb_load_file(mrb, file);
   report();
   fclose(file);
   return 0;
}

typedef toml_table_t * LPTOMLTABLE;
typedef toml_array_t * LPTOMLARRAY;

static WORD PAL_InitExtraMagicData(LPTOMLTABLE data) {
   
   return 0;
}

static WORD PAL_ExtendStoreData(LPTOMLTABLE data) {
   LPTOMLARRAY stores = toml_array_in(data, "stores");
   if (stores == NULL) {
      return -1;
   }
   gpGlobals->ex.nStore = toml_array_nelem(stores);
   gpGlobals->g.lprgStore = (LPSTORE)realloc(gpGlobals->g.lprgStore, gpGlobals->ex.nStore + gpGlobals->g.nStore);
   for (INT i = 0; i < gpGlobals->ex.nStore; i++) {
      LPTOMLARRAY st = toml_array_at(stores, i);
      INT nelem = toml_array_nelem(st);
      nelem = __MIN(nelem, MAX_STORE_ITEM);
      for (INT j = 0; j < nelem; j++) {
         toml_rtoi(toml_raw_at(st, j), &(gpGlobals->g.lprgStore[i + gpGlobals->g.nStore].rgwItems[j]));
      }
   }
   return 0;
}

static WORD PAL_LoadExtraDialog() {
   return 0;
}



WORD PAL_InitExtraGameData() {
   FILE * fp = UTIL_OpenFile("sdlpal.toml");
   if (fp == NULL)
   {
      return -1;
   }
   char errbuf[0xFF];
   LPTOMLTABLE data = toml_parse_file(fp, errbuf, sizeof(errbuf));
   if (data == NULL) {
      return -1;
   }
   // PAL_InitExtraMagicData(data);
   PAL_ExtendStoreData(data);
   toml_free(data);
   fclose(fp);
   return PAL_LoadExtraDialog();
}

VOID PAL_FreeExtraGameData() {
   // TODO finish this part.
}