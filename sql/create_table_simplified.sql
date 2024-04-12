CREATE TABLE simplified_trajectories (
                              id             BIGSERIAL PRIMARY KEY,
                              trajectory_id  INTEGER NOT NULL,
                              coordinates    POINT NOT NULL,
                              time           BIGINT
);
CREATE INDEX simplified_trajectories_index_coords ON simplified_trajectories USING GIST (coordinates);
CREATE INDEX simplified_trajectories_index_time ON simplified_trajectories (time);
