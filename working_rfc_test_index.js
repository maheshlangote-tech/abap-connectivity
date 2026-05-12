const { Client } = require("node-rfc");

// SAP ABAP Connection Parameters
const abapSystem = {
    user: "YOUR_USERNAME",
    passwd: "YOUR_PASSWORD",
    ashost: "YOUR_SAP_HOST",
    sysnr: "00",
    client: "100",
    lang: "EN"
};

async function connectSAP() {
    const client = new Client(abapSystem);

    try {
        console.log("Connecting to SAP...");

        await client.open();

        console.log("SAP Connection Successful");

        // Test RFC Function Module
        const result = await client.call("STFC_CONNECTION", {
            REQUTEXT: "Hello SAP"
        });

        console.log("RFC Response:");
        console.log(JSON.stringify(result, null, 2));

        await client.close();

        console.log("Connection Closed");

    } catch (error) {
        console.error("SAP Connection Failed");
        console.error(error);
    }
}

connectSAP();