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
    app_models(name, package, interface)
VALUES
    (
        "IotApplication",
        "simcan2.Applications.UserApps.IotApplication",
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

INSERT INTO
    apps(name, model_id, parameters)
VALUES
    (
        "SmartHomeAppEndToEnd",
        3,
        JSON_OBJECT(
            "processingMIs",
            "1",
            "listeningPort",
            "1000"
        )
    );


INSERT INTO
    app_models(name, package, interface)
VALUES
    (
        "FactoryApp",
        "simcan2.Applications.UserApps.FactoryApp",
        JSON_OBJECT(
            "listeningPort",
            "int"
        )
    ),
    (
        "FarmApp",
        "simcan2.Applications.UserApps.FarmApp",
        JSON_OBJECT(
            "listeningPort",
            "int"
        )
    ),
    (
        "BuildingApp",
        "simcan2.Applications.UserApps.BuildingApp",
        JSON_OBJECT(
            "listeningPort",
            "int"
        )
    );

INSERT INTO
    apps(name, model_id, parameters)
VALUES
    (
        "FactoryApp",
        4,
        JSON_OBJECT(
            "listeningPort",
            "1000"
        )
    ),
    (
        "FarmApp",
        5,
        JSON_OBJECT(
            "listeningPort",
            "1000"
        )
    ),
    (
        "BuildingApp",
        6,
        JSON_OBJECT(
            "listeningPort",
            "1000"
        )
    );

-- Confirmation query
SELECT apps.name, app_models.name
FROM apps JOIN app_models ON apps.model_id = app_models.id;