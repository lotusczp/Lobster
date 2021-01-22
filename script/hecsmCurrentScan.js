include("lib/lb.js");

var hecsmCurrentScan = function () {
    // Private variables
    var IdRef = lb.point({channel : "Cgd", point : "IdRef", curveEnable : true, logEnable : true, });
    var IdFdb = lb.point({channel : "Cgd", point : "IdFdb", curveEnable : true, logEnable : true, });
    var IqRef = lb.point({channel : "Cgd", point : "IqRef", curveEnable : true, logEnable : true, });
    var IqFdb = lb.point({channel : "Cgd", point : "IqFdb", curveEnable : true, logEnable : true, });
    var IeRef = lb.point({channel : "Cgd", point : "IeRef", curveEnable : true, logEnable : true, });
    var IeFdb = lb.point({channel : "Cgd", point : "IeFdb", curveEnable : true, logEnable : true, });
    var enablePwm = lb.point({channel : "Cgd", point : "EnablePwm", curveEnable : true, logEnable : true, });

    // Private functions
    var setAndVerify = function (ptr, value, equalThres) {
        lb.test.setAndVerify (  ptr, 
                                value,
                                200,        /* Verify timeout in ms*/
                                function() {
                                    throw new Error("Fail to set the point " + ptr.getName() + "\n" + 
                                    "The point value is " + ptr.getValue().toString() + ", but the expected value is " + value.toString() + "\n");
                                },
                                equalThres
                            );
    };

    // Public interfaces
    return {
        init : function() {
            // Initialize some configuration
            lb.config.updateInterval = 20;

            // Subscribe needed points
            IdRef.subscribe();
            IdFdb.subscribe();
            IqRef.subscribe();
            IqFdb.subscribe();
            IeRef.subscribe();
            IeFdb.subscribe();
            enablePwm.subscribe();

            // Set values if needed
            setAndVerify(IdRef, 0);
            setAndVerify(IqRef, 0);
            setAndVerify(enablePwm, 0);
        },

        process : function() {
            setAndVerify(enablePwm, 1);
            IsLimitSquare = 350*350;
            for(var IeStep = 0; IeStep < 11; IeStep++) {
                var IeCmd_A = IeStep * 1;
                for(var IdStep = 0; IdStep < 30; IdStep++) {
                    var IdCmd_A = IdStep * 10;
                    for(var IqStep = 0; IqStep < 30; IqStep++) {
                        var IqCmd_A = IqStep * 10;
                        if(IdCmd_A*IdCmd_A+IqCmd_A*IqCmd_A < IsLimitSquare) { // Abandon meaningless points
                            // Give the command
                            setAndVerify(IeRef, IeCmd_A, 0.05);
                            setAndVerify(IdRef, IdCmd_A, 0.05);
                            setAndVerify(IqRef, IqCmd_A, 0.05);
                            lb.system.wait(1000); // Waiting 5s to stablize
                            
                            // Verify the curves if needed
                            // TODO just test, can be removed
                            //res = lb.test.matchTwoCurves(IdRef, IdFdb, 40, 3000, 2, 10);
                            //console.log(res.result);

                            // Cool down
                            setAndVerify(IeRef, 0);
                            setAndVerify(IdRef, 0);
                            setAndVerify(IqRef, 0);
                            lb.system.wait(10000); // Waiting 10s
                            // TODO wait for temperature to drop if we have the input
                        }
                    }
                }
            }
        },
    };
}();


hecsmCurrentScan.init();
hecsmCurrentScan.process();