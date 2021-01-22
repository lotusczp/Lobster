var vcu1RollingCounterTick = (function(max) {
    var i = 0;
    return function (max) {
        i++;
        if(i>max) i = 0;
        return i;
    }
}
)();

var DBCMessage = {
    VCU1 : {
        cycleTime_ms : 50,
        OnSend : function () {
            //console.log(lbDbcMgr.getSigValue("VCU1_RollingCounter"));
            //lbDbcMgr.setSigValue("VCU1_RollingCounter", lbDbcMgr.getSigValue("VCU1_RollingCounter")+1);
            
            lbDbcMgr.setSigValue("VCU1_CheckSum", 0);
            lbDbcMgr.setSigValue("VCU1_MCUModReq", 0);
            lbDbcMgr.setSigValue("VCU1_CrashOutputSts", 0);
            lbDbcMgr.setSigValue("VCU1_MCUTrqSetPoint", 0);
            lbDbcMgr.setSigValue("VCU1_ChrgEnable", 0);
            lbDbcMgr.setSigValue("VCU1_BMSEmgyToShutDown", 0);
            lbDbcMgr.setSigValue("VCU1_RollingCounter", vcu1RollingCounterTick(15));
            lbDbcMgr.setSigValue("VCU1_HillHold", 0);
            
            
            console.log(lbDbcMgr.getMsgBytes("VCU1")[0]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[1]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[2]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[3]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[4]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[5]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[6]);
            console.log(lbDbcMgr.getMsgBytes("VCU1")[7]);
            console.log("-----------------------------");
            
            
        },
    },
    VCU2 : {
        cycleTime_ms : 500,
        OnSend : function () {
        },
    },
};