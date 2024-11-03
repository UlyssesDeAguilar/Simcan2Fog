/*
 CREATE USER 'sim' IDENTIFIED BY 'change-me';
 
 GRANT ALL
 ON simcan2fog.*
 TO 'sim'@'%';
 
 GRANT SELECT, INSERT, UPDATE ON simcan2fog.* TO 'sim'@'%';
 */
-- VM definitions
CREATE TABLE vms (
    id INT unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,
    type VARCHAR(20) NOT NULL UNIQUE,
    cost DECIMAL(5, 2) NOT NULL,
    cores INT unsigned,
    scu DOUBLE PRECISION,
    disk DOUBLE PRECISION,
    memory DOUBLE PRECISION
);

INSERT INTO
    vms(type, cost, cores, scu, disk, memory)
VALUES
    ("VM_4xlarge", 23.0, 16, 16.0, 1000.0, 64.0),
    ("VM_2xlarge", 23.0, 8, 8.0, 1000.0, 32.0),
    ("VM_xlarge", 23.0, 4, 4.0, 1000.0, 16.0),
    ("VM_large", 23.0, 4, 4.0, 1000.0, 8.0),
    ("VM_medium", 15.0, 2, 2.0, 500.0, 4.0),
    ("VM_small", 15.0, 1, 1.0, 250.0, 2.0),
    ("VM_micro", 15.0, 1, 1.0, 100.0, 1.0),
    ("VM_nanoHD", 15.0, 1, 1.0, 500.0, 0.5),
    ("VM_nanoRAM", 15.0, 1, 1.0, 100.0, 2.0),
    ("VM_nano", 15.0, 1, 1.0, 100.0, 0.5);

-- SLA definitions
CREATE TABLE sla (
    id INT unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,
    type VARCHAR(20) NOT NULL UNIQUE
);

INSERT INTO
    sla(type)
VALUES
    ("Slai0d0c0"),
    ("Slai05d0c0"),
    ("Slai0d02c0"),
    ("Slai0d0c01"),
    ("Slai05d02c0"),
    ("Slai05d0c01"),
    ("Slai05d02c01"),
    ("Slai0d02c01"),
    ("Slai05d02c015"),
    ("Slai05d02c018"),
    ("Slai05d02c02"),
    ("Slai05d02c022"),
    ("Slai05d02c025"),
    ("Slai05d02c028"),
    ("Slai05d02c03"),
    ("Slai05d02c04"),
    ("Slai05d05c01"),
    ("Slai02d02c01"),
    ("Slai05d015c01"),
    ("Slai05d045c01"),
    ("Slai05d03c01"),
    ("Slai015d02c01"),
    ("Slai045d02c01"),
    ("Slai03d02c01");

-- Relation between SLA and offered VMs
CREATE TABLE sla_vms (
    sla_id INT unsigned,
    vm_id INT unsigned,
    cost DECIMAL(5, 2) DEFAULT 0.0,
    -- "Base" cost for renting the machine
    increase DECIMAL(5, 2) DEFAULT 0.0,
    discount DECIMAL(5, 2) DEFAULT 0.0,
    compensation DECIMAL(5, 2) DEFAULT 0.0,
    PRIMARY KEY (sla_id, vm_id),
    FOREIGN KEY (sla_id) REFERENCES sla(id),
    -- MySQL ignores inline REFERENCES field declaration so it has to be declared this way
    FOREIGN KEY (vm_id) REFERENCES vms(id)
);

INSERT INTO
    sla_vms(sla_id, vm_id, cost, increase, discount, compensation)
VALUES
    (1, 1, 0.92, 0.0, 0.0, 0.0),
    (1, 2, 0.46, 0.0, 0.0, 0.0),
    (1, 3, 0.23, 0.0, 0.0, 0.0),
    (1, 4, 0.12, 0.0, 0.0, 0.0),
    (1, 5, 0.05, 0.0, 0.0, 0.0),
    (1, 6, 0.03, 0.0, 0.0, 0.0),
    (1, 7, 0.02, 0.0, 0.0, 0.0),
    (1, 8, 0.01, 0.0, 0.0, 0.0),
    (1, 9, 0.01, 0.0, 0.0, 0.0),
    (1, 10, 0.01, 0.0, 0.0, 0.0),
    (2, 1, 0.92, 0.5, 0.0, 0.0),
    (2, 2, 0.46, 0.5, 0.0, 0.0),
    (2, 3, 0.23, 0.5, 0.0, 0.0),
    (2, 4, 0.12, 0.5, 0.0, 0.0),
    (2, 5, 0.05, 0.5, 0.0, 0.0),
    (2, 6, 0.03, 0.5, 0.0, 0.0),
    (2, 7, 0.02, 0.5, 0.0, 0.0),
    (2, 8, 0.01, 0.5, 0.0, 0.0),
    (2, 9, 0.01, 0.5, 0.0, 0.0),
    (2, 10, 0.01, 0.5, 0.0, 0.0),
    (3, 1, 0.92, 0.0, 0.2, 0.0),
    (3, 2, 0.46, 0.0, 0.2, 0.0),
    (3, 3, 0.23, 0.0, 0.2, 0.0),
    (3, 4, 0.12, 0.0, 0.2, 0.0),
    (3, 5, 0.05, 0.0, 0.2, 0.0),
    (3, 6, 0.03, 0.0, 0.2, 0.0),
    (3, 7, 0.02, 0.0, 0.2, 0.0),
    (3, 8, 0.01, 0.0, 0.2, 0.0),
    (3, 9, 0.01, 0.0, 0.2, 0.0),
    (3, 10, 0.01, 0.0, 0.2, 0.0),
    (4, 1, 0.92, 0.0, 0.0, 0.1),
    (4, 2, 0.46, 0.0, 0.0, 0.1),
    (4, 3, 0.23, 0.0, 0.0, 0.1),
    (4, 4, 0.12, 0.0, 0.0, 0.1),
    (4, 5, 0.05, 0.0, 0.0, 0.1),
    (4, 6, 0.03, 0.0, 0.0, 0.1),
    (4, 7, 0.02, 0.0, 0.0, 0.1),
    (4, 8, 0.01, 0.0, 0.0, 0.1),
    (4, 9, 0.01, 0.0, 0.0, 0.1),
    (4, 10, 0.01, 0.0, 0.0, 0.1),
    (5, 1, 0.92, 0.5, 0.2, 0.0),
    (5, 2, 0.46, 0.5, 0.2, 0.0),
    (5, 3, 0.23, 0.5, 0.2, 0.0),
    (5, 4, 0.12, 0.5, 0.2, 0.0),
    (5, 5, 0.05, 0.5, 0.2, 0.0),
    (5, 6, 0.03, 0.5, 0.2, 0.0),
    (5, 7, 0.02, 0.5, 0.2, 0.0),
    (5, 8, 0.01, 0.5, 0.2, 0.0),
    (5, 9, 0.01, 0.5, 0.2, 0.0),
    (5, 10, 0.01, 0.5, 0.2, 0.0),
    (6, 1, 0.92, 0.5, 0.0, 0.1),
    (6, 2, 0.46, 0.5, 0.0, 0.1),
    (6, 3, 0.23, 0.5, 0.0, 0.1),
    (6, 4, 0.12, 0.5, 0.0, 0.1),
    (6, 5, 0.05, 0.5, 0.0, 0.1),
    (6, 6, 0.03, 0.5, 0.0, 0.1),
    (6, 7, 0.02, 0.5, 0.0, 0.1),
    (6, 8, 0.01, 0.5, 0.0, 0.1),
    (6, 9, 0.01, 0.5, 0.0, 0.1),
    (6, 10, 0.01, 0.5, 0.0, 0.1),
    (7, 1, 0.92, 0.5, 0.2, 0.1),
    (7, 2, 0.46, 0.5, 0.2, 0.1),
    (7, 3, 0.23, 0.5, 0.2, 0.1),
    (7, 4, 0.12, 0.5, 0.2, 0.1),
    (7, 5, 0.05, 0.5, 0.2, 0.1),
    (7, 6, 0.03, 0.5, 0.2, 0.1),
    (7, 7, 0.02, 0.5, 0.2, 0.1),
    (7, 8, 0.01, 0.5, 0.2, 0.1),
    (7, 9, 0.01, 0.5, 0.2, 0.1),
    (7, 10, 0.01, 0.5, 0.2, 0.1),
    (8, 1, 0.92, 0.0, 0.2, 0.1),
    (8, 2, 0.46, 0.0, 0.2, 0.1),
    (8, 3, 0.23, 0.0, 0.2, 0.1),
    (8, 4, 0.12, 0.0, 0.2, 0.1),
    (8, 5, 0.05, 0.0, 0.2, 0.1),
    (8, 6, 0.03, 0.0, 0.2, 0.1),
    (8, 7, 0.02, 0.0, 0.2, 0.1),
    (8, 8, 0.01, 0.0, 0.2, 0.1),
    (8, 9, 0.01, 0.0, 0.2, 0.1),
    (8, 10, 0.01, 0.0, 0.2, 0.1),
    (9, 1, 0.92, 0.5, 0.2, 0.15),
    (9, 2, 0.46, 0.5, 0.2, 0.15),
    (9, 3, 0.23, 0.5, 0.2, 0.15),
    (9, 4, 0.12, 0.5, 0.2, 0.15),
    (9, 5, 0.05, 0.5, 0.2, 0.15),
    (9, 6, 0.03, 0.5, 0.2, 0.15),
    (9, 7, 0.02, 0.5, 0.2, 0.15),
    (9, 8, 0.01, 0.5, 0.2, 0.15),
    (9, 9, 0.01, 0.5, 0.2, 0.15),
    (9, 10, 0.01, 0.5, 0.2, 0.15),
    (10, 1, 0.92, 0.5, 0.2, 0.18),
    (10, 2, 0.46, 0.5, 0.2, 0.18),
    (10, 3, 0.23, 0.5, 0.2, 0.18),
    (10, 4, 0.12, 0.5, 0.2, 0.18),
    (10, 5, 0.05, 0.5, 0.2, 0.18),
    (10, 6, 0.03, 0.5, 0.2, 0.18),
    (10, 7, 0.02, 0.5, 0.2, 0.18),
    (10, 8, 0.01, 0.5, 0.2, 0.18),
    (10, 9, 0.01, 0.5, 0.2, 0.18),
    (10, 10, 0.01, 0.5, 0.2, 0.18),
    (11, 1, 0.92, 0.5, 0.2, 0.2),
    (11, 2, 0.46, 0.5, 0.2, 0.2),
    (11, 3, 0.23, 0.5, 0.2, 0.2),
    (11, 4, 0.12, 0.5, 0.2, 0.2),
    (11, 5, 0.05, 0.5, 0.2, 0.2),
    (11, 6, 0.03, 0.5, 0.2, 0.2),
    (11, 7, 0.02, 0.5, 0.2, 0.2),
    (11, 8, 0.01, 0.5, 0.2, 0.2),
    (11, 9, 0.01, 0.5, 0.2, 0.2),
    (11, 10, 0.01, 0.5, 0.2, 0.2),
    (12, 1, 0.92, 0.5, 0.2, 0.22),
    (12, 2, 0.46, 0.5, 0.2, 0.22),
    (12, 3, 0.23, 0.5, 0.2, 0.22),
    (12, 4, 0.12, 0.5, 0.2, 0.22),
    (12, 5, 0.05, 0.5, 0.2, 0.22),
    (12, 6, 0.03, 0.5, 0.2, 0.22),
    (12, 7, 0.02, 0.5, 0.2, 0.22),
    (12, 8, 0.01, 0.5, 0.2, 0.22),
    (12, 9, 0.01, 0.5, 0.2, 0.22),
    (12, 10, 0.01, 0.5, 0.2, 0.22),
    (13, 1, 0.92, 0.5, 0.2, 0.25),
    (13, 2, 0.46, 0.5, 0.2, 0.25),
    (13, 3, 0.23, 0.5, 0.2, 0.25),
    (13, 4, 0.12, 0.5, 0.2, 0.25),
    (13, 5, 0.05, 0.5, 0.2, 0.25),
    (13, 6, 0.03, 0.5, 0.2, 0.25),
    (13, 7, 0.02, 0.5, 0.2, 0.25),
    (13, 8, 0.01, 0.5, 0.2, 0.25),
    (13, 9, 0.01, 0.5, 0.2, 0.25),
    (13, 10, 0.01, 0.5, 0.2, 0.25),
    (14, 1, 0.92, 0.5, 0.2, 0.28),
    (14, 2, 0.46, 0.5, 0.2, 0.28),
    (14, 3, 0.23, 0.5, 0.2, 0.28),
    (14, 4, 0.12, 0.5, 0.2, 0.28),
    (14, 5, 0.05, 0.5, 0.2, 0.28),
    (14, 6, 0.03, 0.5, 0.2, 0.28),
    (14, 7, 0.02, 0.5, 0.2, 0.28),
    (14, 8, 0.01, 0.5, 0.2, 0.28),
    (14, 9, 0.01, 0.5, 0.2, 0.28),
    (14, 10, 0.01, 0.5, 0.2, 0.28),
    (15, 1, 0.92, 0.5, 0.2, 0.3),
    (15, 2, 0.46, 0.5, 0.2, 0.3),
    (15, 3, 0.23, 0.5, 0.2, 0.3),
    (15, 4, 0.12, 0.5, 0.2, 0.3),
    (15, 5, 0.05, 0.5, 0.2, 0.3),
    (15, 6, 0.03, 0.5, 0.2, 0.3),
    (15, 7, 0.02, 0.5, 0.2, 0.3),
    (15, 8, 0.01, 0.5, 0.2, 0.3),
    (15, 9, 0.01, 0.5, 0.2, 0.3),
    (15, 10, 0.01, 0.5, 0.2, 0.3),
    (16, 1, 0.92, 0.5, 0.2, 0.4),
    (16, 2, 0.46, 0.5, 0.2, 0.4),
    (16, 3, 0.23, 0.5, 0.2, 0.4),
    (16, 4, 0.12, 0.5, 0.2, 0.4),
    (16, 5, 0.05, 0.5, 0.2, 0.4),
    (16, 6, 0.03, 0.5, 0.2, 0.4),
    (16, 7, 0.02, 0.5, 0.2, 0.4),
    (16, 8, 0.01, 0.5, 0.2, 0.4),
    (16, 9, 0.01, 0.5, 0.2, 0.4),
    (16, 10, 0.01, 0.5, 0.2, 0.4),
    (17, 1, 0.92, 0.5, 0.5, 0.1),
    (17, 2, 0.46, 0.5, 0.5, 0.1),
    (17, 3, 0.23, 0.5, 0.5, 0.1),
    (17, 4, 0.12, 0.5, 0.5, 0.1),
    (17, 5, 0.05, 0.5, 0.5, 0.1),
    (17, 6, 0.03, 0.5, 0.5, 0.1),
    (17, 7, 0.02, 0.5, 0.5, 0.1),
    (17, 8, 0.01, 0.5, 0.5, 0.1),
    (17, 9, 0.01, 0.5, 0.5, 0.1),
    (17, 10, 0.01, 0.5, 0.5, 0.1),
    (18, 1, 0.92, 0.2, 0.2, 0.1),
    (18, 2, 0.46, 0.2, 0.2, 0.1),
    (18, 3, 0.23, 0.2, 0.2, 0.1),
    (18, 4, 0.12, 0.2, 0.2, 0.1),
    (18, 5, 0.05, 0.2, 0.2, 0.1),
    (18, 6, 0.03, 0.2, 0.2, 0.1),
    (18, 7, 0.02, 0.2, 0.2, 0.1),
    (18, 8, 0.01, 0.2, 0.2, 0.1),
    (18, 9, 0.01, 0.2, 0.2, 0.1),
    (18, 10, 0.01, 0.2, 0.2, 0.1),
    (19, 1, 0.92, 0.5, 0.15, 0.1),
    (19, 2, 0.46, 0.5, 0.15, 0.1),
    (19, 3, 0.23, 0.5, 0.15, 0.1),
    (19, 4, 0.12, 0.5, 0.15, 0.1),
    (19, 5, 0.05, 0.5, 0.15, 0.1),
    (19, 6, 0.03, 0.5, 0.15, 0.1),
    (19, 7, 0.02, 0.5, 0.15, 0.1),
    (19, 8, 0.01, 0.5, 0.15, 0.1),
    (19, 9, 0.01, 0.5, 0.15, 0.1),
    (19, 10, 0.01, 0.5, 0.15, 0.1),
    (20, 1, 0.92, 0.5, 0.45, 0.1),
    (20, 2, 0.46, 0.5, 0.45, 0.1),
    (20, 3, 0.23, 0.5, 0.45, 0.1),
    (20, 4, 0.12, 0.5, 0.45, 0.1),
    (20, 5, 0.05, 0.5, 0.45, 0.1),
    (20, 6, 0.03, 0.5, 0.45, 0.1),
    (20, 7, 0.02, 0.5, 0.45, 0.1),
    (20, 8, 0.01, 0.5, 0.45, 0.1),
    (20, 9, 0.01, 0.5, 0.45, 0.1),
    (20, 10, 0.01, 0.5, 0.45, 0.1),
    (21, 1, 0.92, 0.5, 0.3, 0.1),
    (21, 2, 0.46, 0.5, 0.3, 0.1),
    (21, 3, 0.23, 0.5, 0.3, 0.1),
    (21, 4, 0.12, 0.5, 0.3, 0.1),
    (21, 5, 0.05, 0.5, 0.3, 0.1),
    (21, 6, 0.03, 0.5, 0.3, 0.1),
    (21, 7, 0.02, 0.5, 0.3, 0.1),
    (21, 8, 0.01, 0.5, 0.3, 0.1),
    (21, 9, 0.01, 0.5, 0.3, 0.1),
    (21, 10, 0.01, 0.5, 0.3, 0.1),
    (22, 1, 0.92, 0.15, 0.2, 0.1),
    (22, 2, 0.46, 0.15, 0.2, 0.1),
    (22, 3, 0.23, 0.15, 0.2, 0.1),
    (22, 4, 0.12, 0.15, 0.2, 0.1),
    (22, 5, 0.05, 0.15, 0.2, 0.1),
    (22, 6, 0.03, 0.15, 0.2, 0.1),
    (22, 7, 0.02, 0.15, 0.2, 0.1),
    (22, 8, 0.01, 0.15, 0.2, 0.1),
    (22, 9, 0.01, 0.15, 0.2, 0.1),
    (22, 10, 0.01, 0.15, 0.2, 0.1),
    (23, 1, 0.92, 0.45, 0.2, 0.1),
    (23, 2, 0.46, 0.45, 0.2, 0.1),
    (23, 3, 0.23, 0.45, 0.2, 0.1),
    (23, 4, 0.12, 0.45, 0.2, 0.1),
    (23, 5, 0.05, 0.45, 0.2, 0.1),
    (23, 6, 0.03, 0.45, 0.2, 0.1),
    (23, 7, 0.02, 0.45, 0.2, 0.1),
    (23, 8, 0.01, 0.45, 0.2, 0.1),
    (23, 9, 0.01, 0.45, 0.2, 0.1),
    (23, 10, 0.01, 0.45, 0.2, 0.1),
    (24, 1, 0.92, 0.3, 0.2, 0.1),
    (24, 2, 0.46, 0.3, 0.2, 0.1),
    (24, 3, 0.23, 0.3, 0.2, 0.1),
    (24, 4, 0.12, 0.3, 0.2, 0.1),
    (24, 5, 0.05, 0.3, 0.2, 0.1),
    (24, 6, 0.03, 0.3, 0.2, 0.1),
    (24, 7, 0.02, 0.3, 0.2, 0.1),
    (24, 8, 0.01, 0.3, 0.2, 0.1),
    (24, 9, 0.01, 0.3, 0.2, 0.1),
    (24, 10, 0.01, 0.3, 0.2, 0.1);

-- Definition for App Models
CREATE TABLE app_models (
    id INT unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(40) UNIQUE,
    package VARCHAR(150),
    interface JSON -- It is expected to be a object with structure{ param1: [data_type, unit] , ... }
);

INSERT INTO
    app_models(name, package, interface)
VALUES
    (
        "LocalApplication",
        "s2f.Applications.UserApps.LocalApplication",
        JSON_OBJECT(
            "inputDataSize",
            "int",
            "outputDataSize",
            "int",
            "inputFile",
            "string",
            "outputFile",
            "string",
            "MIs",
            "int",
            "iterations",
            "int"
        )
    );

-- Definition of App Instances: Could be seen as a docker repository
-- Parameters must match the "schema" of the App model { param1: data, ...}
CREATE TABLE apps (
    id INT unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(40) UNIQUE,
    model_id INT unsigned,
    parameters JSON,
    FOREIGN KEY (model_id) REFERENCES app_models(id)
);

INSERT INTO
    apps(name, model_id, parameters)
VALUES
    (
        "AppCPUIntensive",
        1,
        JSON_OBJECT(
            "inputDataSize",
            "10 MiB",
            "outputDataSize",
            "5 MiB",
            "inputFile",
            "/inputFile_0.dat",
            "outputFile",
            "/outputFile_0.dat",
            "MIs",
            "79200000",
            "iterations",
            "5"
        )
    ),
    (
        "AppDataIntensive",
        1,
        JSON_OBJECT(
            "inputDataSize",
            "50 MiB",
            "outputDataSize",
            "75 MiB",
            "inputFile",
            "/inputFile_0.dat",
            "outputFile",
            "/outputFile_0.dat",
            "MIs",
            "1000",
            "iterations",
            "5"
        )
    ),
    (
        "AppSmall",
        1,
        JSON_OBJECT(
            "inputDataSize",
            "1 MiB",
            "outputDataSize",
            "1 MiB",
            "inputFile",
            "/inputFile_0.dat",
            "outputFile",
            "/outputFile_0.dat",
            "MIs",
            "100",
            "iterations",
            "1"
        )
    );

-- Priority: 0 is Regular / 1 is Priority
CREATE TABLE users (
    id INT unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(40) UNIQUE,
    priority INT unsigned,
    sla_id INT unsigned,
    FOREIGN KEY (sla_id) REFERENCES app_models(id)
);

INSERT INTO
    users(name, priority, sla_id)
VALUES
    ("User_A", 0, 1),
    ("User_B", 0, 1),
    ("User_C", 0, 1),
    ("User_D", 0, 1),
    ("User_E", 0, 1),
    ("User_F", 0, 1),
    ("User_G", 0, 1),
    ("User_H", 0, 1),
    ("User_I", 0, 1),
    ("User_J", 0, 1);

-- CHECK THIS ! -- Does rent time refer to hours ?
CREATE TABLE user_vms (
    user_id INT unsigned,
    vm_id INT unsigned,
    instances INT unsigned,
    rent_time INT unsigned,
    PRIMARY KEY (user_id, vm_id),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (vm_id) REFERENCES vms(id)
);

INSERT INTO
    user_vms(user_id, vm_id, instances, rent_time)
VALUES
    (1, 6, 5, 2),
    (2, 4, 5, 3),
    (2, 5, 5, 2),
    (3, 5, 2, 2),
    (4, 5, 50, 2),
    (5, 4, 1, 10),
    (6, 7, 2, 2),
    (6, 6, 2, 3),
    (7, 7, 2, 2),
    (7, 5, 2, 1),
    (7, 6, 2, 2),
    (8, 9, 1, 24),
    (9, 10, 1, 1),
    (10, 4, 5, 3),
    (10, 5, 5, 3),
    (10, 6, 5, 2);

CREATE TABLE user_apps (
    user_id INT unsigned,
    app_id INT unsigned,
    instances INT unsigned,
    PRIMARY KEY (user_id, app_id),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (app_id) REFERENCES apps(id)
);

INSERT INTO
    user_apps(user_id, app_id, instances)
VALUES
    (1, 1, 1),
    (2, 1, 1),
    (3, 1, 1),
    (4, 1, 1),
    (5, 1, 1),
    (6, 1, 1),
    (7, 1, 1),
    (8, 1, 1),
    (9, 1, 1),
    (10, 1, 1);

-- This is the most interesting table
--      Determines the users
--      Could determine the RNG seed/s
--      Could (and should) determine the network to be simulated
CREATE TABLE experiment (
    id INT unsigned NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(40) UNIQUE
);

INSERT INTO
    experiment(name)
VALUES
    ("Test");

CREATE TABLE experiment_users (
    experiment_id INT unsigned,
    user_id INT unsigned,
    instances INT unsigned,
    PRIMARY KEY (experiment_id, user_id),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (experiment_id) REFERENCES experiment(id)
);

INSERT INTO
    experiment_users
VALUES
    (1, 1, 1);

SELECT
    users.name,
    users.priority,
    sla.type as sla_type,
    experiment_users.instances
FROM
    experiment
    JOIN experiment_users ON experiment.id = experiment_users.experiment_id
    JOIN users ON experiment_users.user_id = users.id
    JOIN sla ON users.sla_id = sla.id
WHERE
    users.name = "User_A"
    AND experiment.name = "Test";

SELECT
    users.name,
    users.priority,
    sla.type as sla_type,
    experiment_users.instances
FROM
    experiment
    JOIN experiment_users ON experiment.id = experiment_users.experiment_id
    JOIN users ON experiment_users.user_id = users.id
    JOIN sla ON users.sla_id = sla.id
WHERE
    experiment.name = "Test";

SELECT
    user_apps.instances,
    apps.name
FROM
    users
    JOIN user_apps ON users.id = user_apps.user_id
    JOIN apps ON apps.id = user_apps.app_id
WHERE
    users.name = "User_A";

-- Después ciclar searchApp -> caching helps
SELECT
    user_vms.instances,
    user_vms.rent_time,
    vms.type
FROM
    users
    JOIN user_vms ON users.id = user_vms.user_id
    JOIN vms ON vms.id = user_vms.vm_id
WHERE
    users.name = "User_A";

-- Después ciclar searchApp -> caching helps