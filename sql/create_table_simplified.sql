CREATE TABLE simplified_trajectories (
                              id             BIGSERIAL PRIMARY KEY,
                              trajectory_id  INTEGER NOT NULL,
                              coordinates    POINT NOT NULL,
                              time           BIGINT
);