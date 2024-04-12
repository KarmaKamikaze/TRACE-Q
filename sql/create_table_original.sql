CREATE TABLE original_trajectories (
                              id             BIGSERIAL PRIMARY KEY,
                              trajectory_id  INTEGER NOT NULL,
                              coordinates    POINT NOT NULL,
                              time           BIGINT
);
CREATE INDEX original_trajectories_index_coords ON original_trajectories USING GIST (coordinates);
CREATE INDEX original_trajectories_index_time ON original_trajectories (time);