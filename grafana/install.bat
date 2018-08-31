cd c:\oshmi\grafana
copy custom.ini conf\

cd c:\oshmi\grafana\
call grafana_start

cd c:\oshmi\grafana\bin
grafana-cli plugins install ayoungprogrammer-finance-datasource
grafana-cli plugins install bessler-pictureit-panel
grafana-cli plugins install blackmirror1-singlestat-math-panel
grafana-cli plugins install blackmirror1-statusbygroup-panel
grafana-cli plugins install briangann-datatable-panel
grafana-cli plugins install briangann-gauge-panel
grafana-cli plugins install btplc-alarm-box-panel
grafana-cli plugins install btplc-peak-report-panel
grafana-cli plugins install btplc-status-dot-panel
grafana-cli plugins install btplc-trend-box-panel
grafana-cli plugins install citilogics-geoloop-panel
grafana-cli plugins install digiapulssi-organisations-panel
grafana-cli plugins install digrich-bubblechart-panel
grafana-cli plugins install fzakaria-simple-annotations-datasource
grafana-cli plugins install grafana-clock-panel
grafana-cli plugins install grafana-piechart-panel
grafana-cli plugins install grafana-simple-json-datasource
grafana-cli plugins install jdbranham-diagram-panel
grafana-cli plugins install marcuscalidus-svg-panel
grafana-cli plugins install michaeldmoore-annunciator-panel
grafana-cli plugins install mtanda-google-calendar-datasource
grafana-cli plugins install mtanda-histogram-panel
grafana-cli plugins install natel-discrete-panel
grafana-cli plugins install natel-plotly-panel
grafana-cli plugins install neocat-cal-heatmap-panel
grafana-cli plugins install novalabs-annotations-panel
grafana-cli plugins install petrslavotinek-carpetplot-panel
grafana-cli plugins install ryantxu-ajax-panel
grafana-cli plugins install satellogic-3d-globe-panel
grafana-cli plugins install savantly-heatmap-panel
grafana-cli plugins install snuids-radar-panel
grafana-cli plugins install snuids-trafficlights-panel
grafana-cli plugins install vonage-status-panel
grafana-cli plugins install zuburqan-parity-report-panel

cd c:\oshmi\grafana\
call grafana_stop
call grafana_start
