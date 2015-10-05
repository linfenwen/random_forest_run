// compile with the following two options:
// -lboost_unit_test_framework -DBOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rfr_test
#include <boost/test/unit_test.hpp>

#include <random>


#include "rfr/data_containers/mostly_continuous_data_container.hpp"
#include "rfr/splits/binary_split_one_feature_rss_loss.hpp"
#include "rfr/nodes/temporary_node.hpp"
#include "rfr/nodes/k_ary_node.hpp"
#include "rfr/trees/tree_options.hpp"
#include "rfr/trees/k_ary_tree.hpp"
#include "rfr/forests/regression_forest.hpp"
#include "rfr/forests/forest_options.hpp"



typedef double num_type;
typedef double response_type;
typedef unsigned int index_type;
typedef std::default_random_engine rng_type;

typedef rfr::mostly_continuous_data<num_type, response_type, index_type> data_container_type;

typedef rfr::binary_split_one_feature_rss_loss<rng_type, num_type, response_type, index_type> split_type;
typedef rfr::k_ary_node<2, split_type, rng_type, num_type, response_type, index_type> node_type;
typedef rfr::temporary_node<num_type, index_type> tmp_node_type;

typedef rfr::k_ary_random_tree<2, split_type, rng_type, num_type, response_type, index_type> tree_type;


BOOST_AUTO_TEST_CASE( data_container_tests ){
	rfr::mostly_contiuous_data<num_type,response_type, index_type> data;

    char *filename = (char*) malloc(1024*sizeof(char));

    strcpy(filename, boost::unit_test::framework::master_test_suite().argv[1]);
    //strcat(filename, "toy_data_set_features.csv");
    strcat(filename, "diabetes_features.csv");
    std::cout<<filename<<"\n";
    data.read_feature_file(filename);

    strcpy(filename, boost::unit_test::framework::master_test_suite().argv[1]);
	//strcat(filename, "toy_data_set_responses.csv");
	strcat(filename, "diabetes_responses.csv");
	std::cout<<filename<<"\n";
    data.read_response_file(filename);

	rfr::tree_options<num_type, response_type, index_type> tree_opts;
	tree_opts.min_samples_to_split = 10;
	tree_opts.min_samples_in_leaf = 5;
	tree_opts.max_features = 10;

	
	rfr::forest_options<num_type, response_type, index_type> forest_opts(tree_opts);

	forest_opts.num_data_points_per_tree = 2*data.num_data_points();
	forest_opts.num_trees = 10;
	forest_opts.do_bootstrapping = true;

	rfr::regression_forest< tree_type, rng_type, num_type, response_type, index_type> the_forest(forest_opts);
	
	rng_type rng;

	the_forest.fit(data, rng);
	the_forest.print_info();

	std::tuple<response_type, num_type> tmp;
	std::vector<response_type> data_point_five = data.retrieve_data_point(5);
	tmp = the_forest.predict_mean_std(data_point_five.data());

	the_forest.save_latex_representation("/tmp/tree");

	std::cout<<std::get<0>(tmp) <<" "<<std::get<1>(tmp)<<"\n";
    free(filename);
}
