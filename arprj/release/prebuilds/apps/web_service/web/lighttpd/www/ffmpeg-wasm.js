var FFMPEGWASM = function() {
    this.ffmpegDecoder = null;
    this.coreLogLevel       = 1;
    this.cacheBuffer = null;
    this.defaultFifoSize = 1024 * 1024;
    this.pcmBufferSize = 128 * 1024;
    this.wasmLoaded = false;
    this.onVideoCallback = null;
    this.cacheSize = 32 * 1024;
    this.cacheMessage;
    console.info("FFMPEGWASM created.");
    this.decodePaused = false;
};

FFMPEGWASM.prototype.initializeWasmDecoder = function(args) {
    var decoder = this;
    if (this.ffmpegDecoder) {
        return true;
    }

    if(this.wasmLoaded == false) {
        return false;
    }

    if(this.onVideoCallback == null) {
        this.onVideoCallback = Module.addFunction(function(buf, size, width, height, ts){
            var outArray = Module.HEAPU8.subarray(buf, buf + size);
            var data = new Uint8Array(outArray);
            var resp = {
                method: "OnVideoFrame",
                result: {
                    data: data,
                    size: size,
                    width: width,
                    height: height,
                    ts: ts
                }
            }
            self.postMessage(resp, [resp.result.data.buffer]);
        });
    }

    var vcodecNamePtr = allocate(intArrayFromString(args.vcodec?args.vcodec:"h264"), 'i8', ALLOC_NORMAL);
    var acodecNamePtr = null;
    var afmtPtr = null;
    this.ffmpegDecoder = Module._ffmpeg_init_decoder(
        -1,
        this.coreLogLevel,
        this.pcmBufferSize,
        this.defaultFifoSize,
        vcodecNamePtr,
        args.vfmt.fmt,
        args.vfmt.width,
        args.vfmt.height,
        args.vfmt.fps,
        this.onVideoCallback,
        acodecNamePtr,
        null,
        null,
        null);

    if(vcodecNamePtr) {
        _free(vcodecNamePtr);
    }
    if(acodecNamePtr) {
       _free(acodecNamePtr);
    }

	if (!this.ffmpegDecoder) {
        console.error("initDecoder failed.");
		return false;
	}
    this.cacheSize = args.cacheSize?args.cacheSize:this.cacheSize;
    this.cacheBuffer = Module._malloc(this.cacheSize);
    return true;
};

FFMPEGWASM.prototype.destroy = function() {
	if (!this.ffmpegDecoder) {
		return true;
	}
	Module._ffmpeg_uninit_decoder();

    if (this.cacheBuffer != null) {
        Module._free(this.cacheBuffer);
        this.cacheBuffer = null;
    }
    console.info("FFMPEGWASM destroy")
    return true;
};

FFMPEGWASM.prototype.openDecoder = function() {
	if (!this.ffmpegDecoder) {
		return;
	}
	Module._ffmpeg_open_decoder();
};

FFMPEGWASM.prototype.feedData = function(args) {
	if (!this.ffmpegDecoder) {
        return false;
	}
    var typedArray = new Uint8Array(args.data);
    var len = args.datalength;

    while (len > 0) {
        slen = len > this.cacheSize ? this.cacheSize : len;
        Module.HEAPU8.set(typedArray.subarray(args.datalength - len, args.datalength - len + slen),
                          this.cacheBuffer);
        Module._ffmpeg_send_data(this.cacheBuffer, slen);
        len -= slen;
    }
    /*
    Module.HEAPU8.set(typedArray, this.cacheBuffer);
    Module._ffmpeg_send_data(this.cacheBuffer, len);*/
    return true;
};

FFMPEGWASM.prototype.decode = function() {
	if (!this.ffmpegDecoder) {
		return false;
	}
   // console.error("decode");
   /*
	var ret = Module._ffmpeg_decode_one_packet();
    if(ret != 0) {
        return false;
    }*/
    if(this.decodePaused == true) {
        this.decodePaused = false;
    }
	return true;
};


FFMPEGWASM.prototype.pause = function() {
	if (!this.ffmpegDecoder) {
		return false;
	}
    if(this.decodePaused == false) {
        this.decodePaused = true;
        Module._ffmpeg_pause();
    }
	return true;
};

self.Module = {
    onRuntimeInitialized: function () {
        console.log("onRuntimeInitialized");
        if (self.decoder) {
            self.decoder.wasmLoaded = true;
            if(self.decoder.cacheMessage) {
                var result = eval('self.decoder.'+self.decoder.cacheMessage.method +
                                '('+JSON.stringify(self.decoder.cacheMessage.args)+')');
                self.postMessage({method: self.decoder.cacheMessage.method, result: result});
            }
        }
    }
};

self.decoder = new FFMPEGWASM;
self.importScripts("libffmpeg.js");

self.onmessage = function(event) {
    if (!self.decoder) {
        console.error("Decoder not created!");
        return;
    }


    var method = event.data.method;
    var args = event.data.args;
    var result = false;
    //console.log("decoder onMessage:" + method)
    if (self.decoder.wasmLoaded == false) {
        if (method == "initializeWasmDecoder") {
            self.decoder.cacheMessage = {method:method, args:args};
        }
        return;
    }
    if (method == "feedData") {
        result = self.decoder.feedData(args);
    }
    else {
        result = eval('self.decoder.'+method +'('+JSON.stringify(args)+')');
    }
    self.postMessage({method: method, result: result});
}

