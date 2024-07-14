INSERT INTO
    app_models(name, package, interface)
VALUES
    (
        "LocalIotApplication",
        "s2f.Applications.UserApps.LocalIotApplication",
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
        "s2f.Applications.UserApps.IotApplication",
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
        "s2f.Applications.UserApps.FactoryApp",
        JSON_OBJECT(
            "listeningPort",
            "int"
        )
    ),
    (
        "FarmApp",
        "s2f.Applications.UserApps.FarmApp",
        JSON_OBJECT(
            "listeningPort",
            "int"
        )
    ),
    (
        "BuildingApp",
        "s2f.Applications.UserApps.BuildingApp",
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

-- Update path query
UPDATE app_models
SET package = 's2f.apps.models.LocalIotApplication'
WHERE name = 'LocalIotApplication';