CREATE TABLE simplified_trajectories (
                              id             SERIAL PRIMARY KEY,
                              trajectory_id  INTEGER NOT NULL,
                              coordinates    POINT NOT NULL,
                              time           DECIMAL(10,7)
);