function patch() {
CAPI.WASM_change_magic_efx(0x50, -1, -1, -1, -1, 1<<0); // 借尸还魂
CAPI.WASM_change_magic_data(0x37, 1, -1, -1);// 御剑伏魔 - 梦幻
CAPI.WASM_change_magic_efx(0x37, -1, -1, -1, -1, 1<<2); // 御剑伏魔 - 梦幻
CAPI.WASM_change_magic_efx(0x65, -1, -1, -1, -1, 1<<2); // 大咒蛇 - 梦幻
CAPI.WASM_change_magic_efx(0x4A, -1, -1, -1, -1, 1<<2 | 1<<4); // 魔焰焚 - 梦幻
CAPI.WASM_change_magic_efx(0x68, -1, -1, -1, -1, 1<<2); // 万里狂杀前序
CAPI.WASM_change_magic_efx(0x3c, -1, -1, -1, -1, 1<<2);// 逍遥神剑本体效果
// 真元护体 - 66MP，全体3回合金刚，回梦特效
CAPI.WASM_change_obj(313, 6, 2 | 1<<4);
CAPI.WASM_edit_script(0xA440, 0x2D, 6, 3, 1);
CAPI.WASM_change_magic_data(0x28, 5, 66, 0);
CAPI.WASM_change_magic_efx(0x28, 0x28, 0, 4, -1, 1<<0);

// 五气 - 75MP，全体555HP，凝神特效
CAPI.WASM_edit_script(0x9A8F, 0x1B, 1, 555, 0);
CAPI.WASM_change_magic_data(0x2E, 5, 75, 0);
CAPI.WASM_change_magic_efx(0x2E, 0x1D, 0xfff8, 4, 0x8B, 0);

// 忘剑五诀修正
CAPI.WASM_edit_script(0x2b87, 0x2b, 0, 581, 0);
CAPI.WASM_edit_script(0x2b88, 0xFFFF, 0x25E8, 0, 0);
CAPI.WASM_edit_script(0x2b89, 0xFFFF, 0x25E9, 0, 0);

// 怪物重生
CAPI.WASM_edit_script(0x9CCF, 0x52, 0, 0, 0);

// 毒烈度都降低到98
// 禁止隐身
CAPI.WASM_change_obj(577, 0, 98);
// 一忘皆空
CAPI.WASM_change_obj(581, 0, 98);
// 圣灵祝福中
CAPI.WASM_change_obj(582, 0, 98);
// 无心
CAPI.WASM_change_obj(583, 0, 98);
// 寂灭天下
CAPI.WASM_change_obj(587, 0, 98);

// 怪物不再出现
// CAPI.WASM_edit_script(0x9CCF, 0x49, 0xFFFF, 0, 0);
alert("补丁成功！");
}
