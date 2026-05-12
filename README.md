# abap-connectivity

requirement
node version 18
npm version 10

### Set up linux nwrfc sdk path

export SAPNWRFC_HOME=/home/sap-abap-cribl-collector/abap-connectivity/node_modules/@sap-rfc/node-rfc-library/dist/sapnoderfcaddons/nwrfcsdk/linux

export LD_LIBRARY_PATH=$SAPNWRFC_HOME/lib:$LD_LIBRARY_PATH
