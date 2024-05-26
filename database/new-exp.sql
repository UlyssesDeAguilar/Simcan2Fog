INSERT INTO
    experiment(name)
VALUES
    ("Test2");

INSERT INTO
    experiment_users
VALUES
    (2, 1, 100),
    (2, 2, 100),
    (2, 3, 100),
    (2, 4, 100),
    (2, 5, 100),
    (2, 6, 100),
    (2, 7, 100),
    (2, 8, 100),
    (2, 9, 100),
    (2, 10, 100);

SELECT
    *
FROM
    experiment
    JOIN experiment_users ON experiment.id = experiment_users.experiment_id
    JOIN users ON experiment_users.user_id = users.id
WHERE
    experiment.name = "Test2";


INSERT INTO
    experiment(name)
VALUES
    ("Test3");

INSERT INTO
    experiment_users
VALUES
    (3, 1, 100);