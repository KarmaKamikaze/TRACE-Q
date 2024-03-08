#ifndef TRACE_Q_SQLITE_QUERYING_HPP
#define TRACE_Q_SQLITE_QUERYING_HPP

enum query_purpose { insert_into_trajectory_table, insert_into_rtree_table, load_trajectory_information_into_datastructure, create_table, reset_database};

void run_sql(const char* query, query_purpose callback_type);

#endif //TRACE_Q_SQLITE_QUERYING_HPP
