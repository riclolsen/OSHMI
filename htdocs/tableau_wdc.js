(function () {
    var myConnector = tableau.makeConnector();

    myConnector.getSchema = function (schemaCallback) {
    var cols = [
        { id : "value", alias : "value", columnRole: "measure", dataType : tableau.dataTypeEnum.float },
        { id : "failed", alias : "failed", columnRole: "measure", dataType : tableau.dataTypeEnum.bool },
        { id : "tag", alias : "tag", dataType : tableau.dataTypeEnum.string },
        { id : "point_key", alias : "point_key", dataType : tableau.dataTypeEnum.int },
        { id : "status", alias : "status", dataType : tableau.dataTypeEnum.string },
        { id : "timestamp", alias : "timestamp", columnRole: "dimension", dataType : tableau.dataTypeEnum.datetime }
    ];

    var tableInfo = {
        id : "OSHMI_hist_feed",
        alias : "OSHMI historical values of points.",
        columns : cols
    };

    schemaCallback([tableInfo]);
    };
    
    myConnector.getData = function(table, doneCallback) {
    
    $.getJSON("json.php?"+tableau.connectionData, function(resp) {
        tableData = [];

        // Iterate over the JSON object
        for (var i = 0, len = resp.length; i < len; i++) {
            
            var dt = new Date(resp[i].timestamp * 1000);
            var dateString =
              dt.getFullYear() + "/" +
              ("0" + (dt.getMonth()+1)).slice(-2) + "/" +
              ("0" + dt.getDate()).slice(-2) + " " +
              ("0" + dt.getHours()).slice(-2) + ":" +
              ("0" + dt.getMinutes()).slice(-2) + ":" +
              ("0" + dt.getSeconds()).slice(-2) ;
              // + "." + ("00" + dt.getUTCMilliseconds()).slice(-3);

            tableData.push({
                "tag": resp[i].tag,
                "point_key": resp[i].point_key,
                "value": resp[i].value,
                "status": resp[i].status,
                "failed": resp[i].failed,
                "timestamp": dateString
            });
        }

        table.appendRows(tableData);
        doneCallback();
    });
    };
    tableau.registerConnector(myConnector);
    
    $(document).ready(function () {
      $("#submitButton").click(function () {
      tableau.connectionData = $("#taglist").val();
      tableau.connectionData = tableau.connectionData.replace(" ", ",");
      tableau.connectionData = "FILTER=" + tableau.connectionData;
      if ( $("#fromdate").datepicker().val() != "" )
        tableau.connectionData += "&INITDATE=" + $("#fromdate").datepicker().val();
      if ( $("#todate").datepicker().val() != "" )
        tableau.connectionData += "&ENDDATE=" + $("#todate").datepicker().val();
      tableau.connectionData += "&INTERVAL=" + $("#interval").val();      
      tableau.connectionName += "OSHMI Historical Data Feed";
      tableau.submit();
      });
    });

})();
