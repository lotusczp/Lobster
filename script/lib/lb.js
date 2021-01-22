include("lib/map.js");

/// The lobster 12-color palette
/// \todo Is it need to be global?
var colorDef = {};
Object.defineProperty(colorDef, "tomato", {writable: false, configurable: false, enumerable: true, value: "#ff3D3D"});
Object.defineProperty(colorDef, "seaGreen", {writable: false, configurable: false, enumerable: true,value: "#23AE23"});
Object.defineProperty(colorDef, "rose", {writable: false, configurable: false, enumerable: true, value: "#F53093"});
Object.defineProperty(colorDef, "royalblue", {writable: false, configurable: false, enumerable: true, value: "#3C48E8"});
Object.defineProperty(colorDef, "eggYellow", {writable: false, configurable: false, enumerable: true, value: "#FFFF80"});
Object.defineProperty(colorDef, "salmon", {writable: false, configurable: false, enumerable: true, value: "#FF9595"});
Object.defineProperty(colorDef, "peachPuff", {writable: false, configurable: false, enumerable: true, value: "#FFCA95"});
Object.defineProperty(colorDef, "paleGreen", {writable: false, configurable: false, enumerable: true, value: "#AFFF95"});
Object.defineProperty(colorDef, "mintcream", {writable: false, configurable: false, enumerable: true, value: "#D5FFF4"});
Object.defineProperty(colorDef, "skyBlue", {writable: false, configurable: false, enumerable: true, value: "#95CAFF"});
Object.defineProperty(colorDef, "mediumPurple", {writable: false, configurable: false, enumerable: true, value: "#9595FF"});
Object.defineProperty(colorDef, "pink", {writable: false, configurable: false, enumerable: true, value: "#FF95E4"});
/// The color array inside colorDef object
colorDef.colorArray = function () {
  var that = [], index = 0;
  for (var prop in colorDef) {
    that[index] = (colorDef[prop]);
    index++;
  }
  return that;
}();
Object.defineProperty(colorDef, "colorArray", {writable: false, configurable: false});

/// lb is the OO style Lobster library. 
/// The global "namespace" lb is define here.
var lb = function () {
  return {
  };
}();


/// Option definitions
lb.config = {};                   // Lobster library options object
lb.config.updateInterval = 100;   // Default update interval to 100ms
lb.config.equalThres = 0.001;     // Default threshold when comparing equal of floating numbers


/// Helper function 
/// It's used for reference counting
lb.refCount = function () {
  var refCountMap = new Map();
  return {
    increase : function (name) {
      if (refCountMap.containsKey(name)) {
        var value = refCountMap.get(name);
        refCountMap.remove(name);
        refCountMap.put(name, value+1);
      }
      else {
        refCountMap.put(name, 1);
      }
    },
    decrease : function (name) {
      var value = refCountMap.get(name);
      refCountMap.remove(name);
      if (value > 0) {
        refCountMap.remove(name);
        refCountMap.put(name, value-1);
      }
    },
    isZero : function (name) {
      if (refCountMap.containsKey(name)) {
        return refCountMap.get(name) === 0;
      }
      else {
        return true;
      }
    }
  };
}();
  

/// Object - main
/// It's used for main window function
lb.main = function () {
  return {
    startTrans : function () {
      lbMain.startTrans();
    },
    stopTrans : function () {
      lbMain.stopTrans();
    }
  };
}();


/// Object - system
/// It's used for lobster system function
lb.system = function () {
  return {
    /// Get the current time
    /// \return The system time in millisecond 
    now : function () {
      // The absTime return value is a double type of second
      // So convert it to ms by *1000, and round it
      return Math.round(lbSystem.absTime() * 1000);
    },
    /// Wait for a while based on the input argument in millisecond
    /// \param ms : X milliseconds to wait
    wait : function (ms) {
      lbSystem.wait(ms);
    },
    /// Play sound of the input argument
    /// \param content : Content to say
    say : function (content) {
      lbSystem.say(content);
    },
    /// Print the input string or object to Lobster console
    /// \param content : Content to print
    print : function (content) {
      lbSystem.consoleWrite(content);
    },
    /// Pause the script execution only when the script is 
    /// running in the debug mode!
    pause : function () {
      lbSystem.pause();
    }
  };
}();


/// Constructor - point
/// It's used for lobster point
lb.point = function (spec) {
  // Handle the channel name to satisfy Lobster channel prefix
  spec.name = spec.channel + "_" + spec.point;
  // This is private variable to store the value for debug mode
  spec._val = 0;
  
  // a simple hash value calculator
  _calcHash = function (str){
    var hash = 0, i, chr, len;
    if ( (typeof str == 'string') && str.constructor == String ) {
      if (str.length == 0) return hash;
      for (i = 0, len = str.length; i < len; i++) {
        chr   = str.charCodeAt(i);
        hash  = hash + chr;
        hash |= 0; // Convert to 32bit integer
      }
      return hash; 
    }
    else {
      return 0;
    }
  };
  
  // Return the point instance
  return {
    subscribe : function () {
      if (spec.name != undefined) {
        // increase ref count
        lb.refCount.increase(spec.name);
        // subscribe this point
        lbPoint.subscribe(spec.name);
        if (spec.color != undefined) {
          lbPoint.setCurveColor(spec.name, spec.color);
        }
        if ( (spec.curveGain != undefined) && (parseFloat(spec.curveGain) != NaN) ) {
          lbPoint.setCurveGain(spec.name, parseFloat(spec.curveGain));
        }
        if ( (spec.curveOffset != undefined) && (parseFloat(spec.curveOffset) != NaN) ) {
          lbPoint.setCurveOffset(spec.name, parseFloat(spec.curveOffset));
        }
        if (spec.curveEnable === true) {
          lbPoint.setCurveEnable(spec.name, true);
          // if the curve is enabled, but hasn't assigned other properties,
          // give a default color based on the hash value of point name
          // lobster will set the default gain and offset
          if (spec.color === undefined) {
            var hash = _calcHash(spec.name);
            // 12 colors, see the palette
            spec.color = colorDef.colorArray[hash%(colorDef.colorArray.length)];
            lbPoint.setCurveColor(spec.name, spec.color);
          }
        }
        else if (spec.curveEnable === false) {
          lbPoint.setCurveEnable(spec.name, false);
        }
        if(spec.logEnable == true) {
          lbPoint.setLogEnable(spec.name, true);
        }
        else if(spec.logEnable == false) {
          lbPoint.setLogEnable(spec.name, false);
        }
      }
    },
    
    unsubscribe : function () {
      if (spec.name) {
        lb.refCount.decrease(spec.name);
        // Make sure no other reference of this point before unsubscribe it from Lobster!
        if (lb.refCount.isZero(spec.name)) {
          lbPoint.unsubscribe(spec.name);
        }
      }
    },
  
    getValue : function () {
      spec._val = lbPoint.getValue(spec.name);
      return spec._val;
    },
    
    setValue : function (value) {
      lbPoint.setValue(spec.name, value);
      spec._val = value;
    },

    pullValue : function () {
      lbPoint.pullValue(spec.name) 
    },

    getName : function () {
      return spec.name;
    },
    
    // Debugging only property, the value won't be visible in the 
    // Lobster script debug IDE without this property.
    // Don't use it in the user script, please!!!
    __watch : spec,
  };
};


/// Object - test
/// It's used for testing or verifying something
lb.test = function () {
  return {
    /// Wait for the callback function returns true before timeout in milliseconds.
    /// If callback returns true before timeout, means waitFor OK,
    /// otherwise waitFor failed.
    waitFor : function (callback, timeoutMS) {
      var ret = false, curTime = startTime = lb.system.now();
      // Loop until the conditions in callback function are satisfied, or timeout
      while (!ret && (curTime - startTime < timeoutMS + lb.config.updateInterval)) {
        // Invoke the callback function
        ret = callback();
        if (!ret) {
          // Wait a little while
          lb.system.wait(lb.config.updateInterval);
          curTime = lb.system.now();
        }
      }
      // Return the result & elapsed time in MS
      return { result : ret, timeElapsed : curTime - startTime};
    },
    
    /// Wait while the callback funtin return true, until reach the timeout
    /// condition
    waitWhile : function (callback, timeoutMS) {
      var ret = true, curTime = startTime = lb.system.now({trace : false});
      // Loop until the conditions in callback function are satisfied, or timeout
      while (ret && (curTime - startTime < timeoutMS + lb.config.updateInterval)) {
        // Invoke the callback function
        ret = callback();
        if (ret) {
          // Wait a little while
          lb.system.wait(lb.config.updateInterval, {trace : false});
          curTime = lb.system.now({trace : false});
        }
      }
      // Return the result & elapsed time in MS
      return { result : ret, timeElapsed : curTime - startTime};
    },

    /// Set and verify the point value
    /// Callback function is called when verify fails.
    setAndVerify : function (ptr, value, timeout_ms, failCallback, zeroThres) {
      zeroThres = zeroThres || lb.config.equalThres;
      failCallback = failCallback || function() {} // Do nothing if no callback function is passed in
      ptr.setValue(value); // Set the value
      lbSystem.wait(timeout_ms*0.5); // Wait a while to read back
      ptr.pullValue();  // Pull value if needed
      lbSystem.wait(timeout_ms*0.5); // Wait for transmission.
      /// We treat the transmission time cost of set and pull to be equal
      /// So half of the timeout for each.
      var diff = ptr.getValue()-value;
      if(diff<-zeroThres || diff>zeroThres) {
        failCallback();
        return false;
      }
      else {
        return true;
      }
    },

    matchTwoCurves : function (ptr1, ptr2, interval_ms, time_ms, errAvgAllow, errRangeAllow) {
      var avgRes = false;
      var rangeRes = false;
      var sumError = 0;
      var maxPosError = 0;
      var minNegError = 0;
      var count = Math.round(time_ms/interval_ms);
      for(var i=0; i<count; i++) {
        var error = ptr1.getValue() - ptr2.getValue();
        sumError = sumError + error;
        if(error > 0 && error > maxPosError) {
          maxPosError = error;
        }
        else if(error < 0 && error < minNegError) {
          minNegError = error;
        }
        lbSystem.wait(interval_ms);
      }
      var avgError = sumError / count;
      if(avgError > -errAvgAllow && avgError < errAvgAllow) {
        avgRes = true;
      }
      if(maxPosError > 0 && maxPosError < errRangeAllow && minNegError < 0 && minNegError > -errRangeAllow) {
        // We have positive error and negative error, and both of them are within the range
        rangeRes = true;
      }
      return {result: (avgRes&&rangeRes)};
    },

  };
}();


// add short cuts
var console = {};
console.log = lb.system.print;