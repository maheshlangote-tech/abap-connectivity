const { Client } = require("node-rfc");
const { Readable } = require("stream");

exports.name = "SAP RFC Collector";
exports.version = "1.0.0";
exports.disabled = false;
exports.destroyable = false;

let collectorConf = {};

const logger = C.util.getLogger("sap_rfc_collector");

/*
    INIT
    Called when collector starts
    Receives UI config from Cribl Dashboard
*/
exports.init = async (opts) => {

    logger.info("Initializing SAP RFC Collector");

    collectorConf = opts;

    logger.info("Collector Configuration Loaded", {
        ashost: opts.ashost,
        sysnr: opts.sysnr,
        client: opts.client,
        lang: opts.lang,
        rfc_function: opts.rfc_function
    });
};

/*
    DISCOVER
    Creates collection tasks
*/
exports.discover = async (job) => {

    job.logger().info("Running discover phase");

    job.addResult({
        name: "sap_rfc_task"
    });
};

/*
    COLLECT
    Executes RFC and streams events
*/
exports.collect = async (collectible, job) => {

    const jobLogger = job.logger();

    jobLogger.info("Starting SAP RFC Collection", {
        collectible
    });

    let client;

    try {

        /*
            Create SAP Client
        */
        client = new Client({
            user: collectorConf.user,
            passwd: collectorConf.passwd,
            ashost: collectorConf.ashost,
            sysnr: collectorConf.sysnr,
            client: collectorConf.client,
            lang: "EN"
        });

        /*
            Open Connection
        */
        await client.open();

        jobLogger.info("SAP Connection Successful");

        /*
            Execute RFC
        */
        const result = await client.call(
            collectorConf.rfc_function || "STFC_CONNECTION",
            {
                REQUTEXT:
                    collectorConf.request_text ||
                    "Hello From Cribl"
            }
        );

        jobLogger.info("RFC Call Successful");

        /*
            Convert Result To Stream
        */
        const stream = new Readable({
            read() {}
        });

        stream.push(
            JSON.stringify({
                source: "sap_abap",
                sourcetype: "sap:rfc",
                host: collectorConf.ashost,
                timestamp: Date.now(),
                collectible: collectible.name,
                rfc: collectorConf.rfc_function,
                data: result
            })
        );

        stream.push(null);

        return Promise.resolve(stream);

    } catch (err) {

        jobLogger.error("SAP RFC Collection Failed", {
            error: err.message,
            stack: err.stack
        });

        throw err;

    } finally {

        /*
            Close SAP Connection
        */
        if (client) {

            try {

                await client.close();

                jobLogger.info("SAP Connection Closed");

            } catch (closeErr) {

                jobLogger.error("Failed Closing SAP Connection", {
                    error: closeErr.message
                });
            }
        }
    }
};