var fakePwName = Memory.alloc(64);
fakePwName.writeUtf8String("test12345");
Interceptor.attach(Module.getGlobalExportByName('getpwuid'), {
    onLeave(retval) {
        if (retval.isNull()) return;
        console.log('getpwuid: return hooked, replacing passwd->pw_name with correct value...');
        retval.writePointer(fakePwName);
    }
});



Interceptor.attach(Module.getGlobalExportByName('memmove'), {
    onEnter: function(args) {
        //skip redundant memmoves...
        if(!Memory.queryProtection(args[0]).includes("x")) return;
        this.isExecutableBlock = true;

        console.log('memmove: enter hooked with addr', args[0], 'and block size', args[2], '. Dumping function code...');
        let file = new File("function_dump.bin", 'wb');
        file.write(args[1].readByteArray(args[2].toInt32()));
        file.flush();
        file.close();
        console.log('memmove: function code dumped!');
    },
    onLeave: function(retval) {
        if (!this.isExecutableBlock) return;

        console.log('memmove: return hooked, intercepting with function code block...');
        let codeBlockAddr = retval.toString();
        Interceptor.attach(retval, {
            onEnter: function(args) {
                console.log(`${codeBlockAddr}: enter hooked, enumerating args:`);
                for(let i = 0; i < 3; i++) {
                    console.log(`args[${i}] = ${args[i]}`);
                }
                console.log('1 arg:', args[0].readCString());
                console.log("2 arg:\n", args[1].readByteArray(args[2].toInt32()));
            },
            onLeave: function(rval) {
                console.log(`${codeBlockAddr}: return hooked with result`, rval, '. Replacing with TRUE...');
                rval.replace(1);
            }
        });
    }
});