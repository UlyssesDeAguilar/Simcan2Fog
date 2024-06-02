INSERT INTO
    app_models(name, package, interface)
VALUES
    (
        "LocalIotApplication",
        "simcan2.Applications.UserApps.LocalIotApplication",
        JSON_OBJECT(
            "processingMIs",
            "int",
            "listeningPort",
            "int"
        )
    );

INSERT INTO
    apps(name, model_id, parameters)
VALUES
    (
        "SmartHomeApp",
        2,
        JSON_OBJECT(
            "processingMIs",
            "100",
            "listeningPort",
            "1000"
        )
    );