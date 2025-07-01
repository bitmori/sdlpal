var strSyncingFs = 'Syncing FS...';
var strDone = 'Done.';
var strDeleting = 'Deleting...';
var strNoSave = 'Cannot find saved games to download';
var strNoData = 'Error: Game data not loaded!';
var strInit = 'Initializing...';
var strLoading = 'Loading';
var strDelConfirm = "This will DELETE your game data and saved games stored in browser cache. Type 'YES' to continue.";
var strTips = "Note: Avoid using uppercase letters in the configuration file when specifying paths and file names.";

var userLang = navigator.language || navigator.userLanguage;
if (userLang === 'zh-CN' || userLang.startsWith('zh-Hans') ) {
    strSyncingFs = '正在同步文件系统...';
    strDone = '完成。';
    strDeleting = '正在删除...';
    strNoSave = '无法找到可下载的游戏存档！';
    strNoData = '错误：游戏数据未上传。请先上传ZIP格式的游戏数据文件。';
    strInit = '正在初始化...';
    strLoading = '正在加载';
    strDelConfirm = '此操作将删除您浏览器缓存中保存的数据文件及存档。请输入 "YES" 继续：';
    strTips = "注意！配置文件内的路径和文件名 不能 包含大写字母。";
} else if (userLang === 'zh-TW' || userLang.startsWith('zh-Hant') ) {
    strSyncingFs = '正在同步檔案系統...';
    strDone = '完成。';
    strDeleting = '正在刪除...';
    strNoSave = '無法找到可下載的遊戲記錄！';
    strNoData = '錯誤：遊戲資料檔未上傳。請先上傳ZIP格式的遊戲資料檔。';
    strInit = '正在初始化...';
    strLoading = '正在加載';
    strDelConfirm = '此操作將刪除您瀏覽器緩存中保存的遊戲資料檔及記錄。請輸入 "YES" 繼續：';
    strTips = "請注意：在設定檔中指定路徑和檔案名稱時，請勿使用大寫字母。";
}


var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');
var tipsElement;
window.addEventListener('load', function () {
    tipsElement = document.getElementById('tips');
    tipsElement.textContent = strTips;
})

var Module = {
    preRun: [],
    postRun: [],
    print: function(text) {
        console.log(text);
    },
    printErr: function(text) {
        console.error(text);
    },
    canvas: (function() {
        var canvas = document.getElementById('canvas');

        // As a default initial behavior, pop up an alert when webgl context is lost. To make your
        // application robust, you may want to override this behavior before shipping!
        // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
        canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

        return canvas;
    })(),
    setStatus: function(text) {
        if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
        if (text === Module.setStatus.last.text) return;
	if (text === '' && Module.setStatus.last.text == strSyncingFs) return;
        var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
        var now = Date.now();
        if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
        Module.setStatus.last.time = now;
        Module.setStatus.last.text = text;
        if (m) {
            text = m[1];
            progressElement.value = parseInt(m[2])*100;
            progressElement.max = parseInt(m[4])*100;
            progressElement.hidden = false;
            spinnerElement.hidden = false;
        } else {
            progressElement.value = null;
            progressElement.max = null;
            progressElement.hidden = true;
            if (!text) spinnerElement.style.display = 'none';
        }
        statusElement.innerHTML = text;
    },
    totalDependencies: 0,
    monitorRunDependencies: function(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    },
    onRuntimeInitialized:function() {
        onRuntimeInitialized();
    }
};

function registerPALFunctions() {
    const functions = [
        ['WASM_give_cash', null, ['number']],
        ['WASM_remove_team_member', null, ['number', 'number']],
        ['WASM_append_team_member', null, ['number']],
        ['WASM_change_team_member_sprite', null, ['number', 'number']],
        ['WASM_learn_magic', null, ['number', 'number']],
        ['WASM_forget_magic', null, ['number', 'number']],
        ['WASM_change_obj', null, ['number', 'number', 'number']],
        ['WASM_query_obj', null, ['number']],
        ['WASM_change_magic_data', null, ['number', 'number', 'number','number']],
        ['WASM_change_magic_efx', null, ['number', 'number', 'number','number', 'number', 'number']],
        
        ['WASM_query_magic_data', null, ['number']],
        
        ['WASM_give_item', null, ['number', 'number']],
        ['WASM_lock_team', 'number', []],

        ['WASM_set_log_level', null, ['number']],
        ['WASM_query_script', null, ['number']],
        ['WASM_edit_script', null, ['number', 'number', 'number', 'number', 'number']],

        ['WASM_dump_sss', null, []],
        ['WASM_dump_magic_data', null, []],
        ['WASM_query_event_obj', null, ['number']],
    ];

    window.CAPI = {}; // 导出到全局变量

    for (const [name, returnType, argTypes] of functions) {
        CAPI[name] = Module.cwrap(name, returnType, argTypes);
    }
}

function onRuntimeInitialized() {
    try {
	FS.mkdir('/data');
    } catch (e) {
    }
    FS.mount(IDBFS, {}, '/data');
    Module.setStatus(strSyncingFs);
    spinnerElement.style.display = 'inline-block';
    FS.syncfs(true, function (err) {
	spinnerElement.style.display = 'none';
        Module.setStatus(strDone);
    });
    registerPALFunctions();
}

function loadZip() {
    var fileBtn = document.getElementById('btnLoadZip');
    Module.setStatus(strLoading + ' ' + fileBtn.files[0].name + '...');
    spinnerElement.style.display = 'inline-block';

    var fileInput = document.getElementById("btnLoadZip");
    var zip = new JSZip();
    var file = fileInput.files[0];

    zip.loadAsync(file).then(function(z) {
        var promises = [];
	z.forEach(function(relativePath, zipEntry) {
        if (relativePath.includes('._')) {
            Module.print("ignoring file "+relativePath);
            return;
        }
	    if (zipEntry.dir) {
		var pathArr = relativePath.split('/');
		var currPath = '/data';
		for (var i = 0; i < pathArr.length; i++) {
		    currPath += '/';
		    currPath += pathArr[i];
		    try { 
			FS.mkdir(currPath.toLowerCase(), 0777);
		    } catch (e) {
		    }
		}
	    } else {
		promises.push(zipEntry.async('uint8array').then(function(arr) {
		    FS.writeFile('/data/' + relativePath.toLowerCase(), arr, {encoding: 'binary'});
		}));
        }
    });
    Promise.all(promises).then(function() {
	Module.setStatus(strSyncingFs);
	FS.syncfs(function (err) {
	    Module.setStatus(strDone);
	    spinnerElement.style.display = 'none';
	});
    });
    });
}

function loadFile() {
    // 1. 获取文件输入元素和选择的文件
    var fileInput = document.getElementById('btnLoadFile'); // 注意：这里使用了一个新的ID
    if (!fileInput.files || fileInput.files.length === 0) {
        Module.print("错误：没有选择文件。");
        return;
    }
    var file = fileInput.files[0];

    // 2. 更新UI状态，显示正在加载
    Module.setStatus(strLoading + ' ' + file.name + '...');
    spinnerElement.style.display = 'inline-block';

    // 3. 使用FileReader或现代的file.arrayBuffer()异步读取文件内容
    // file.arrayBuffer() 返回一个Promise，非常适合这种异步流程
    file.arrayBuffer().then(function(buffer) {
        // 将ArrayBuffer转换为Uint8Array，这是FS.writeFile的最佳格式
        var data = new Uint8Array(buffer);

        // 4. 将文件写入Emscripten文件系统
        // FS.writeFile会覆盖任何已存在的同名文件
        var path = '/data/' + file.name.toLowerCase(); // 保持与loadZip一致，使用小写文件名
        FS.writeFile(path, data, { encoding: 'binary' });
        Module.print("文件 " + file.name + " 已写入到 " + path);

        // 5. 同步文件系统以持久化更改
        Module.setStatus(strSyncingFs);
        FS.syncfs(function(err) {
            // 操作完成后的回调
            if (err) {
                Module.setStatus('同步文件系统时出错。');
                Module.printErr('FS.syncfs error: ', err);
            } else {
                Module.setStatus(strDone);
            }
            spinnerElement.style.display = 'none';
        });

    }).catch(function(err) {
        // 如果文件读取失败，处理错误
        Module.setStatus('加载文件时出错。');
        Module.printErr('File reading error: ', err);
        spinnerElement.style.display = 'none';
    });
}

function clearData() {
    if (window.prompt(strDelConfirm) === "YES") {
        var doDelete = function(path) {
            Object.keys(FS.lookupPath(path).node.contents).forEach(element => {
                var stat = FS.stat(path + '/' + element);
                if (stat.mode & 0040000) {
                    doDelete(path + '/' + element);
                    FS.rmdir(path + '/' + element);
                } else {
                    FS.unlink(path + '/' + element);
                }
            });
        };
        Module.setStatus(strDeleting);
        spinnerElement.style.display = 'inline-block';
        doDelete('/data');
        Module.setStatus(strSyncingFs);
        FS.syncfs(false, function (err) {
            spinnerElement.style.display = 'none';
            Module.setStatus(strDone);
        });
    }
}

function downloadSaves() {
    var zip = new JSZip();
    var hasData = false;
    Object.keys(FS.lookupPath('/data').node.contents).forEach(element => {
        if (element.endsWith('.rpg') || element.endsWith('.cfg')) {
            var array = FS.readFile('/data/' + element);
            zip.file(element, array);
            hasData = true;
        }
    });
    if (!hasData) {
        window.alert(strNoSave);
        return;
    }
    zip.generateAsync({type:"base64"}).then(function (base64) {
        window.location = "data:application/zip;base64," + base64;
    }, function (err) {
        Module.printErr(err);
    });
}

async function runGame() {
    mainFunc = Module.cwrap('EMSCRIPTEN_main', 'number', ['number', 'number'], {async:true});
    mainFunc(0, 0);
}

function launch() {
    var checkFile = false;
    try {
	if (FS.stat('/data/fbp.mkf').size > 0) {
	    checkFile = true;
	}
    } catch (e) {
    }
    if (!checkFile) {
	Module.setStatus(strNoData);
	return;
    }
    document.getElementById('emscripten_logo').style = "display:none";
    document.getElementById('btnToggleTheme').style = "display:none";
    document.getElementById('btnLaunch').style = "display:none";
    document.getElementById('btnLoadZip').style = "display:none";
    document.getElementById('btnLoadFile').style = "display:none";
    document.getElementById('zip-label').style = "display:none";
    document.getElementById('file-label').style = "display:none";
    document.getElementById('btnDeleteData').style = "display:none";
    document.getElementById('btnGiveMoney').style = "display:flex";
    document.getElementById('btnAddTeamMember').style = "display:flex";
    document.getElementById('btnGiveItem').style = "display:flex";
    document.getElementById('btnLockTeam').style = "display:flex";
    document.getElementById('btnPatch').style = "display:flex";
    tipsElement.style = "display:none";
    runGame();
}

Module.setStatus(strInit);
window.onerror = function(event) {
    Module.setStatus('Exception thrown, see JavaScript console');
    spinnerElement.style.display = 'none';
    Module.setStatus = function(text) {
        if (text) Module.printErr('[post-exception status] ' + text);
    };
};

function giveMoney() {
    const userInput = prompt("输入需要增加的金钱", "1000");
    if (userInput === null) {
        return;
    }
    const cash = parseInt(userInput, 10);
    if (!isNaN(cash)) {
        CAPI.WASM_give_cash(cash);
    }
}

function giveItem() {
    const userInput = prompt("输入需要增加的道具编号", "1000");
    if (userInput === null) {
        return;
    }
    const obj_id = parseInt(userInput, 10);
    const userInput2 = prompt("输入想要的数量", "1");
    const count = parseInt(userInput2, 10);
    if (!isNaN(count)) {
        CAPI.WASM_give_item(obj_id, count);
    }
}

function lockTeam() {
    const locked = !!CAPI.WASM_lock_team();
    alert(locked ? "队伍锁定" : "队伍解锁");
}

function addTeamMember() {
    const userInput = prompt("输入需要增加的队友：逍遥0，灵儿1，月如2，巫后3，阿奴4", "4");
    if (userInput === null) {
        return;
    }
    const member = parseInt(userInput, 10);
    if (!isNaN(member) && member >= 0 && member <= 4) {
        CAPI.WASM_append_team_member(member);
    }
}

function patch() {
    // 额外修改在此处
}