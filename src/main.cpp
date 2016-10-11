/*
 * Copyright (C) 2016 Hops.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* 
 * File:   main.cpp
 * Author: Mahmoud Ismail<maism@kth.se>
 *
 */
#include "Notifier.h"
#include <boost/program_options.hpp>
#include "Version.h"

namespace po = boost::program_options;

int main(int argc, char** argv) { 
    
    string connection_string;
    string database_name = "hops";
    string meta_database_name = "hopsworks";
    int wait_time = 1000;
    int ndb_batch = 5;
    int poll_maxTimeToWait = 2000;
    int num_ndb_readers = 5;
    string elastic_addr = "localhost:9200";
    int log_level = 2;
    
    bool hopsworks = true;
    string elastic_index = "projects";
    string elastic_project_type = "proj";
    string elastic_dataset_type = "ds";
    string elastic_inode_type = "inode";
    int elastic_batch_size = 5000;
    int elastic_issue_time = 5000;
    
    int lru_cap = DEFAULT_MAX_CAPACITY;
    bool recovery = true;
    bool stats = false;
    MetadataType metadata_type = Both;
    
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("connection", po::value<string>(), "connection string/ ndb host")
            ("database", po::value<string>()->default_value(database_name), "database name.")
            ("meta_database", po::value<string>()->default_value(meta_database_name), "database name for metadata")
            ("poll_maxTimeToWait", po::value<int>()->default_value(wait_time), "max time to wait in miliseconds while waiting for events in pollEvents")
            ("wait_time", po::value<int>()->default_value(poll_maxTimeToWait), "time to wait in miliseconds before issuing the ndb request if the batch size wasn't reached")
            ("ndb_batch", po::value<int>()->default_value(ndb_batch), "batch size for reading from ndb")
            ("num_ndb_readers", po::value<int>()->default_value(num_ndb_readers), "num of ndb reader threads")
            ("elastic_addr", po::value<string>()->default_value(elastic_addr), "ip and port of the elasticsearch server")
            ("hopsworks", po::value<bool>()->default_value(hopsworks), "enable or disable hopsworks, which will use grandparents to index inodes and metadata")
            ("index", po::value<string>()->default_value(elastic_index), "Elastic index to add the data to.")
            ("project_type", po::value<string>()->default_value(elastic_project_type), "Elastic type for projects, only used when hopsworks is enabled.")
            ("dataset_type", po::value<string>()->default_value(elastic_dataset_type), "Elastic type for datasets, only used when hopsworks is enabled.")
            ("inode_type", po::value<string>()->default_value(elastic_inode_type), "Elastic type for inodes.")
            ("elastic_batch", po::value<int>()->default_value(elastic_batch_size), "Elastic batch size in bytes for bulk requests")
            ("ewait_time", po::value<int>()->default_value(elastic_issue_time), "time to wait in miliseconds before issuing a bulk request to Elasticsearch if the batch size wasn't reached")
            ("lru_cap", po::value<int>()->default_value(lru_cap), "LRU Cache max capacity")
            ("log_level", po::value<int>()->default_value(log_level), "log level trace=0, debug=1, info=2, warn=3, error=4, fatal=5")
            ("recovery", po::value<bool>()->default_value(recovery), "enable or disable startup recovery")
            ("stats", po::value<bool>()->default_value(stats), "enable or disable print of accumulators stats")
            ("metadata_type", po::value<int>()->default_value(metadata_type), "Extended metadata type; SchemaBased=0, Schemaless=1, Both=2")
            ("version", "ePipe version")
            ;

   
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("version")) {
        cout << "ePipe " << EPIPE_VERSION_MAJOR << "."  << EPIPE_VERSION_MINOR 
                << "." << EPIPE_VERSION_BUILD << endl;
        return EXIT_SUCCESS;
    }
    
    if (vm.count("connection")) {
        connection_string = vm["connection"].as<string>();
    }

    if (vm.count("database")) {
        database_name = vm["database"].as<string>();
    }

    if (vm.count("meta_database")) {
        meta_database_name = vm["meta_database"].as<string>();
    }
    
    if (vm.count("log_level")) {
        log_level = vm["log_level"].as<int>();
    }

    if (vm.count("wait_time")) {
        wait_time = vm["wait_time"].as<int>();
    }

    if (vm.count("ndb_batch")) {
        ndb_batch = vm["ndb_batch"].as<int>();
    }

    if (vm.count("poll_maxTimeToWait")) {
        poll_maxTimeToWait = vm["poll_maxTimeToWait"].as<int>();
    }

    if (vm.count("num_ndb_readers")) {
        num_ndb_readers = vm["num_ndb_readers"].as<int>();
    }
    
    if(vm.count("elastic_addr")){
        elastic_addr = vm["elastic_addr"].as<string>();
    }
    
    if(vm.count("hopsworks")){
        hopsworks = vm["hopsworks"].as<bool>();
    }
    
     if(vm.count("index")){
        elastic_index = vm["index"].as<string>();
    }
    
    if(vm.count("project_type")){
        elastic_project_type = vm["project_type"].as<string>();
    }
    
    if(vm.count("dataset_type")){
        elastic_dataset_type = vm["dataset_type"].as<string>();
    }
    
    if(vm.count("inode_type")){
        elastic_inode_type = vm["inode_type"].as<string>();
    }
    
    if (vm.count("elastic_batch")) {
        elastic_batch_size = vm["elastic_batch"].as<int>();
    }
    
    if (vm.count("ewait_time")) {
        elastic_issue_time = vm["ewait_time"].as<int>();
    }
    
    if (vm.count("lru_cap")) {
        lru_cap = vm["lru_cap"].as<int>();
    }
    
    if(vm.count("recovery")){
        recovery = vm["recovery"].as<bool>();
    }
    
    if(vm.count("stats")){
        stats = vm["stats"].as<bool>();
    }
    
    if(vm.count("metadata_type")){
        metadata_type = static_cast<MetadataType>(vm["metadata_type"].as<int>());
    }
    
    Logger::setLoggerLevel(log_level);
            
    if(connection_string.empty() || database_name.empty() || meta_database_name.empty()){
        LOG_ERROR("you should provide at least connection, database, meta_database");
        return EXIT_FAILURE;
    }

    Notifier *notifer = new Notifier(connection_string.c_str(), database_name.c_str(), 
            meta_database_name.c_str(), wait_time, ndb_batch, poll_maxTimeToWait, 
            num_ndb_readers, elastic_addr, hopsworks, elastic_index, elastic_project_type, 
            elastic_dataset_type, elastic_inode_type, elastic_batch_size, elastic_issue_time,
            lru_cap, recovery, stats, metadata_type);
    notifer->start();

    return EXIT_SUCCESS;
}
