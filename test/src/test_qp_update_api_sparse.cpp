#include <qp/sparse/wrapper.hpp>
#include <util.hpp>
#include <doctest.h>
#include <veg/util/dynstack_alloc.hpp>

using namespace qp;
using T = double;
using I = c_int;
using namespace linearsolver::sparse::tags;

TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test update rho") {
        std::cout << "------------------------sparse random strongly convex qp with equality and inequality constraints: test update rho" << std::endl;
	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 0.15;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);
		auto A = ldlt_test::rand::sparse_matrix_rand<T>(n_eq,n, p);
                auto x_sol = ldlt_test::rand::vector_rand<T>(n);
	        auto b = A * x_sol;
		auto C = ldlt_test::rand::sparse_matrix_rand<T>(n_in,n, p);
		auto l =  C * x_sol; 
                auto u = (l.array() + 100).matrix().eval();

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.settings.verbose = false;
        Qp.init(H,g,A,b,C,u,l);
        Qp.update_proximal_parameters(T(1.e-7), tl::nullopt, tl::nullopt);
        std::cout << "after upating" << std::endl;
        std::cout << "rho :  " << Qp.results.info.rho << std::endl;
        Qp.solve();
        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;

        /*
        Qp.cleanup();
        Qp.setup_sparse_matrices(H,g,A,b,C,u,l);
        Qp.update_proximal_parameters(T(1.e-7), tl::nullopt, tl::nullopt);
        std::cout << "after upating" << std::endl;
        std::cout << "rho :  " << Qp.results.info.rho << std::endl;

        Qp.solve();

        CHECK(
                qp::dense::infty_norm(
                        H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) <=
                1e-9);
        CHECK(qp::dense::infty_norm(A * Qp.results.x - b) <= 1e-9);
        if (n_in > 0) {
            CHECK((C * Qp.results.x - l).minCoeff() > -1e-9);
            CHECK((C * Qp.results.x - u).maxCoeff() < 1e-9);
        }
        */
	}
}

TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test update mus") {

        std::cout << "------------------------sparse random strongly convex qp with equality and inequality constraints: test update mus" << std::endl;
	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 1.0;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);
		auto A = ldlt_test::rand::sparse_matrix_rand<T>(n_eq,n, p);
                auto x_sol = ldlt_test::rand::vector_rand<T>(n);
	        auto b = A * x_sol;
		auto C = ldlt_test::rand::sparse_matrix_rand<T>(n_in,n, p);
		auto l =  C * x_sol; 
                auto u = (l.array() + 100).matrix().eval();

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.init(H,g,A,b,C,u,l);
        Qp.update_proximal_parameters(tl::nullopt, T(1.E-2), T(1.E-3));
        std::cout << "after upating" << std::endl;
        std::cout << "mu_eq :  " << Qp.results.info.mu_eq << std::endl;
        std::cout << "mu_in :  " << Qp.results.info.mu_in << std::endl;
        Qp.solve();

        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;

        /*
        Qp.cleanup();
        Qp.setup_sparse_matrices(H,g,A,b,C,u,l);
        Qp.update_proximal_parameter(tl::nullopt, T(1.E-2), T(1.E-3));
        std::cout << "after upating" << std::endl;
        std::cout << "mu_eq :  " << Qp.results.info.mu_eq << std::endl;
        std::cout << "mu_in :  " << Qp.results.info.mu_in << std::endl;

        Qp.solve();

        CHECK(
                qp::dense::infty_norm(
                        H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) <=
                1e-9);
        CHECK(qp::dense::infty_norm(A * Qp.results.x - b) <= 1e-9);
        if (n_in > 0) {
            CHECK((C * Qp.results.x - l).minCoeff() > -1e-9);
            CHECK((C * Qp.results.x - u).maxCoeff() < 1e-9);
        }
        */
	}
}
TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test with no equilibration at initialization") {

        std::cout << "------------------------sparse random strongly convex qp with equality and inequality constraints: test with no equilibration at initialization" << std::endl;
	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 0.15;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);
		auto A = ldlt_test::rand::sparse_matrix_rand<T>(n_eq,n, p);
                auto x_sol = ldlt_test::rand::vector_rand<T>(n);
	        auto b = A * x_sol;
		auto C = ldlt_test::rand::sparse_matrix_rand<T>(n_in,n, p);
		auto l =  C * x_sol; 
                auto u = (l.array() + 100).matrix().eval();

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.init(H,g,A,b,C,u,l,false);
        Qp.solve();

        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;
	}
}

TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test with equilibration at initialization") {

        std::cout << "------------------------sparse random strongly convex qp with equality and inequality constraints: test with equilibration at initialization" << std::endl;
	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 0.15;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);
		auto A = ldlt_test::rand::sparse_matrix_rand<T>(n_eq,n, p);
                auto x_sol = ldlt_test::rand::vector_rand<T>(n);
	        auto b = A * x_sol;
		auto C = ldlt_test::rand::sparse_matrix_rand<T>(n_in,n, p);
		auto l =  C * x_sol; 
                auto u = (l.array() + 100).matrix().eval();

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.init(H,g,A,b,C,u,l,true);
        Qp.solve();

        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;
	}
}
TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test with no initial guess") {

        std::cout << "------------------------sparse random strongly convex qp with equality and inequality constraints: test with no initial guess" << std::endl;
	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 0.15;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);
		auto A = ldlt_test::rand::sparse_matrix_rand<T>(n_eq,n, p);
                auto x_sol = ldlt_test::rand::vector_rand<T>(n);
	        auto b = A * x_sol;
		auto C = ldlt_test::rand::sparse_matrix_rand<T>(n_in,n, p);
		auto l =  C * x_sol; 
                auto u = (l.array() + 100).matrix().eval();

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.settings.initial_guess = proxsuite::qp::InitialGuessStatus::NO_INITIAL_GUESS;
        Qp.init(H,g,A,b,C,u,l);
        Qp.solve();

        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;
	}
}
TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test update g for unconstrained problem") {

        std::cout << "------------------------sparse random strongly convex qp with equality and inequality constraints: test with no initial guess" << std::endl;
	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 0.15;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.settings.initial_guess = proxsuite::qp::InitialGuessStatus::NO_INITIAL_GUESS;
        Qp.init(H,g,A,b,C,u,l);
        Qp.solve();

        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;
	}
}
/*
TEST_CASE("sparse random strongly convex qp with equality and "
                  "inequality constraints: test warm starting --> TO DEBUG") {

	for (auto const& dims : {
					 //veg::tuplify(50, 0, 0),
					 //veg::tuplify(50, 25, 0),
					 //veg::tuplify(10, 0, 10),
					 //veg::tuplify(50, 0, 25),
					 //veg::tuplify(50, 10, 25),
                                         veg::tuplify(10, 2, 2)
			 }) {
		VEG_BIND(auto const&, (n, n_eq, n_in), dims);

		double p = 0.15;

		auto H = ldlt_test::rand::sparse_positive_definite_rand(n, T(10.0), p);
		auto g = ldlt_test::rand::vector_rand<T>(n);
		auto A = ldlt_test::rand::sparse_matrix_rand<T>(n_eq,n, p);
                auto x_sol = ldlt_test::rand::vector_rand<T>(n);
	        auto b = A * x_sol;
		auto C = ldlt_test::rand::sparse_matrix_rand<T>(n_in,n, p);
		auto l =  C * x_sol; 
                auto u = (l.array() + 100).matrix().eval();

        qp::sparse::QP<T,I> Qp(n, n_eq, n_in);
        Qp.settings.eps_abs = 1.E-9;
        Qp.settings.initial_guess = proxsuite::qp::InitialGuessStatus::WARM_START;
        Qp.init(H,g,A,b,C,u,l);
        auto x_wm = ldlt_test::rand::vector_rand<T>(n);
        auto y_wm = ldlt_test::rand::vector_rand<T>(n_eq);
        auto z_wm = ldlt_test::rand::vector_rand<T>(n_in);
        std::cout << "proposed warm start" << std::endl;
        std::cout << "x_wm :  " << x_wm << std::endl;
        std::cout << "y_wm :  " << y_wm << std::endl;
        std::cout << "z_wm :  " << z_wm << std::endl;
        Qp.solve(x_wm,y_wm,z_wm);

        T dua_res = qp::dense::infty_norm(H.selfadjointView<Eigen::Upper>() * Qp.results.x + g + A.transpose() * Qp.results.y + C.transpose() * Qp.results.z) ;
        T pri_res = std::max( qp::dense::infty_norm(A * Qp.results.x - b),
			qp::dense::infty_norm(sparse::detail::positive_part(C * Qp.results.x - u) + sparse::detail::negative_part(C * Qp.results.x - l)));
        CHECK(dua_res <= 1e-9);
        CHECK(pri_res <= 1E-9);
        std::cout << "--n = " << n << " n_eq " << n_eq << " n_in " << n_in << std::endl;
        std::cout  << "; dual residual " << dua_res << "; primal residual " <<  pri_res << std::endl;
	std::cout << "total number of iteration: " << Qp.results.info.iter << std::endl;
	std::cout << "setup timing " << Qp.results.info.setup_time << " solve time " << Qp.results.info.solve_time << std::endl;

	}
}
*/